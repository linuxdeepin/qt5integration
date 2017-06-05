/**
 * Copyright (C) 2017 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/
#include "dforeignplatformwindow.h"
#include "dplatformintegration.h"
#include "global.h"
#include "utility.h"

#include "qxcbconnection.h"

#include <QDebug>

#include <private/qwindow_p.h>

#include <xcb/xcb_icccm.h>

DPP_BEGIN_NAMESPACE

DForeignPlatformWindow::DForeignPlatformWindow(QWindow *window)
    : QXcbWindow(window)
{
    create();

    m_dirtyFrameMargins = true;

    init();
}

DForeignPlatformWindow::~DForeignPlatformWindow()
{
    qt_window_private(window())->windowFlags = Qt::ForeignWindow;
}

QRect DForeignPlatformWindow::geometry() const
{
    xcb_connection_t *conn = DPlatformIntegration::xcbConnection()->xcb_connection();
    xcb_get_geometry_reply_t *geomReply =
        xcb_get_geometry_reply(conn, xcb_get_geometry(conn, m_window), 0);
    if (!geomReply)
        return QRect();

    xcb_translate_coordinates_reply_t *translateReply =
        xcb_translate_coordinates_reply(conn, xcb_translate_coordinates(conn, m_window, DPlatformIntegration::xcbConnection()->rootWindow(), 0, 0), 0);
    if (!translateReply) {
        free(geomReply);
        return QRect();
    }

    const QRect result(QPoint(translateReply->dst_x, translateReply->dst_y), QSize(geomReply->width, geomReply->height));
    free(translateReply);

    // auto remove _GTK_FRAME_EXTENTS
    xcb_get_property_cookie_t cookie = xcb_get_property(xcb_connection(), false, m_window,
                                                        Utility::internAtom("_GTK_FRAME_EXTENTS"), XCB_ATOM_CARDINAL, 0, 4);
    QScopedPointer<xcb_get_property_reply_t, QScopedPointerPodDeleter> reply(
        xcb_get_property_reply(xcb_connection(), cookie, NULL));
    if (reply && reply->type == XCB_ATOM_CARDINAL && reply->format == 32 && reply->value_len == 4) {
        quint32 *data = (quint32 *)xcb_get_property_value(reply.data());
        // _NET_FRAME_EXTENTS format is left, right, top, bottom
        return result.marginsRemoved(QMargins(data[0], data[2], data[1], data[3]));
    }

    return result;
}

void DForeignPlatformWindow::updateTitle()
{
    xcb_get_property_reply_t *wm_name =
        xcb_get_property_reply(xcb_connection(),
            xcb_get_property_unchecked(xcb_connection(), false, m_window,
                             atom(QXcbAtom::_NET_WM_NAME),
                             atom(QXcbAtom::UTF8_STRING), 0, 1024), NULL);
    if (wm_name && wm_name->format == 8
            && wm_name->type == atom(QXcbAtom::UTF8_STRING)) {
        qt_window_private(window())->windowTitle = QString::fromUtf8((const char *)xcb_get_property_value(wm_name), xcb_get_property_value_length(wm_name));
    }

    free(wm_name);
}

void DForeignPlatformWindow::updateWmClass()
{
    xcb_get_property_reply_t *wm_class =
        xcb_get_property_reply(xcb_connection(),
            xcb_get_property(xcb_connection(), 0, m_window, XCB_ATOM_WM_CLASS, XCB_ATOM_STRING, 0L, 2048L), NULL);
    if (wm_class && wm_class->format == 8
            && wm_class->type == XCB_ATOM_STRING) {
        const QByteArray wm_class_name((const char *)xcb_get_property_value(wm_class), xcb_get_property_value_length(wm_class));
        const QList<QByteArray> wm_class_name_list = wm_class_name.split('\0');

        if (!wm_class_name_list.isEmpty())
            window()->setProperty(WmClass, QString::fromLocal8Bit(wm_class_name_list.first()));
    }

    free(wm_class);
}

void DForeignPlatformWindow::updateWmDesktop()
{
    window()->setProperty(WmNetDesktop, Utility::getWorkspaceForWindow(m_window));
}

void DForeignPlatformWindow::updateWindowState()
{
    Qt::WindowState newState = Qt::WindowNoState;
    const xcb_get_property_cookie_t get_cookie =
    xcb_get_property(xcb_connection(), 0, m_window, atom(QXcbAtom::WM_STATE),
                     XCB_ATOM_ANY, 0, 1024);

    xcb_get_property_reply_t *reply =
        xcb_get_property_reply(xcb_connection(), get_cookie, NULL);

    if (reply && reply->format == 32 && reply->type == atom(QXcbAtom::WM_STATE)) {
        const quint32 *data = (const quint32 *)xcb_get_property_value(reply);
        if (reply->length != 0 && XCB_ICCCM_WM_STATE_ICONIC == data[0])
            newState = Qt::WindowMinimized;
    }
    free(reply);

    if (newState != Qt::WindowMinimized) { // Something else changed, get _NET_WM_STATE.
        const NetWmStates states = netWmStates();
        if (states & NetWmStateFullScreen)
            newState = Qt::WindowFullScreen;
        else if ((states & NetWmStateMaximizedHorz) && (states & NetWmStateMaximizedVert))
            newState = Qt::WindowMaximized;
    }

    m_windowState = newState;
    qt_window_private(window())->windowState = newState;
    qt_window_private(window())->updateVisibility();
}

void DForeignPlatformWindow::updateWindowTypes()
{
    QXcbWindowFunctions::WmWindowTypes window_types = wmWindowTypes();
    Qt::WindowFlags window_flags = 0;

    if (window_types & QXcbWindowFunctions::Normal)
            window_flags |= Qt::Window;
    else if (window_types & QXcbWindowFunctions::Desktop)
        window_flags |= Qt::Desktop;
    else if (window_types & QXcbWindowFunctions::Dialog)
        window_flags |= Qt::Dialog;
    else if (window_types & QXcbWindowFunctions::Utility)
        window_flags |= Qt::Tool;
    else if (window_types & QXcbWindowFunctions::Tooltip)
        window_flags |= Qt::ToolTip;
    else if (window_types & QXcbWindowFunctions::Splash)
        window_flags |= Qt::SplashScreen;
    else
        window_flags |= Qt::Widget;

    if (window_types & QXcbWindowFunctions::KdeOverride)
        window_flags |= Qt::FramelessWindowHint;

    qt_window_private(window())->windowFlags = window_flags;
    window()->setProperty(WmWindowTypes, (quint32)window_types);
}

void DForeignPlatformWindow::updateProcessId()
{
    xcb_get_property_cookie_t cookie = xcb_get_property(xcb_connection(), false, m_window,
                                                        atom(QXcbAtom::_NET_WM_PID), XCB_ATOM_CARDINAL, 0, 1);
    QScopedPointer<xcb_get_property_reply_t, QScopedPointerPodDeleter> reply(
        xcb_get_property_reply(xcb_connection(), cookie, NULL));
    if (reply && reply->type == XCB_ATOM_CARDINAL && reply->format == 32 && reply->value_len == 1) {
        window()->setProperty(ProcessId, *(quint32 *)xcb_get_property_value(reply.data()));
    }
}

void DForeignPlatformWindow::init()
{
    updateTitle();
    updateWindowState();
    updateWindowTypes();
    updateWmClass();
    updateWmDesktop();
    updateProcessId();

//    m_mapped = Utility::getWindows().contains(m_window);
//    qt_window_private(window())->visible = m_mapped;
}

DPP_END_NAMESPACE
