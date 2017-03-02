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
#include "qxcbkeyboard.h"
#undef private
#undef protected

#include <QDrag>
#include <QMimeData>

#include <private/qguiapplication_p.h>
#include <private/qwindow_p.h>

#include <X11/extensions/XI2proto.h>

DPP_BEGIN_NAMESPACE

PUBLIC_CLASS(QXcbWindow, WindowEventHook);

WindowEventHook::WindowEventHook(QXcbWindow *window)
{
    VtableHook::overrideVfptrFun(window, &QXcbWindowEventListener::handleClientMessageEvent,
                                 this, &WindowEventHook::handleClientMessageEvent);
    VtableHook::overrideVfptrFun(window, &QXcbWindowEventListener::handleFocusInEvent,
                                 this, &WindowEventHook::handleFocusInEvent);
    VtableHook::overrideVfptrFun(window, &QXcbWindowEventListener::handleFocusOutEvent,
                                 this, &WindowEventHook::handleFocusOutEvent);
#ifdef XCB_USE_XINPUT22
    VtableHook::overrideVfptrFun(window, &QXcbWindowEventListener::handleXIEnterLeave,
                                 this, &WindowEventHook::handleXIEnterLeave);
#endif

    QObject::connect(window->window(), &QWindow::destroyed, window->window(), [this, window] {
        delete this;
        VtableHook::clearGhostVtable(static_cast<QXcbWindowEventListener*>(window));
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

bool WindowEventHook::relayFocusToModalWindow(QWindow *w, QXcbConnection *connection)
{
    QWindow *modal_window = 0;
    if (QGuiApplicationPrivate::instance()->isWindowBlocked(w,&modal_window) && modal_window != w) {
        if (!modal_window->isExposed())
            return false;

        modal_window->requestActivate();
        connection->flush();
        return true;
    }

    return false;
}

void WindowEventHook::handleFocusInEvent(const xcb_focus_in_event_t *event)
{
    Q_UNUSED(event)

    QXcbWindow *xcbWindow = window();
    QWindow *w = static_cast<QWindowPrivate *>(QObjectPrivate::get(xcbWindow->window()))->eventReceiver();

    if (relayFocusToModalWindow(w, xcbWindow->connection()))
        return;

    xcbWindow->connection()->setFocusWindow(static_cast<QXcbWindow *>(w->handle()));
    QWindowSystemInterface::handleWindowActivated(w, Qt::ActiveWindowFocusReason);
}

enum QX11EmbedMessageType {
    XEMBED_EMBEDDED_NOTIFY = 0,
    XEMBED_WINDOW_ACTIVATE = 1,
    XEMBED_WINDOW_DEACTIVATE = 2,
    XEMBED_REQUEST_FOCUS = 3,
    XEMBED_FOCUS_IN = 4,
    XEMBED_FOCUS_OUT = 5,
    XEMBED_FOCUS_NEXT = 6,
    XEMBED_FOCUS_PREV = 7,
    XEMBED_MODALITY_ON = 10,
    XEMBED_MODALITY_OFF = 11,
    XEMBED_REGISTER_ACCELERATOR = 12,
    XEMBED_UNREGISTER_ACCELERATOR = 13,
    XEMBED_ACTIVATE_ACCELERATOR = 14
};

static bool focusInPeeker(QXcbConnection *connection, xcb_generic_event_t *event)
{
    if (!event) {
        // FocusIn event is not in the queue, proceed with FocusOut normally.
        QWindowSystemInterface::handleWindowActivated(0, Qt::ActiveWindowFocusReason);
        return true;
    }
    uint response_type = event->response_type & ~0x80;
    if (response_type == XCB_FOCUS_IN) {
        // Ignore focus events that are being sent only because the pointer is over
        // our window, even if the input focus is in a different window.
        xcb_focus_in_event_t *e = (xcb_focus_in_event_t *) event;
        if (e->detail != XCB_NOTIFY_DETAIL_POINTER)
            return true;
    }

    /* We are also interested in XEMBED_FOCUS_IN events */
    if (response_type == XCB_CLIENT_MESSAGE) {
        xcb_client_message_event_t *cme = (xcb_client_message_event_t *)event;
        if (cme->type == connection->atom(QXcbAtom::_XEMBED)
            && cme->data.data32[1] == XEMBED_FOCUS_IN)
            return true;
    }

    return false;
}

void WindowEventHook::handleFocusOutEvent(const xcb_focus_out_event_t *event)
{
    Q_UNUSED(event)

    QXcbWindow *xcbWindow = window();
    QWindow *w = static_cast<QWindowPrivate *>(QObjectPrivate::get(xcbWindow->window()))->eventReceiver();

    if (relayFocusToModalWindow(w, xcbWindow->connection()))
        return;

    xcbWindow->connection()->setFocusWindow(0);
    // Do not set the active window to 0 if there is a FocusIn coming.
    // There is however no equivalent for XPutBackEvent so register a
    // callback for QXcbConnection instead.
    xcbWindow->connection()->addPeekFunc(focusInPeeker);
}

#ifdef XCB_USE_XINPUT22
static Qt::KeyboardModifiers translateModifiers(const QXcbKeyboard::_mod_masks &rmod_masks, int s)
{
    Qt::KeyboardModifiers ret = 0;
    if (s & XCB_MOD_MASK_SHIFT)
        ret |= Qt::ShiftModifier;
    if (s & XCB_MOD_MASK_CONTROL)
        ret |= Qt::ControlModifier;
    if (s & rmod_masks.alt)
        ret |= Qt::AltModifier;
    if (s & rmod_masks.meta)
        ret |= Qt::MetaModifier;
    if (s & rmod_masks.altgr)
        ret |= Qt::GroupSwitchModifier;
    return ret;
}

static inline int fixed1616ToInt(FP1616 val)
{
    return int((qreal(val >> 16)) + (val & 0xFFFF) / (qreal)0xFFFF);
}

void WindowEventHook::handleXIEnterLeave(xcb_ge_event_t *event)
{
    DQXcbWindow *me = reinterpret_cast<DQXcbWindow*>(window());

    xXIEnterEvent *ev = reinterpret_cast<xXIEnterEvent *>(event);

    // Compare the window with current mouse grabber to prevent deliver events to any other windows.
    // If leave event occurs and the window is under mouse - allow to deliver the leave event.
    QXcbWindow *mouseGrabber = me->connection()->mouseGrabber();
    if (mouseGrabber && mouseGrabber != me
            && (ev->evtype != XI_Leave || QGuiApplicationPrivate::currentMouseWindow != me->window())) {
        return;
    }

    if (ev->evtype == XI_Enter) {
        if (ev->buttons_len > 0) {
            Qt::MouseButtons buttons = me->connection()->buttons();
            const Qt::KeyboardModifiers modifiers = translateModifiers(me->connection()->keyboard()->rmod_masks, ev->mods.effective_mods);
            unsigned char *buttonMask = (unsigned char *) &ev[1];
            for (int i = 1; i <= 15; ++i) {
                Qt::MouseButton b = me->connection()->translateMouseButton(i);
                bool isSet = XIMaskIsSet(buttonMask, i);

                me->connection()->setButton(b, isSet);

                const int event_x = fixed1616ToInt(ev->event_x);
                const int event_y = fixed1616ToInt(ev->event_y);
                const int root_x = fixed1616ToInt(ev->root_x);
                const int root_y = fixed1616ToInt(ev->root_y);

                if (buttons.testFlag(b)) {
                    if (!isSet) {
                        me->handleButtonReleaseEvent(event_x, event_y, root_x, root_y,
                                                     ev->detail, modifiers, ev->time,
                                                     Qt::MouseEventSynthesizedBySystem);
                    }
                } else if (isSet) {
                    me->handleButtonPressEvent(event_x, event_y, root_x, root_y,
                                               ev->detail, modifiers, ev->time,
                                               Qt::MouseEventSynthesizedBySystem);
                }
            }
        }
    }

    me->QXcbWindow::handleXIEnterLeave(event);
}
#endif

DPP_END_NAMESPACE
