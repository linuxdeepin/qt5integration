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

#include "dplatformintegration.h"
#include "dplatformbackingstore.h"
#include "global.h"
#include "dplatformwindowhelper.h"
#include "dforeignplatformwindow.h"
#include "dplatformbackingstorehelper.h"
#include "dplatformopenglcontexthelper.h"
#include "dframewindow.h"
#include "vtablehook.h"
#include "dplatformwindowhook.h"

#ifdef Q_OS_LINUX
#define private public
#include "qxcbcursor.h"
#undef private

#include "windoweventhook.h"
#include "xcbnativeeventfilter.h"
#include "dplatformnativeinterface.h"

#include "qxcbscreen.h"
#include "qxcbbackingstore.h"
#include "qxcbcursor.h"
#include "qxcbxsettings.h"

#include <X11/cursorfont.h>
#include <xcb/xcb_image.h>
#include <xcb/xcb_renderutil.h>
#endif

#if QT_VERSION < QT_VERSION_CHECK(5, 4, 0)
#include <private/qwidgetwindow_qpa_p.h>
#else
#include <private/qwidgetwindow_p.h>
#endif

#include <QWidget>
#include <QGuiApplication>
#include <QLibrary>

#include <private/qguiapplication_p.h>

DPP_BEGIN_NAMESPACE

#ifdef Q_OS_LINUX
#if QT_VERSION < QT_VERSION_CHECK(5, 5, 0)
DPlatformIntegration *DPlatformIntegration::m_instance = Q_NULLPTR;
#endif
#endif

DPlatformIntegration::DPlatformIntegration(const QStringList &parameters, int &argc, char **argv)
    : DPlatformIntegrationParent(parameters, argc, argv)
    , m_storeHelper(new DPlatformBackingStoreHelper)
    , m_contextHelper(new DPlatformOpenGLContextHelper)
{
#ifdef Q_OS_LINUX
#if QT_VERSION < QT_VERSION_CHECK(5, 5, 0)
    m_instance = this;
#endif
#endif

    m_nativeInterface.reset(new DPlatformNativeInterface());
}

DPlatformIntegration::~DPlatformIntegration()
{
#ifdef Q_OS_LINUX
    if (!m_eventFilter)
        return;

    qApp->removeNativeEventFilter(m_eventFilter);
    delete m_eventFilter;
#endif

    delete m_storeHelper;
    delete m_contextHelper;
}

QPlatformWindow *DPlatformIntegration::createPlatformWindow(QWindow *window) const
{
    qDebug() << __FUNCTION__ << window << window->type() << window->parent();

    // handle foreign native window
    if (window->type() == Qt::ForeignWindow && window->property("_q_foreignWinId").isValid()) {
        return new DForeignPlatformWindow(window);
    }

    bool isUseDxcb = window->type() != Qt::Desktop && window->property(useDxcb).toBool();

    if (isUseDxcb) {
        QSurfaceFormat format = window->format();

        const int oldAlpha = format.alphaBufferSize();
        const int newAlpha = 8;

        if (oldAlpha != newAlpha) {
            format.setAlphaBufferSize(newAlpha);
            window->setFormat(format);
        }
    }

    QPlatformWindow *w = DPlatformIntegrationParent::createPlatformWindow(window);
    QNativeWindow *xw = static_cast<QNativeWindow*>(w);

    if (isUseDxcb) {
#ifdef USE_NEW_IMPLEMENTING
        Q_UNUSED(new DPlatformWindowHelper(xw))
#else
        Q_UNUSED(new DPlatformWindowHook(xw))
#endif
    }

#ifdef Q_OS_LINUX
    qDebug() << window << window->type();

    if (window->type() == Qt::Widget
            || window->type() == Qt::Window
            || window->type() == Qt::Dialog
            || window->type() == Qt::Popup) {
        Q_UNUSED(new WindowEventHook(xw))
    }
#endif

    QWindow *tp_for_window = window->transientParent();

    if (tp_for_window) {
        // reset transient parent
        if (DPlatformWindowHelper *helper = DPlatformWindowHelper::mapped.value(tp_for_window->handle())) {
            window->setTransientParent(helper->m_frameWindow);
        }
    }

    return xw;
}

QPlatformBackingStore *DPlatformIntegration::createPlatformBackingStore(QWindow *window) const
{
    qDebug() << __FUNCTION__ << window << window->type() << window->parent();

    QPlatformBackingStore *store = DPlatformIntegrationParent::createPlatformBackingStore(window);

    if (window->type() != Qt::Desktop && window->property(useDxcb).toBool())
#ifdef USE_NEW_IMPLEMENTING
        m_storeHelper->addBackingStore(store);
#else
        return new DPlatformBackingStore(window, static_cast<QXcbBackingStore*>(store));
#endif

    return store;
}

QPlatformOpenGLContext *DPlatformIntegration::createPlatformOpenGLContext(QOpenGLContext *context) const
{
    QPlatformOpenGLContext *p_context = DPlatformIntegrationParent::createPlatformOpenGLContext(context);

//    m_contextHelper->addOpenGLContext(context, p_context);

    return p_context;
}

QPlatformNativeInterface *DPlatformIntegration::nativeInterface() const
{
    return m_nativeInterface.data();
}

QStringList DPlatformIntegration::themeNames() const
{
    QStringList list = DPlatformIntegrationParent::themeNames();

    list.prepend("deepin");

    return list;
}

#ifdef Q_OS_LINUX
typedef int (*PtrXcursorLibraryLoadCursor)(void *, const char *);
typedef char *(*PtrXcursorLibraryGetTheme)(void *);
typedef int (*PtrXcursorLibrarySetTheme)(void *, const char *);
typedef int (*PtrXcursorLibraryGetDefaultSize)(void *);

#if defined(XCB_USE_XLIB) && !defined(QT_NO_LIBRARY)
#include <X11/Xlib.h>
enum {
    XCursorShape = CursorShape
};
#undef CursorShape

static PtrXcursorLibraryLoadCursor ptrXcursorLibraryLoadCursor = 0;
static PtrXcursorLibraryGetTheme ptrXcursorLibraryGetTheme = 0;
static PtrXcursorLibrarySetTheme ptrXcursorLibrarySetTheme = 0;
static PtrXcursorLibraryGetDefaultSize ptrXcursorLibraryGetDefaultSize = 0;

static xcb_font_t cursorFont = 0;
#endif

static int cursorIdForShape(int cshape)
{
    int cursorId = 0;
    switch (cshape) {
    case Qt::ArrowCursor:
        cursorId = XC_left_ptr;
        break;
    case Qt::UpArrowCursor:
        cursorId = XC_center_ptr;
        break;
    case Qt::CrossCursor:
        cursorId = XC_crosshair;
        break;
    case Qt::WaitCursor:
        cursorId = XC_watch;
        break;
    case Qt::IBeamCursor:
        cursorId = XC_xterm;
        break;
    case Qt::SizeAllCursor:
        cursorId = XC_fleur;
        break;
    case Qt::PointingHandCursor:
        cursorId = XC_hand2;
        break;
    case Qt::SizeBDiagCursor:
        cursorId = XC_top_right_corner;
        break;
    case Qt::SizeFDiagCursor:
        cursorId = XC_bottom_right_corner;
        break;
    case Qt::SizeVerCursor:
    case Qt::SplitVCursor:
        cursorId = XC_sb_v_double_arrow;
        break;
    case Qt::SizeHorCursor:
    case Qt::SplitHCursor:
        cursorId = XC_sb_h_double_arrow;
        break;
    case Qt::WhatsThisCursor:
        cursorId = XC_question_arrow;
        break;
    case Qt::ForbiddenCursor:
        cursorId = XC_circle;
        break;
    case Qt::BusyCursor:
        cursorId = XC_watch;
        break;
    default:
        break;
    }
    return cursorId;
}

static const char * const cursorNames[] = {
    "left_ptr",
    "up_arrow",
    "cross",
    "wait",
    "ibeam",
    "size_ver",
    "size_hor",
    "size_bdiag",
    "size_fdiag",
    "size_all",
    "blank",
    "split_v",
    "split_h",
    "pointing_hand",
    "forbidden",
    "whats_this",
    "left_ptr_watch",
    "openhand",
    "closedhand",
    "copy",
    "move",
    "link"
};

static xcb_cursor_t loadCursor(void *dpy, int cshape)
{
    xcb_cursor_t cursor = XCB_NONE;
    if (!ptrXcursorLibraryLoadCursor || !dpy)
        return cursor;
    switch (cshape) {
    case Qt::DragCopyCursor:
        cursor = ptrXcursorLibraryLoadCursor(dpy, "dnd-copy");
        break;
    case Qt::DragMoveCursor:
        cursor = ptrXcursorLibraryLoadCursor(dpy, "dnd-move");
        break;
    case Qt::DragLinkCursor:
        cursor = ptrXcursorLibraryLoadCursor(dpy, "dnd-link");
        break;
    default:
        break;
    }
    if (!cursor) {
        cursor = ptrXcursorLibraryLoadCursor(dpy, cursorNames[cshape]);
    }
    return cursor;
}

static xcb_cursor_t qt_xcb_createCursorXRender(QXcbScreen *screen, const QImage &image,
                                        const QPoint &spot)
{
#ifdef XCB_USE_RENDER
    xcb_connection_t *conn = screen->xcb_connection();
    const int w = image.width();
    const int h = image.height();
    xcb_generic_error_t *error = 0;
    xcb_render_query_pict_formats_cookie_t formatsCookie = xcb_render_query_pict_formats(conn);
    xcb_render_query_pict_formats_reply_t *formatsReply = xcb_render_query_pict_formats_reply(conn,
                                                                                              formatsCookie,
                                                                                              &error);
    if (!formatsReply || error) {
        qWarning("qt_xcb_createCursorXRender: query_pict_formats failed");
        free(formatsReply);
        free(error);
        return XCB_NONE;
    }
    xcb_render_pictforminfo_t *fmt = xcb_render_util_find_standard_format(formatsReply,
                                                                          XCB_PICT_STANDARD_ARGB_32);
    if (!fmt) {
        qWarning("qt_xcb_createCursorXRender: Failed to find format PICT_STANDARD_ARGB_32");
        free(formatsReply);
        return XCB_NONE;
    }

    QImage img = image.convertToFormat(QImage::Format_ARGB32_Premultiplied);
    xcb_image_t *xi = xcb_image_create(w, h, XCB_IMAGE_FORMAT_Z_PIXMAP,
                                       32, 32, 32, 32,
                                       QSysInfo::ByteOrder == QSysInfo::BigEndian ? XCB_IMAGE_ORDER_MSB_FIRST : XCB_IMAGE_ORDER_LSB_FIRST,
                                       XCB_IMAGE_ORDER_MSB_FIRST,
                                       0, 0, 0);
    if (!xi) {
        qWarning("qt_xcb_createCursorXRender: xcb_image_create failed");
        free(formatsReply);
        return XCB_NONE;
    }
    xi->data = (uint8_t *) malloc(xi->stride * h);
    if (!xi->data) {
        qWarning("qt_xcb_createCursorXRender: Failed to malloc() image data");
        xcb_image_destroy(xi);
        free(formatsReply);
        return XCB_NONE;
    }
    memcpy(xi->data, img.constBits(), img.byteCount());

    xcb_pixmap_t pix = xcb_generate_id(conn);
    xcb_create_pixmap(conn, 32, pix, screen->root(), w, h);

    xcb_render_picture_t pic = xcb_generate_id(conn);
    xcb_render_create_picture(conn, pic, pix, fmt->id, 0, 0);

    xcb_gcontext_t gc = xcb_generate_id(conn);
    xcb_create_gc(conn, gc, pix, 0, 0);
    xcb_image_put(conn, pix, gc, xi, 0, 0, 0);
    xcb_free_gc(conn, gc);

    xcb_cursor_t cursor = xcb_generate_id(conn);
    xcb_render_create_cursor(conn, cursor, pic, spot.x(), spot.y());

    free(xi->data);
    xcb_image_destroy(xi);
    xcb_render_free_picture(conn, pic);
    xcb_free_pixmap(conn, pix);
    free(formatsReply);
    return cursor;

#else
    Q_UNUSED(screen);
    Q_UNUSED(image);
    Q_UNUSED(spot);
    return XCB_NONE;
#endif
}

static uint8_t cur_blank_bits[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

static xcb_cursor_t overrideCreateNonStandardCursor(QXcbCursor *xcb_cursor, Qt::CursorShape cshape, QWindow *window)
{
    xcb_cursor_t cursor = 0;
    xcb_connection_t *conn = xcb_cursor->xcb_connection();
    QImage image;

    switch (cshape) {
    case Qt::BlankCursor: {
        xcb_pixmap_t cp = xcb_create_pixmap_from_bitmap_data(conn, xcb_cursor->m_screen->root(), cur_blank_bits, 16, 16,
                                                             1, 0, 0, 0);
        xcb_pixmap_t mp = xcb_create_pixmap_from_bitmap_data(conn, xcb_cursor->m_screen->root(), cur_blank_bits, 16, 16,
                                                             1, 0, 0, 0);
        cursor = xcb_generate_id(conn);
        xcb_create_cursor(conn, cursor, cp, mp, 0, 0, 0, 0xFFFF, 0xFFFF, 0xFFFF, 8, 8);

        return cursor;
    }
    case Qt::SizeVerCursor:
        image.load(":/bottom_side.png");
        break;
    case Qt::SizeAllCursor:
        image.load(":/all-scroll.png");
        break;
    case Qt::SplitVCursor:
        image.load(":/sb_v_double_arrow.png");
        break;
    case Qt::SplitHCursor:
        image.load(":/sb_h_double_arrow.png");
        break;
    case Qt::WhatsThisCursor:
        image.load(":/question_arrow.png");
        break;
    case Qt::BusyCursor:
        image.load(":/left_ptr_watch_0001.png");
        break;
    case Qt::ForbiddenCursor:
        image.load(":/crossed_circle.png");
        break;
    case Qt::OpenHandCursor:
        image.load(":/hand1.png");
        break;
    case Qt::ClosedHandCursor:
        image.load(":/grabbing.png");
        break;
    case Qt::DragCopyCursor:
        image.load(":/dnd-copy.png");
        break;
    case Qt::DragMoveCursor:
        image.load(":/dnd-move.png");
        break;
    case Qt::DragLinkCursor:
        image.load(":/dnd-link.png");
        break;
    default:
        break;
    }


    if (!image.isNull()) {
        image = image.scaledToWidth(24 * window->devicePixelRatio());
        cursor = qt_xcb_createCursorXRender(xcb_cursor->m_screen, image, QPoint(8, 8) * window->devicePixelRatio());
    }

    return cursor;
}

static bool updateCursorTheme(void *dpy)
{
    if (!ptrXcursorLibraryGetTheme
            || !ptrXcursorLibrarySetTheme)
        return false;
    QByteArray theme = ptrXcursorLibraryGetTheme(dpy);

    int setTheme = ptrXcursorLibrarySetTheme(dpy,theme.constData());
    return setTheme;
}

static xcb_cursor_t overrideCreateFontCursor(QXcbCursor *xcb_cursor, QCursor *c, QWindow *window)
{
    const Qt::CursorShape cshape = c->shape();
    xcb_connection_t *conn = xcb_cursor->xcb_connection();
    int cursorId = cursorIdForShape(cshape);
    xcb_cursor_t cursor = XCB_NONE;

    // Try Xcursor first
#if defined(XCB_USE_XLIB) && !defined(QT_NO_LIBRARY)
    if (cshape >= 0 && cshape <= Qt::LastCursor) {
        void *dpy = xcb_cursor->connection()->xlib_display();
        // special case for non-standard dnd-* cursors
        cursor = loadCursor(dpy, cshape);

        if (!cursor && !xcb_cursor->m_gtkCursorThemeInitialized) {
            VtableHook::callOriginalFun(xcb_cursor, &QPlatformCursor::changeCursor, c, window);

            if (updateCursorTheme(dpy)) {
                cursor = loadCursor(dpy, cshape);
            }
        }
    }
    if (cursor)
        return cursor;
    if (!cursor && cursorId) {
        cursor = XCreateFontCursor(DISPLAY_FROM_XCB(xcb_cursor), cursorId);
        if (cursor)
            return cursor;
    }

#endif

    // Non-standard X11 cursors are created from bitmaps
    cursor = overrideCreateNonStandardCursor(xcb_cursor, cshape, window);

    // Create a glpyh cursor if everything else failed
    if (!cursor && cursorId) {
        cursor = xcb_generate_id(conn);
        xcb_create_glyph_cursor(conn, cursor, cursorFont, cursorFont,
                                cursorId, cursorId + 1,
                                0xFFFF, 0xFFFF, 0xFFFF, 0, 0, 0);
    }

    if (cursor && cshape >= 0 && cshape < Qt::LastCursor && xcb_cursor->connection()->hasXFixes()) {
        const char *name = cursorNames[cshape];
        xcb_xfixes_set_cursor_name(conn, cursor, strlen(name), name);
    }

    return cursor;
}

static void overrideChangeCursor(QPlatformCursor *cursorHandle, QCursor * cursor, QWindow * widget)
{
    QXcbWindow *w = 0;
    if (widget && widget->handle())
        w = static_cast<QXcbWindow *>(widget->handle());
    else
        // No X11 cursor control when there is no widget under the cursor
        return;

    // set cursor size scale
    static bool xcursrSizeIsSet = qEnvironmentVariableIsSet("XCURSOR_SIZE");

    if (!xcursrSizeIsSet)
        qputenv("XCURSOR_SIZE", QByteArray::number(24 * widget->devicePixelRatio()));

    QXcbCursor *xcb_cursor = static_cast<QXcbCursor*>(cursorHandle);

    xcb_cursor_t c = XCB_CURSOR_NONE;
    if (cursor && cursor->shape() != Qt::BitmapCursor) {
        const QXcbCursorCacheKey key(cursor->shape());
        QXcbCursor::CursorHash::iterator it = xcb_cursor->m_cursorHash.find(key);
        if (it == xcb_cursor->m_cursorHash.end()) {
            it = xcb_cursor->m_cursorHash.insert(key, overrideCreateFontCursor(xcb_cursor, cursor, widget));
        }
        c = it.value();
#if QT_VERSION < QT_VERSION_CHECK(5, 7, 1)
        w->setCursor(c);
#else
        w->setCursor(c, false);
#endif
    }

    VtableHook::callOriginalFun(cursorHandle, &QPlatformCursor::changeCursor, cursor, widget);
}

static void hookXcbCursor(QScreen *screen)
{
    if (screen && screen->handle())
        VtableHook::overrideVfptrFun(screen->handle()->cursor(), &QPlatformCursor::changeCursor, &overrideChangeCursor);
}
#endif

void DPlatformIntegration::initialize()
{
    QXcbIntegration::initialize();

#ifdef Q_OS_LINUX
    m_eventFilter = new XcbNativeEventFilter(defaultConnection());
    qApp->installNativeEventFilter(m_eventFilter);

    if (!qEnvironmentVariableIsSet("DXCB_DISABLE_HOOK_CURSOR")) {
#if defined(XCB_USE_XLIB) && !defined(QT_NO_LIBRARY)
    static bool function_ptrs_not_initialized = true;
    if (function_ptrs_not_initialized) {
        QLibrary xcursorLib(QLatin1String("Xcursor"), 1);
        bool xcursorFound = xcursorLib.load();
        if (!xcursorFound) { // try without the version number
            xcursorLib.setFileName(QLatin1String("Xcursor"));
            xcursorFound = xcursorLib.load();
        }
        if (xcursorFound) {
            ptrXcursorLibraryLoadCursor =
                (PtrXcursorLibraryLoadCursor) xcursorLib.resolve("XcursorLibraryLoadCursor");
            ptrXcursorLibraryGetTheme =
                    (PtrXcursorLibraryGetTheme) xcursorLib.resolve("XcursorGetTheme");
            ptrXcursorLibrarySetTheme =
                    (PtrXcursorLibrarySetTheme) xcursorLib.resolve("XcursorSetTheme");
            ptrXcursorLibraryGetDefaultSize =
                    (PtrXcursorLibraryGetDefaultSize) xcursorLib.resolve("XcursorGetDefaultSize");
        }
        function_ptrs_not_initialized = false;
    }

#endif

    for (QScreen *s : qApp->screens()) {
        hookXcbCursor(s);
    }

    QObject::connect(qApp, &QGuiApplication::screenAdded, qApp, &hookXcbCursor);
#endif
    }

    VtableHook::overrideVfptrFun(qApp->d_func(), &QGuiApplicationPrivate::isWindowBlocked,
                                 this, &DPlatformIntegration::isWindowBlockedHandle);
}

bool DPlatformIntegration::isWindowBlockedHandle(QWindow *window, QWindow **blockingWindow)
{
    if (DFrameWindow *frame = qobject_cast<DFrameWindow*>(window)) {
        return VtableHook::callOriginalFun(qApp->d_func(), &QGuiApplicationPrivate::isWindowBlocked, frame->m_contentWindow, blockingWindow);
    }

    return VtableHook::callOriginalFun(qApp->d_func(), &QGuiApplicationPrivate::isWindowBlocked, window, blockingWindow);
}

DPP_END_NAMESPACE
