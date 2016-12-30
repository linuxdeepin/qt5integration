/**
 * Copyright (C) 2016 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#include "windoweventhook.h"
#include "vtablehook.h"
#include "utility.h"

#define private public
#define protected public
#include "qxcbdrag.h"
#undef private
#undef protected

#include <QDrag>
#include <QMimeData>

#include <private/qguiapplication_p.h>

DPP_BEGIN_NAMESPACE

WindowEventHook::WindowEventHook(QXcbWindow *window)
{
    VtableHook::overrideVfptrFun(window, &QXcbWindowEventListener::handleClientMessageEvent,
                                 this, &WindowEventHook::handleClientMessageEvent);

    QObject::connect(window->window(), &QWindow::destroyed, window->window(), [this, window] {
        delete this;
        VtableHook::clearGhostVtable(static_cast<QPlatformWindow*>(window));
    });
}

//#define DND_DEBUG
#ifdef DND_DEBUG
#define DEBUG qDebug
#else
#define DEBUG if(0) qDebug
#endif

#ifdef DND_DEBUG
#define DNDDEBUG qDebug()
#else
#define DNDDEBUG if(0) qDebug()
#endif

static inline xcb_window_t xcb_window(QWindow *w)
{
    return static_cast<QXcbWindow *>(w->handle())->xcb_window();
}

xcb_atom_t toXdndAction(const QXcbDrag *drag, Qt::DropAction a)
{
    switch (a) {
    case Qt::CopyAction:
        return drag->atom(QXcbAtom::XdndActionCopy);
    case Qt::LinkAction:
        return drag->atom(QXcbAtom::XdndActionLink);
    case Qt::MoveAction:
    case Qt::TargetMoveAction:
        return drag->atom(QXcbAtom::XdndActionMove);
    case Qt::IgnoreAction:
        return XCB_NONE;
    default:
        return drag->atom(QXcbAtom::XdndActionCopy);
    }
}

void WindowEventHook::handleClientMessageEvent(const xcb_client_message_event_t *event)
{
    QXcbWindow *me = window();

    if (event->format == 32 && event->type == me->atom(QXcbAtom::XdndDrop)) {
        QXcbDrag *drag = me->connection()->drag();

        DEBUG("xdndHandleDrop");
        if (!drag->currentWindow) {
            drag->xdnd_dragsource = 0;
            return; // sanity
        }

        const uint32_t *l = event->data.data32;

        DEBUG("xdnd drop");

        if (l[0] != drag->xdnd_dragsource) {
            DEBUG("xdnd drop from unexpected source (%x not %x", l[0], drag->xdnd_dragsource);
            return;
        }

        // update the "user time" from the timestamp in the event.
        if (l[2] != 0)
            drag->target_time = /*X11->userTime =*/ l[2];

        Qt::DropActions supported_drop_actions;
        QMimeData *dropData = 0;
        if (drag->currentDrag()) {
            dropData = drag->currentDrag()->mimeData();
            supported_drop_actions = Qt::DropActions(l[4]);
        } else {
            dropData = drag->platformDropData();
            supported_drop_actions = drag->accepted_drop_action;

            // Drop coming from another app? Update keyboard modifiers.
            QGuiApplicationPrivate::modifier_buttons = QGuiApplication::queryKeyboardModifiers();
        }

        if (!dropData)
            return;
        // ###
        //        int at = findXdndDropTransactionByTime(target_time);
        //        if (at != -1)
        //            dropData = QDragManager::dragPrivate(X11->dndDropTransactions.at(at).object)->data;
        // if we can't find it, then use the data in the drag manager

        bool directSaveMode = dropData->hasFormat("XdndDirectSave0");

        dropData->setProperty("IsDirectSaveMode", directSaveMode);

        QPlatformDropQtResponse response = QWindowSystemInterface::handleDrop(drag->currentWindow.data(),
                                                                              dropData, drag->currentPosition,
                                                                              supported_drop_actions);
        drag->setExecutedDropAction(response.acceptedAction());

        if (directSaveMode) {
            const QUrl &url = dropData->property("DirectSaveUrl").toUrl();

            if (url.isValid() && drag->xdnd_dragsource) {
                xcb_atom_t XdndDirectSaveAtom = me->connection()->internAtom("XdndDirectSave0");
                xcb_atom_t textAtom = me->connection()->internAtom("text/plain");
                QByteArray basename = Utility::windowProperty(drag->xdnd_dragsource, XdndDirectSaveAtom, textAtom, 1024);
                QByteArray fileUri = url.toString().toLocal8Bit() + "/" + basename;

                Utility::setWindowProperty(drag->xdnd_dragsource, XdndDirectSaveAtom,
                                           textAtom, fileUri.constData(), fileUri.length());

                Q_UNUSED(dropData->data("XdndDirectSave0"));
            }
        }

        xcb_client_message_event_t finished;
        finished.response_type = XCB_CLIENT_MESSAGE;
        finished.sequence = 0;
        finished.window = drag->xdnd_dragsource;
        finished.format = 32;
        finished.type = drag->atom(QXcbAtom::XdndFinished);
        finished.data.data32[0] = drag->currentWindow ? xcb_window(drag->currentWindow.data()) : XCB_NONE;
        finished.data.data32[1] = response.isAccepted(); // flags
        finished.data.data32[2] = toXdndAction(drag, response.acceptedAction());
        Q_XCB_CALL(xcb_send_event(drag->xcb_connection(), false, drag->current_proxy_target,
                                  XCB_EVENT_MASK_NO_EVENT, (char *)&finished));

        drag->xdnd_dragsource = 0;
        drag->currentWindow.clear();
        drag->waiting_for_status = false;

        // reset
        drag->target_time = XCB_CURRENT_TIME;
    } else {
        me->QXcbWindow::handleClientMessageEvent(event);
    }
}

DPP_END_NAMESPACE
