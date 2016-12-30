#include "xcbnativeeventfilter.h"

#define private public
#include "qxcbconnection.h"
#include "qxcbclipboard.h"
#undef private

#include <xcb/xfixes.h>

DPP_BEGIN_NAMESPACE

XcbNativeEventFilter::XcbNativeEventFilter(QXcbConnection *connection)
    : m_connection(connection)
{

}

QClipboard::Mode XcbNativeEventFilter::clipboardModeForAtom(xcb_atom_t a) const
{
    if (a == XCB_ATOM_PRIMARY)
        return QClipboard::Selection;
    if (a == m_connection->atom(QXcbAtom::CLIPBOARD))
        return QClipboard::Clipboard;
    // not supported enum value, used to detect errors
    return QClipboard::FindBuffer;
}

bool XcbNativeEventFilter::nativeEventFilter(const QByteArray &eventType, void *message, long *result)
{
    Q_UNUSED(eventType)
    Q_UNUSED(result)

    xcb_xfixes_selection_notify_event_t *event = (xcb_xfixes_selection_notify_event_t *)reinterpret_cast<xcb_generic_event_t*>(message);
    uint response_type = event->response_type & ~0x80;

    if (response_type != m_connection->xfixes_first_event + XCB_XFIXES_SELECTION_NOTIFY)
        return false;

    QXcbClipboard *xcbClipboard = m_connection->m_clipboard;

    QClipboard::Mode mode = clipboardModeForAtom(event->selection);
    if (mode > QClipboard::Selection)
        return false;

    // here we care only about the xfixes events that come from non Qt processes
    if (event->owner == XCB_NONE && event->subtype == XCB_XFIXES_SELECTION_EVENT_SET_SELECTION_OWNER) {
        xcbClipboard->emitChanged(mode);
    }

    return false;
}

DPP_END_NAMESPACE
