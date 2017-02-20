#ifndef DXCBWMSUPPORT_H
#define DXCBWMSUPPORT_H

#include "global.h"

#include <QObject>
#include <QVector>

#include <xcb/xcb.h>

#include <functional>

DPP_BEGIN_NAMESPACE

class DXcbWMSupport : public QObject
{
    Q_OBJECT

    Q_PROPERTY(bool hasBlurWindow READ hasBlurWindow NOTIFY hasBlurWindowChanged)

public:
    static DXcbWMSupport *instance();
    static bool connectWindowManagerChangedSignal(QObject *object, std::function<void()> slot);
    static bool connectHasBlurWindowChanged(QObject *object, std::function<void()> slot);

    bool isDeepinWM() const;
    bool isSupportedByWM(xcb_atom_t atom) const;
    bool isContainsForRootWindow(xcb_atom_t atom) const;
    bool hasBlurWindow() const;

signals:
    void windowManagerChanged();
    void hasBlurWindowChanged(bool hasBlurWindow);

protected:
    explicit DXcbWMSupport();

private:
    void emitWMChanged();
    void updateNetWMAtoms();
    void updateRootWindowProperties();
    void updateHasBlurWindow();

    bool m_isDeepinWM = false;
    bool m_hasBlurWindow = false;
    xcb_atom_t _net_wm_deepin_blur_region_rounded_atom = 0;
    xcb_atom_t _kde_net_wm_blur_rehind_region_atom = 0;

    QVector<xcb_atom_t> net_wm_atoms;
    QVector<xcb_atom_t> root_window_properties;

    friend class XcbNativeEventFilter;
    friend class Utility;
};

DPP_END_NAMESPACE

#endif // DXCBWMSUPPORT_H
