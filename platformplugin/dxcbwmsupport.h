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
    Q_PROPERTY(bool hasComposite READ hasComposite NOTIFY hasCompositeChanged)

public:
    enum {
        MWM_HINTS_FUNCTIONS   = (1L << 0),

        MWM_FUNC_ALL      = (1L << 0),
        MWM_FUNC_RESIZE   = (1L << 1),
        MWM_FUNC_MOVE     = (1L << 2),
        MWM_FUNC_MINIMIZE = (1L << 3),
        MWM_FUNC_MAXIMIZE = (1L << 4),
        MWM_FUNC_CLOSE    = (1L << 5),

        MWM_HINTS_DECORATIONS = (1L << 1),

        MWM_DECOR_ALL      = (1L << 0),
        MWM_DECOR_BORDER   = (1L << 1),
        MWM_DECOR_RESIZEH  = (1L << 2),
        MWM_DECOR_TITLE    = (1L << 3),
        MWM_DECOR_MENU     = (1L << 4),
        MWM_DECOR_MINIMIZE = (1L << 5),
        MWM_DECOR_MAXIMIZE = (1L << 6),

        MWM_HINTS_INPUT_MODE = (1L << 2),

        MWM_INPUT_MODELESS                  = 0L,
        MWM_INPUT_PRIMARY_APPLICATION_MODAL = 1L,
        MWM_INPUT_FULL_APPLICATION_MODAL    = 3L
    };

    static DXcbWMSupport *instance();
    static bool connectWindowManagerChangedSignal(QObject *object, std::function<void()> slot);
    static bool connectHasBlurWindowChanged(QObject *object, std::function<void()> slot);
    static bool connectHasCompositeChanged(QObject *object, std::function<void()> slot);
    static bool connectWindowListChanged(QObject *object, std::function<void()> slot);

    bool isDeepinWM() const;
    bool isKwin() const;
    bool isSupportedByWM(xcb_atom_t atom) const;
    bool isContainsForRootWindow(xcb_atom_t atom) const;
    bool hasBlurWindow() const;
    bool hasComposite() const;

    QString windowManagerName() const;

    QVector<xcb_window_t> allWindow() const;

signals:
    void windowManagerChanged();
    void hasBlurWindowChanged(bool hasBlurWindow);
    void hasCompositeChanged(bool hasComposite);
    void windowListChanged();

protected:
    explicit DXcbWMSupport();

private:
    void updateWMName(bool emitSignal = true);
    void updateNetWMAtoms();
    void updateRootWindowProperties();
    void updateHasBlurWindow();
    void updateHasComposite();

    bool m_isDeepinWM = false;
    bool m_isKwin = false;
    bool m_hasBlurWindow = false;
    bool m_hasComposite = false;

    QString m_wmName;

    xcb_atom_t _net_wm_deepin_blur_region_rounded_atom = 0;
    xcb_atom_t _kde_net_wm_blur_rehind_region_atom = 0;
    xcb_atom_t _net_wm_deepin_blur_region_mask = 0;

    QVector<xcb_atom_t> net_wm_atoms;
    QVector<xcb_atom_t> root_window_properties;

    friend class XcbNativeEventFilter;
    friend class Utility;
};

DPP_END_NAMESPACE

#endif // DXCBWMSUPPORT_H
