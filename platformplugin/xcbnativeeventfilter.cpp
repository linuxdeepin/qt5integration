#include "xcbnativeeventfilter.h"
#include "utility.h"

#define private public
#include "qxcbconnection.h"
#include "qxcbclipboard.h"
#undef private

#include "dplatformintegration.h"
#include "dxcbwmsupport.h"

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

    xcb_generic_event_t *event = reinterpret_cast<xcb_generic_event_t*>(message);
    uint response_type = event->response_type & ~0x80;

    if (response_type == m_connection->xfixes_first_event + XCB_XFIXES_SELECTION_NOTIFY) {
        xcb_xfixes_selection_notify_event_t *xsn = (xcb_xfixes_selection_notify_event_t *)event;

        if (xsn->selection == DPlatformIntegration::xcbConnection()->atom(QXcbAtom::_NET_WM_CM_S0)) {
            DXcbWMSupport::instance()->updateHasComposite();
        }

        QClipboard::Mode mode = clipboardModeForAtom(xsn->selection);
        if (mode > QClipboard::Selection)
            return false;

        // here we care only about the xfixes events that come from non Qt processes
        if (xsn->owner == XCB_NONE && xsn->subtype == XCB_XFIXES_SELECTION_EVENT_SET_SELECTION_OWNER) {
            QXcbClipboard *xcbClipboard = m_connection->m_clipboard;
            xcbClipboard->emitChanged(mode);
        }
    } else if (response_type == XCB_PROPERTY_NOTIFY) {
        xcb_property_notify_event_t *pn = (xcb_property_notify_event_t *)event;

        if (pn->window != DPlatformIntegration::instance()->defaultConnection()->rootWindow())
            return false;

        if (pn->atom == DPlatformIntegration::instance()->defaultConnection()->atom(QXcbAtom::_NET_SUPPORTED)) {
            DXcbWMSupport::instance()->updateNetWMAtoms();
        } else if (pn->atom == DPlatformIntegration::instance()->defaultConnection()->atom(QXcbAtom::_NET_SUPPORTING_WM_CHECK)) {
            DXcbWMSupport::instance()->updateWMName();
        } else if (pn->atom == DXcbWMSupport::instance()->_kde_net_wm_blur_rehind_region_atom) {
            DXcbWMSupport::instance()->updateRootWindowProperties();
        }
    }

    return false;
}

DPP_END_NAMESPACE
