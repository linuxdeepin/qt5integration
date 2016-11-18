#include "xcbwindowhook.h"
#include "vtablehook.h"
#include "global.h"
#include "utility.h"

#include <QX11Info>

#include <private/qwindow_p.h>

#include <X11/Xlib.h>

#define HOOK_VFPTR(Fun) VtableHook::overrideVfptrFun(window, &QPlatformWindow::Fun, this, &XcbWindowHook::Fun)
#define CALL this->window()->QXcbWindow

PUBLIC_CLASS(QXcbWindow, XcbWindowHook);

QHash<const QPlatformWindow*, XcbWindowHook*> XcbWindowHook::mapped;

XcbWindowHook::XcbWindowHook(QXcbWindow *window)
    : xcbWindow(window)
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
    HOOK_VFPTR(setWindowState);
    HOOK_VFPTR(propagateSizeHints);

    QObject::connect(window->window(), &QWindow::destroyed, window->window(), [this] {
        if (mapped.contains(xcbWindow)) {
            delete this;
            VtableHook::clearGhostVtable(static_cast<QPlatformWindow*>(xcbWindow));
        }
    });
}

XcbWindowHook::~XcbWindowHook()
{
    mapped.remove(xcbWindow);
}

XcbWindowHook *XcbWindowHook::me() const
{
    return getHookByWindow(window());
}

void XcbWindowHook::setGeometry(const QRect &rect)
{
    const QMargins &margins = me()->windowMargins;

//    qDebug() << __FUNCTION__ << rect << rect + margins;

    CALL::setGeometry(rect + margins);
}

QRect XcbWindowHook::geometry() const
{
    const QMargins &margins = me()->windowMargins;

//    qDebug() << __FUNCTION__ << CALL::geometry() << CALL::window()->isVisible();

    return CALL::geometry() - margins;
}

QMargins XcbWindowHook::frameMargins() const
{
    QMargins margins = CALL::frameMargins();

    return margins/* + me()->windowMargins*/;
}

void XcbWindowHook::setParent(const QPlatformWindow *window)
{
    CALL::setParent(window);
}

void XcbWindowHook::setWindowTitle(const QString &title)
{
    return CALL::setWindowTitle(title);
}

void XcbWindowHook::setWindowIcon(const QIcon &icon)
{
    return CALL::setWindowIcon(icon);
}

QPoint XcbWindowHook::mapToGlobal(const QPoint &pos) const
{
    XcbWindowHook *me = XcbWindowHook::me();

    return CALL::mapToGlobal(pos + QPoint(me->windowMargins.left(), me->windowMargins.top()));
}

QPoint XcbWindowHook::mapFromGlobal(const QPoint &pos) const
{
    XcbWindowHook *me = XcbWindowHook::me();

    return CALL::mapFromGlobal(pos - QPoint(me->windowMargins.left(), me->windowMargins.top()));
}

void XcbWindowHook::setMask(const QRegion &region)
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

void XcbWindowHook::setWindowState(Qt::WindowState state)
{
    DQXcbWindow *window = static_cast<DQXcbWindow*>(this->window());

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

void XcbWindowHook::propagateSizeHints()
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

XcbWindowHook *XcbWindowHook::getHookByWindow(const QPlatformWindow *window)
{
    return mapped.value(window);
}

void XcbWindowHook::setWindowMargins(const QMargins &margins, bool propagateSizeHints)
{
    windowMargins = margins;

    if (!propagateSizeHints) {
        return;
    }

    QWindow *win = xcbWindow->window();
    QWindowPrivate *winp = qt_window_private(win);

    const QSize &user_max_size = win->property(userWindowMaximumSize).toSize();
    const QSize &user_min_size = win->property(userWindowMinimumSize).toSize();

    if (user_max_size.isValid()) {
        winp->maximumSize = user_max_size;
    }

    if (user_min_size.isValid()) {
        winp->minimumSize = user_min_size;
    }

    static_cast<QPlatformWindow*>(xcbWindow)->propagateSizeHints();
}
