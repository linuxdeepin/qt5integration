#include "dplatformwindowhook.h"
#include "vtablehook.h"
#include "global.h"
#include "utility.h"

#include <private/qwindow_p.h>

#ifdef Q_OS_LINUX
#include <QX11Info>
#include <X11/Xlib.h>
#endif

#define HOOK_VFPTR(Fun) VtableHook::overrideVfptrFun(window, &QPlatformWindow::Fun, this, &DPlatformWindowHook::Fun)
#define CALL this->window()->QNativeWindow

DPP_BEGIN_NAMESPACE

PUBLIC_CLASS(QNativeWindow, DPlatformWindowHook);

QHash<const QPlatformWindow*, DPlatformWindowHook*> DPlatformWindowHook::mapped;

DPlatformWindowHook::DPlatformWindowHook(QNativeWindow *window)
    : nativeWindow(window)
    , QObject(window->window())
{
    mapped[window] = this;

    HOOK_VFPTR(setGeometry);
    HOOK_VFPTR(geometry);
//    HOOK_VFPTR(frameMargins);
//    HOOK_VFPTR(setParent);
//    HOOK_VFPTR(setWindowTitle);
//    HOOK_VFPTR(setWindowIcon);
    HOOK_VFPTR(mapToGlobal);
    HOOK_VFPTR(mapFromGlobal);
    HOOK_VFPTR(setMask);
#ifdef Q_OS_LINUX
    HOOK_VFPTR(setWindowState);
#endif
    HOOK_VFPTR(propagateSizeHints);
}

DPlatformWindowHook::~DPlatformWindowHook()
{
    mapped.remove(nativeWindow);
    VtableHook::clearGhostVtable(static_cast<QPlatformWindow*>(nativeWindow));
}

DPlatformWindowHook *DPlatformWindowHook::me() const
{
    return getHookByWindow(window());
}

void DPlatformWindowHook::setGeometry(const QRect &rect)
{
    const QMargins &margins = me()->windowMargins;

//    qDebug() << __FUNCTION__ << rect << rect + margins;
    emit me()->windowGeometryAboutChanged(rect);
    CALL::setGeometry(rect + margins);
}

QRect DPlatformWindowHook::geometry() const
{
    const QMargins &margins = me()->windowMargins;

//    qDebug() << __FUNCTION__ << CALL::geometry() << CALL::window()->isVisible();

    return CALL::geometry() - margins;
}

QMargins DPlatformWindowHook::frameMargins() const
{
    QMargins margins = CALL::frameMargins();

    return margins/* + me()->windowMargins*/;
}

void DPlatformWindowHook::setParent(const QPlatformWindow *window)
{
    CALL::setParent(window);
}

void DPlatformWindowHook::setWindowTitle(const QString &title)
{
    return CALL::setWindowTitle(title);
}

void DPlatformWindowHook::setWindowIcon(const QIcon &icon)
{
    return CALL::setWindowIcon(icon);
}

QPoint DPlatformWindowHook::mapToGlobal(const QPoint &pos) const
{
    DPlatformWindowHook *me = DPlatformWindowHook::me();

    return CALL::mapToGlobal(pos + QPoint(me->windowMargins.left(), me->windowMargins.top()));
}

QPoint DPlatformWindowHook::mapFromGlobal(const QPoint &pos) const
{
    DPlatformWindowHook *me = DPlatformWindowHook::me();

    return CALL::mapFromGlobal(pos - QPoint(me->windowMargins.left(), me->windowMargins.top()));
}

void DPlatformWindowHook::setMask(const QRegion &region)
{
    QRegion tmp_region;

    const QMargins &margins = me()->windowMargins;

    QRect window_rect = CALL::geometry() - margins;

    window_rect.moveTopLeft(QPoint(margins.left(), margins.top()));

    for (const QRect &rect : region.rects()) {
        tmp_region += rect.translated(window_rect.topLeft()).intersected(window_rect) + margins;
    }

    QPainterPath path;

    path.addRegion(region);

    CALL::window()->setProperty(clipPath, QVariant::fromValue(path));
//    CALL::setMask(tmp_region);
    Utility::setInputShapeRectangles(CALL::winId(), tmp_region);
}

#ifdef Q_OS_LINUX
void DPlatformWindowHook::setWindowState(Qt::WindowState state)
{
    DQNativeWindow *window = static_cast<DQNativeWindow*>(this->window());

    if (window->m_windowState == state)
        return;

    if (state == Qt::WindowMinimized
            && (window->m_windowState == Qt::WindowMaximized
                || window->m_windowState == Qt::WindowFullScreen)) {
        window->changeNetWmState(true, Utility::internAtom("_NET_WM_STATE_HIDDEN"));
        XIconifyWindow(QX11Info::display(), window->m_window, QX11Info::appScreen());
        window->connection()->sync();

        window->m_windowState = state;
    } else {
        CALL::setWindowState(state);
    }
}
#endif

void DPlatformWindowHook::propagateSizeHints()
{
    QWindow *win = window()->window();
    QWindowPrivate *winp = qt_window_private(win);

    win->setProperty(userWindowMinimumSize, winp->minimumSize);
    win->setProperty(userWindowMaximumSize, winp->maximumSize);

    const QMargins &windowMargins = me()->windowMargins;
    const QSize &marginSize = QSize(windowMargins.left() + windowMargins.right(),
                                    windowMargins.top() + windowMargins.bottom());

    winp->minimumSize += marginSize;
    winp->maximumSize += marginSize;
    winp->maximumSize.setWidth(qMin(QWINDOWSIZE_MAX, winp->maximumSize.width()));
    winp->maximumSize.setHeight(qMin(QWINDOWSIZE_MAX, winp->maximumSize.height()));

    CALL::propagateSizeHints();

//    qDebug() << winp->minimumSize << winp->maximumSize << marginSize;
}

DPlatformWindowHook *DPlatformWindowHook::getHookByWindow(const QPlatformWindow *window)
{
    return mapped.value(window);
}

void DPlatformWindowHook::setWindowMargins(const QMargins &margins, bool propagateSizeHints)
{
    windowMargins = margins;

    if (!propagateSizeHints) {
        return;
    }

    QWindow *win = nativeWindow->window();
    QWindowPrivate *winp = qt_window_private(win);

    const QSize &user_max_size = win->property(userWindowMaximumSize).toSize();
    const QSize &user_min_size = win->property(userWindowMinimumSize).toSize();

    if (user_max_size.isValid()) {
        winp->maximumSize = user_max_size;
    }

    if (user_min_size.isValid()) {
        winp->minimumSize = user_min_size;
    }

    static_cast<QPlatformWindow*>(nativeWindow)->propagateSizeHints();
}

DPP_END_NAMESPACE
