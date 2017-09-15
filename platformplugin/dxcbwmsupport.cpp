/*
 * Copyright (C) 2017 ~ 2017 Deepin Technology Co., Ltd.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "dxcbwmsupport.h"
#include "dplatformintegration.h"
#include "utility.h"

#include "qxcbconnection.h"
#include "qxcbscreen.h"

DPP_BEGIN_NAMESPACE

class _DXcbWMSupport : public DXcbWMSupport {};

Q_GLOBAL_STATIC(_DXcbWMSupport, globalXWMS)

DXcbWMSupport::DXcbWMSupport()
{
    updateWMName(false);
}

void DXcbWMSupport::updateWMName(bool emitSignal)
{
    _net_wm_deepin_blur_region_rounded_atom = Utility::internAtom(QT_STRINGIFY(_NET_WM_DEEPIN_BLUR_REGION_ROUNDED));
    _net_wm_deepin_blur_region_mask = Utility::internAtom(QT_STRINGIFY(_NET_WM_DEEPIN_BLUR_REGION_MASK));
    _kde_net_wm_blur_rehind_region_atom = Utility::internAtom(QT_STRINGIFY(_KDE_NET_WM_BLUR_BEHIND_REGION));

    m_wmName.clear();

    xcb_connection_t *xcb_connection = DPlatformIntegration::xcbConnection()->xcb_connection();
    xcb_window_t root = DPlatformIntegration::xcbConnection()->primaryScreen()->root();

    xcb_get_property_reply_t *reply =
        xcb_get_property_reply(xcb_connection,
            xcb_get_property_unchecked(xcb_connection, false, root,
                             DPlatformIntegration::xcbConnection()->atom(QXcbAtom::_NET_SUPPORTING_WM_CHECK),
                             XCB_ATOM_WINDOW, 0, 1024), NULL);

    if (reply && reply->format == 32 && reply->type == XCB_ATOM_WINDOW) {
        xcb_window_t windowManager = *((xcb_window_t *)xcb_get_property_value(reply));

        if (windowManager != XCB_WINDOW_NONE) {
            xcb_get_property_reply_t *windowManagerReply =
                xcb_get_property_reply(xcb_connection,
                    xcb_get_property_unchecked(xcb_connection, false, windowManager,
                                     DPlatformIntegration::xcbConnection()->atom(QXcbAtom::_NET_WM_NAME),
                                     DPlatformIntegration::xcbConnection()->atom(QXcbAtom::UTF8_STRING), 0, 1024), NULL);
            if (windowManagerReply && windowManagerReply->format == 8
                    && windowManagerReply->type == DPlatformIntegration::xcbConnection()->atom(QXcbAtom::UTF8_STRING)) {
                m_wmName = QString::fromUtf8((const char *)xcb_get_property_value(windowManagerReply), xcb_get_property_value_length(windowManagerReply));
            }

            free(windowManagerReply);
        }
    }
    free(reply);

    m_isDeepinWM = (m_wmName == QStringLiteral("Mutter(DeepinGala)"));
    m_isKwin = !m_isDeepinWM && (m_wmName == QStringLiteral("KWin"));

    updateNetWMAtoms();
    updateRootWindowProperties();
    updateHasComposite();

    if (emitSignal)
        emit windowManagerChanged();
}

void DXcbWMSupport::updateNetWMAtoms()
{
    net_wm_atoms.clear();

    xcb_window_t root = DPlatformIntegration::xcbConnection()->primaryScreen()->root();
    int offset = 0;
    int remaining = 0;
    xcb_connection_t *xcb_connection = DPlatformIntegration::xcbConnection()->xcb_connection();

    do {
        xcb_get_property_cookie_t cookie = xcb_get_property(xcb_connection, false, root,
                                                            DPlatformIntegration::xcbConnection()->atom(QXcbAtom::_NET_SUPPORTED),
                                                            XCB_ATOM_ATOM, offset, 1024);
        xcb_get_property_reply_t *reply = xcb_get_property_reply(xcb_connection, cookie, NULL);
        if (!reply)
            break;

        remaining = 0;

        if (reply->type == XCB_ATOM_ATOM && reply->format == 32) {
            int len = xcb_get_property_value_length(reply)/sizeof(xcb_atom_t);
            xcb_atom_t *atoms = (xcb_atom_t *)xcb_get_property_value(reply);
            int s = net_wm_atoms.size();
            net_wm_atoms.resize(s + len);
            memcpy(net_wm_atoms.data() + s, atoms, len*sizeof(xcb_atom_t));

            remaining = reply->bytes_after;
            offset += len;
        }

        free(reply);
    } while (remaining > 0);

    updateHasBlurWindow();
}

void DXcbWMSupport::updateRootWindowProperties()
{
    root_window_properties.clear();

    xcb_window_t root = DPlatformIntegration::xcbConnection()->primaryScreen()->root();
    xcb_connection_t *xcb_connection = DPlatformIntegration::xcbConnection()->xcb_connection();

    xcb_list_properties_cookie_t cookie = xcb_list_properties(xcb_connection, root);
    xcb_list_properties_reply_t *reply = xcb_list_properties_reply(xcb_connection, cookie, NULL);

    if (!reply)
        return;

    int len = xcb_list_properties_atoms_length(reply);
    xcb_atom_t *atoms = (xcb_atom_t *)xcb_list_properties_atoms(reply);
    root_window_properties.resize(len);
    memcpy(root_window_properties.data(), atoms, len * sizeof(xcb_atom_t));

    free(reply);

    updateHasBlurWindow();
}

void DXcbWMSupport::updateHasBlurWindow()
{
    bool hasBlurWindow((m_isDeepinWM && isSupportedByWM(_net_wm_deepin_blur_region_rounded_atom))
                       || (m_isKwin && isContainsForRootWindow(_kde_net_wm_blur_rehind_region_atom)));

    if (m_hasBlurWindow == hasBlurWindow)
        return;

    m_hasBlurWindow = hasBlurWindow;
    emit hasBlurWindowChanged(hasBlurWindow);
}

void DXcbWMSupport::updateHasComposite()
{
    xcb_connection_t *xcb_connection = DPlatformIntegration::xcbConnection()->xcb_connection();
    xcb_get_selection_owner_cookie_t cookit = xcb_get_selection_owner(xcb_connection, DPlatformIntegration::xcbConnection()->atom(QXcbAtom::_NET_WM_CM_S0));
    xcb_get_selection_owner_reply_t *reply = xcb_get_selection_owner_reply(xcb_connection, cookit, NULL);

    if (!reply)
        return;

    bool hasComposite(reply->owner != XCB_NONE);

    free(reply);

    if (m_hasComposite == hasComposite)
        return;

    m_hasComposite = hasComposite;
    emit hasCompositeChanged(hasComposite);
}

DXcbWMSupport *DXcbWMSupport::instance()
{
    return globalXWMS;
}

bool DXcbWMSupport::connectWindowManagerChangedSignal(QObject *object, std::function<void ()> slot)
{
    if (!object)
        return QObject::connect(globalXWMS, &DXcbWMSupport::windowManagerChanged, slot);

    return QObject::connect(globalXWMS, &DXcbWMSupport::windowManagerChanged, object, slot);
}

bool DXcbWMSupport::connectHasBlurWindowChanged(QObject *object, std::function<void ()> slot)
{
    if (!object)
        return QObject::connect(globalXWMS, &DXcbWMSupport::hasBlurWindowChanged, slot);

    return QObject::connect(globalXWMS, &DXcbWMSupport::hasBlurWindowChanged, object, slot);
}

bool DXcbWMSupport::connectHasCompositeChanged(QObject *object, std::function<void ()> slot)
{
    if (!object)
        return QObject::connect(globalXWMS, &DXcbWMSupport::hasCompositeChanged, slot);

    return QObject::connect(globalXWMS, &DXcbWMSupport::hasCompositeChanged, object, slot);
}

bool DXcbWMSupport::connectWindowListChanged(QObject *object, std::function<void ()> slot)
{
    if (!object)
        return QObject::connect(globalXWMS, &DXcbWMSupport::windowListChanged, slot);

    return QObject::connect(globalXWMS, &DXcbWMSupport::windowListChanged, object, slot);
}

QString DXcbWMSupport::windowManagerName() const
{
    return m_wmName;
}

QVector<xcb_window_t> DXcbWMSupport::allWindow() const
{
    QVector<xcb_window_t> window_list_stacking;

    xcb_window_t root = DPlatformIntegration::xcbConnection()->primaryScreen()->root();
    int offset = 0;
    int remaining = 0;
    xcb_connection_t *xcb_connection = DPlatformIntegration::xcbConnection()->xcb_connection();

    do {
        xcb_get_property_cookie_t cookie = xcb_get_property(xcb_connection, false, root,
                                                            Utility::internAtom("_NET_CLIENT_LIST_STACKING"),
                                                            XCB_ATOM_WINDOW, offset, 1024);
        xcb_get_property_reply_t *reply = xcb_get_property_reply(xcb_connection, cookie, NULL);
        if (!reply)
            break;

        remaining = 0;

        if (reply->type == XCB_ATOM_WINDOW && reply->format == 32) {
            int len = xcb_get_property_value_length(reply)/sizeof(xcb_window_t);
            xcb_window_t *windows = (xcb_window_t *)xcb_get_property_value(reply);
            int s = window_list_stacking.size();
            window_list_stacking.resize(s + len);
            memcpy(window_list_stacking.data() + s, windows, len*sizeof(xcb_window_t));

            remaining = reply->bytes_after;
            offset += len;
        }

        free(reply);
    } while (remaining > 0);

    return window_list_stacking;
}

bool DXcbWMSupport::isDeepinWM() const
{
    return m_isDeepinWM;
}

bool DXcbWMSupport::isKwin() const
{
    return m_isKwin;
}

bool DXcbWMSupport::isSupportedByWM(xcb_atom_t atom) const
{
    return net_wm_atoms.contains(atom);
}

bool DXcbWMSupport::isContainsForRootWindow(xcb_atom_t atom) const
{
    return root_window_properties.contains(atom);
}

bool DXcbWMSupport::hasBlurWindow() const
{
    return m_hasBlurWindow;
}

bool DXcbWMSupport::hasComposite() const
{
    return m_hasComposite;
}

DPP_END_NAMESPACE
