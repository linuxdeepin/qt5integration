/**
 * Copyright (C) 2016 by Deepin Technology Co., Ltd.
 *
 * Use of this source code is governed by GPLv3
 * that can be found in the LICENSE file.
 **/

#ifndef WINDOWEVENTHOOK_H
#define WINDOWEVENTHOOK_H

#include "qxcbwindow.h"

#include "global.h"

DPP_BEGIN_NAMESPACE

class WindowEventHook
{
public:
    WindowEventHook(QXcbWindow *window);

    QXcbWindow *window() const
    { return static_cast<QXcbWindow*>(reinterpret_cast<QXcbWindowEventListener*>(const_cast<WindowEventHook*>(this)));}

    void handleClientMessageEvent(const xcb_client_message_event_t *event);
    void handleFocusInEvent(const xcb_focus_in_event_t *event);
    void handleFocusOutEvent(const xcb_focus_out_event_t *event);
#ifdef XCB_USE_XINPUT22
    void handleXIEnterLeave(xcb_ge_event_t *event);
#endif

private:
    static bool relayFocusToModalWindow(QWindow *w, QXcbConnection *connection);
};

DPP_END_NAMESPACE

#endif // WINDOWEVENTHOOK_H
