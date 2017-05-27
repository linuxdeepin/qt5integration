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
    : QObject(window->window())
    , nativeWindow(window)
{
    mapped[window] = this;

    HOOK_VFPTR(setGeometry);
    HOOK_VFPTR(geometry);
    HOOK_VFPTR(frameMargins);
//    HOOK_VFPTR(setParent);
//    HOOK_VFPTR(setWindowTitle);
//    HOOK_VFPTR(setWindowIcon);
    HOOK_VFPTR(mapToGlobal);
    HOOK_VFPTR(mapFromGlobal);
    HOOK_VFPTR(setMask);
#ifdef Q_OS_LINUX
    HOOK_VFPTR(setWindowState);
    HOOK_VFPTR(setVisible);
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
    return QMargins();
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
    Utility::setRectangles(CALL::winId(), tmp_region);
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

struct QtMotifWmHints {
    quint32 flags, functions, decorations;
    qint32 input_mode;
    quint32 status;
};

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

static QtMotifWmHints getMotifWmHints(QXcbConnection *c, xcb_window_t window)
{
    QtMotifWmHints hints;

    xcb_get_property_cookie_t get_cookie =
        xcb_get_property_unchecked(c->xcb_connection(), 0, window, c->atom(QXcbAtom::_MOTIF_WM_HINTS),
                         c->atom(QXcbAtom::_MOTIF_WM_HINTS), 0, 20);

    xcb_get_property_reply_t *reply =
        xcb_get_property_reply(c->xcb_connection(), get_cookie, NULL);

    if (reply && reply->format == 32 && reply->type == c->atom(QXcbAtom::_MOTIF_WM_HINTS)) {
        hints = *((QtMotifWmHints *)xcb_get_property_value(reply));
    } else {
        hints.flags = 0L;
        hints.functions = MWM_FUNC_ALL;
        hints.decorations = MWM_DECOR_ALL;
        hints.input_mode = 0L;
        hints.status = 0L;
    }

    free(reply);

    return hints;
}

static void setMotifWmHints(QXcbConnection *c, xcb_window_t window, const QtMotifWmHints &hints)
{
    if (hints.flags != 0l) {
        Q_XCB_CALL2(xcb_change_property(c->xcb_connection(),
                                       XCB_PROP_MODE_REPLACE,
                                       window,
                                       c->atom(QXcbAtom::_MOTIF_WM_HINTS),
                                       c->atom(QXcbAtom::_MOTIF_WM_HINTS),
                                       32,
                                       5,
                                       &hints), c);
    } else {
        Q_XCB_CALL2(xcb_delete_property(c->xcb_connection(), window, c->atom(QXcbAtom::_MOTIF_WM_HINTS)), c);
    }
}

void DPlatformWindowHook::setVisible(bool visible)
{
    if (!visible) {
        return CALL::setVisible(visible);
    }

    // reupdate _MOTIF_WM_HINTS
    DQNativeWindow *window = static_cast<DQNativeWindow*>(this->window());

    QtMotifWmHints mwmhints = getMotifWmHints(window->connection(), window->m_window);

    if (window->window()->modality() != Qt::NonModal) {
        switch (window->window()->modality()) {
        case Qt::WindowModal:
            mwmhints.input_mode = MWM_INPUT_PRIMARY_APPLICATION_MODAL;
            break;
        case Qt::ApplicationModal:
        default:
            mwmhints.input_mode = MWM_INPUT_FULL_APPLICATION_MODAL;
            break;
        }
        mwmhints.flags |= MWM_HINTS_INPUT_MODE;
    } else {
        mwmhints.input_mode = MWM_INPUT_MODELESS;
        mwmhints.flags &= ~MWM_HINTS_INPUT_MODE;
    }

    if (window->windowMinimumSize() == window->windowMaximumSize()) {
        // fixed size, remove the resize handle (since mwm/dtwm
        // isn't smart enough to do it itself)
        mwmhints.flags |= MWM_HINTS_FUNCTIONS;
        mwmhints.functions &= ~MWM_FUNC_RESIZE;

        if (mwmhints.decorations == MWM_DECOR_ALL) {
            mwmhints.flags |= MWM_HINTS_DECORATIONS;
            mwmhints.decorations = (MWM_DECOR_BORDER
                                    | MWM_DECOR_TITLE
                                    | MWM_DECOR_MENU);
        } else {
            mwmhints.decorations &= ~MWM_DECOR_RESIZEH;
        }
    }

    if (window->window()->flags() & Qt::WindowMinimizeButtonHint) {
        mwmhints.functions |= MWM_FUNC_MINIMIZE;
    }
    if (window->window()->flags() & Qt::WindowMaximizeButtonHint) {
        mwmhints.functions |= MWM_FUNC_MAXIMIZE;
    }
    if (window->window()->flags() & Qt::WindowCloseButtonHint)
        mwmhints.functions |= MWM_FUNC_CLOSE;

    CALL::setVisible(visible);
    // Fix the window can't show minimized if window is fixed size
    setMotifWmHints(window->connection(), window->m_window, mwmhints);
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
