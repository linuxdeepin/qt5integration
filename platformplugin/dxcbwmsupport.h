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

public:
    static DXcbWMSupport *instance();
    static bool connectWindowManagerChangedSignal(QObject *object, std::function<void()> slot);

    bool isSupportedByWM(xcb_atom_t atom) const;

signals:
    void windowManagerChanged();

protected:
    explicit DXcbWMSupport();

private:
    void emitWMChanged();
    void updateNetWMAtoms();

    QVector<xcb_atom_t> net_wm_atoms;

    friend class XcbNativeEventFilter;
};

DPP_END_NAMESPACE

#endif // DXCBWMSUPPORT_H
