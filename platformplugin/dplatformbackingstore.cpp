#include "dplatformbackingstore.h"
#include "dplatformwindowhook.h"
#include "vtablehook.h"
#ifdef Q_OS_LINUX
#include "dxcbwmsupport.h"
#endif

#include "qxcbbackingstore.h"

#include <QDebug>
#include <QPainter>
#include <QEvent>
#include <QWidget>
#include <QOpenGLContext>
#include <QOpenGLFunctions>
#include <QPainterPathStroker>
#include <QGuiApplication>
#include <QVariantAnimation>
#include <QTimer>
#include <QDebug>

#if QT_VERSION < QT_VERSION_CHECK(5, 4, 0)
#include <private/qwidgetwindow_qpa_p.h>
#else
#include <private/qwidgetwindow_p.h>
#endif
#include <qpa/qplatformscreen.h>
#include <qpa/qplatformcursor.h>
#include <qpa/qplatformnativeinterface.h>

DPP_BEGIN_NAMESPACE

PUBLIC_CLASS(QMouseEvent, WindowEventListener);
PUBLIC_CLASS(QWheelEvent, WindowEventListener);
PUBLIC_CLASS(QResizeEvent, WindowEventListener);
PUBLIC_CLASS(QWidget, WindowEventListener);
PUBLIC_CLASS(QWindow, WindowEventListener);

PUBLIC_CLASS(QXcbWindow, DPlatformBackingStore);

class WindowEventListener : public QObject
{
    Q_OBJECT

public:
    explicit WindowEventListener(DPlatformBackingStore *store)
        : QObject(0)
        , m_store(store)
    {
        store->window()->installEventFilter(this);

        cursorAnimation.setDuration(50);
        cursorAnimation.setEasingCurve(QEasingCurve::InExpo);

        connect(&cursorAnimation, &QVariantAnimation::valueChanged,
                this, &WindowEventListener::onAnimationValueChanged);

        startAnimationTimer.setSingleShot(true);
        startAnimationTimer.setInterval(300);

        connect(&startAnimationTimer, &QTimer::timeout,
                this, &WindowEventListener::startAnimation);
    }

    ~WindowEventListener()
    {
        const QWidgetWindow *widgetWindow = m_store->widgetWindow();

        QWidget *widget = widgetWindow->widget();

        if (widget) {
            VtableHook::clearGhostVtable(widget);
        } else {
            VtableHook::clearGhostVtable(m_store->window());
        }
    }

public slots:
    void updateWindowBlurAreasForWM()
    {
        m_store->updateWindowBlurAreasForWM();
    }

protected:
    bool eventFilter(QObject *obj, QEvent *event) Q_DECL_OVERRIDE
    {
        QWindow *window = qobject_cast<QWindow*>(obj);

        if (!window)
            return false;

        const QRect &window_geometry = window->geometry();
//        qDebug() << obj << event->type() << window_geometry;

        switch ((int)event->type()) {
        case QEvent::Wheel: {
            DQWheelEvent *e = static_cast<DQWheelEvent*>(event);

            if (!window_geometry.contains(e->globalPos()))
                return true;

            e->p -= m_store->windowOffset();

            break;
        }
        case QEvent::MouseMove:
        case QEvent::MouseButtonDblClick:
        case QEvent::MouseButtonPress:
        case QEvent::MouseButtonRelease: {
            DQMouseEvent *e = static_cast<DQMouseEvent*>(event);

            e->l -= m_store->windowOffset();
            e->w -= m_store->windowOffset();

            if (window->flags().testFlag(Qt::Popup))
                break;

            const QRect &window_visible_rect = m_store->windowValidRect.translated(window_geometry.topLeft());

            if (window->minimumSize() == window->maximumSize() || !m_store->m_enableSystemResize)
                goto skip_set_cursor;

            if (!leftButtonPressed && (!window_visible_rect.contains(e->globalPos())
                    || !m_store->m_clipPath.contains(e->windowPos()))) {
                if (event->type() == QEvent::MouseMove) {
                    bool isFixedWidth = window->minimumWidth() == window->maximumWidth();
                    bool isFixedHeight = window->minimumHeight() == window->maximumHeight();

                    Utility::CornerEdge mouseCorner;
                    QRect cornerRect;
                    const QRect window_real_geometry = window_visible_rect
                            + QMargins(MOUSE_MARGINS, MOUSE_MARGINS, MOUSE_MARGINS, MOUSE_MARGINS);

                    if (isFixedWidth || isFixedHeight)
                        goto set_edge;

                    /// begin set cursor corner type
                    cornerRect.setSize(QSize(MOUSE_MARGINS * 2, MOUSE_MARGINS * 2));
                    cornerRect.moveTopLeft(window_real_geometry.topLeft());

                    if (cornerRect.contains(e->globalPos())) {
                        mouseCorner = Utility::TopLeftCorner;

                        goto set_cursor;
                    }

                    cornerRect.moveTopRight(window_real_geometry.topRight());

                    if (cornerRect.contains(e->globalPos())) {
                        mouseCorner = Utility::TopRightCorner;

                        goto set_cursor;
                    }

                    cornerRect.moveBottomRight(window_real_geometry.bottomRight());

                    if (cornerRect.contains(e->globalPos())) {
                        mouseCorner = Utility::BottomRightCorner;

                        goto set_cursor;
                    }

                    cornerRect.moveBottomLeft(window_real_geometry.bottomLeft());

                    if (cornerRect.contains(e->globalPos())) {
                        mouseCorner = Utility::BottomLeftCorner;

                        goto set_cursor;
                    }
set_edge:
                    /// begin set cursor edge type
                    if (e->globalX() <= window_visible_rect.x()) {
                        if (isFixedWidth)
                            goto skip_set_cursor;

                        mouseCorner = Utility::LeftEdge;
                    } else if (e->globalX() < window_visible_rect.right()) {
                        if (isFixedHeight)
                            goto skip_set_cursor;

                        if (e->globalY() <= window_visible_rect.y()) {
                            mouseCorner = Utility::TopEdge;
                        } else if (!isFixedWidth || e->globalY() >= window_visible_rect.bottom()) {
                            mouseCorner = Utility::BottomEdge;
                        } else {
                            goto skip_set_cursor;
                        }
                    } else if (!isFixedWidth && (!isFixedHeight || e->globalX() >= window_visible_rect.right())) {
                        mouseCorner = Utility::RightEdge;
                    } else {
                        goto skip_set_cursor;
                    }
set_cursor:
                    Utility::setWindowCursor(window->winId(), mouseCorner);

                    if (qApp->mouseButtons() == Qt::LeftButton) {
                        Utility::startWindowSystemResize(window->winId(), mouseCorner, e->globalPos());

                        cancelAdsorbCursor();
                    } else {
                        adsorbCursor(mouseCorner);
                    }
                } else if (event->type() == QEvent::MouseButtonRelease) {
                    Utility::cancelWindowMoveResize(window->winId());
                }

                return true;
            }
skip_set_cursor:
            if (e->buttons() == Qt::LeftButton) {
                if (e->type() == QEvent::MouseButtonPress)
                    setLeftButtonPressed(true);
                else if (e->type() == QEvent::MouseButtonRelease)
                    setLeftButtonPressed(false);
            } else {
                setLeftButtonPressed(false);
            }

            qApp->setOverrideCursor(window->cursor());

            cancelAdsorbCursor();
            canAdsorbCursor = m_store->m_enableSystemResize;

            break;
        }
        case QEvent::Resize: {
            DQResizeEvent *e = static_cast<DQResizeEvent*>(event);

            const QRect &rect = QRect(QPoint(0, 0), e->size());

            e->s = (rect - m_store->windowMargins).size();

            break;
        }
        case QEvent::Enter:
            canAdsorbCursor = m_store->m_enableSystemResize;

            break;
        case QEvent::Leave:
            canAdsorbCursor = false;
            cancelAdsorbCursor();

            break;
        case QEvent::DynamicPropertyChange: {
            QDynamicPropertyChangeEvent *e = static_cast<QDynamicPropertyChangeEvent*>(event);

            if (e->propertyName() == netWmStates) {
                m_store->onWindowStateChanged();
            } else if (e->propertyName() == windowRadius) {
                m_store->updateWindowRadius();
            } else if (e->propertyName() == borderWidth) {
                m_store->updateBorderWidth();
            } else if (e->propertyName() == borderColor) {
                m_store->updateBorderColor();
            } else if (e->propertyName() == shadowRadius) {
                m_store->updateShadowRadius();
            } else if (e->propertyName() == shadowOffset) {
                m_store->updateShadowOffset();
            } else if (e->propertyName() == shadowColor) {
                m_store->updateShadowColor();
            } else if (e->propertyName() == clipPath) {
                m_store->updateUserClipPath();
            } else if (e->propertyName() == frameMask) {
                m_store->updateFrameMask();
            } else if (e->propertyName() == translucentBackground) {
                m_store->updateTranslucentBackground();
            } else if (e->propertyName() == enableSystemResize) {
                m_store->updateEnableSystemResize();

                if (!m_store->m_enableSystemMove) {
                    qApp->setOverrideCursor(window->cursor());

                    cancelAdsorbCursor();
                    canAdsorbCursor = false;

                    Utility::cancelWindowMoveResize(window->winId());
                }
            } else if (e->propertyName() == enableSystemMove) {
                m_store->updateEnableSystemMove();

                if (!m_store->m_enableSystemMove)
                    Utility::cancelWindowMoveResize(window->winId());

                updateStealMoveEvent();
            } else if (e->propertyName() == enableBlurWindow) {
                m_store->updateEnableBlurWindow();
            } else if (e->propertyName() == windowBlurAreas) {
                m_store->updateWindowBlurAreas();
            }

            break;
        }
        default: break;
        }

        return false;
    }

    void mouseMoveEvent(QMouseEvent *event)
    {
        Q_UNUSED(event);

        ///TODO: Warning: System move finished no mouse release event
        Utility::startWindowSystemMove(reinterpret_cast<QWidget*>(this)->winId());
    }

    void timerEvent(QTimerEvent *event) Q_DECL_OVERRIDE
    {
        if (event->timerId() == m_store->updateShadowTimer.timerId()) {
            m_store->repaintWindowShadow();
        }
    }

private:
    void setLeftButtonPressed(bool pressed)
    {
        if (leftButtonPressed == pressed)
            return;

        if (!pressed)
            Utility::cancelWindowMoveResize(m_store->window()->winId());

        leftButtonPressed = pressed;

        updateStealMoveEvent();
    }

    void updateStealMoveEvent()
    {
        const QWidgetWindow *widgetWindow = m_store->widgetWindow();

        QWidget *widget = widgetWindow->widget();

        if (widget) {
            if (leftButtonPressed && m_store->m_enableSystemMove) {
                VtableHook::overrideVfptrFun(static_cast<DQWidget*>(widget), &DQWidget::mouseMoveEvent,
                                             this, &WindowEventListener::mouseMoveEvent);
            } else {
                VtableHook::resetVfptrFun(static_cast<DQWidget*>(widget), &DQWidget::mouseMoveEvent);
            }
        } else {
            QWindow *window = m_store->window();

            if (leftButtonPressed && m_store->m_enableSystemMove) {
                VtableHook::overrideVfptrFun(static_cast<DQWindow*>(window), &DQWindow::mouseMoveEvent,
                                             this, &WindowEventListener::mouseMoveEvent);
            } else {
                VtableHook::resetVfptrFun(static_cast<DQWindow*>(window), &DQWindow::mouseMoveEvent);
            }
        }
    }

    void adsorbCursor(Utility::CornerEdge cornerEdge)
    {
        lastCornerEdge = cornerEdge;

        if (!canAdsorbCursor)
            return;

        if (cursorAnimation.state() == QVariantAnimation::Running)
            return;

        startAnimationTimer.start();
    }

    void cancelAdsorbCursor()
    {
        QSignalBlocker blocker(&startAnimationTimer);
        Q_UNUSED(blocker)
        startAnimationTimer.stop();
        cursorAnimation.stop();
    }

    void onAnimationValueChanged(const QVariant &value)
    {
        QCursor::setPos(value.toPoint());
    }

    void startAnimation()
    {
        QPoint cursorPos = QCursor::pos();
        QPoint toPos = cursorPos;
        const QRect geometry = m_store->windowValidRect.translated(m_store->window()->position()).adjusted(-1, -1, 1, 1);

        switch (lastCornerEdge) {
        case Utility::TopLeftCorner:
            toPos = geometry.topLeft();
            break;
        case Utility::TopEdge:
            toPos.setY(geometry.y());
            break;
        case Utility::TopRightCorner:
            toPos = geometry.topRight();
            break;
        case Utility::RightEdge:
            toPos.setX(geometry.right());
            break;
        case Utility::BottomRightCorner:
            toPos = geometry.bottomRight();
            break;
        case Utility::BottomEdge:
            toPos.setY(geometry.bottom());
            break;
        case Utility::BottomLeftCorner:
            toPos = geometry.bottomLeft();
            break;
        case Utility::LeftEdge:
            toPos.setX(geometry.x());
            break;
        default:
            break;
        }

        const QPoint &tmp = toPos - cursorPos;

        if (qAbs(tmp.x()) < 3 && qAbs(tmp.y()) < 3)
            return;

        canAdsorbCursor = false;

        cursorAnimation.setStartValue(cursorPos);
        cursorAnimation.setEndValue(toPos);
        cursorAnimation.start();
    }

    /// mouse left button is pressed in window vaild geometry
    bool leftButtonPressed = false;

    bool canAdsorbCursor = false;
    Utility::CornerEdge lastCornerEdge;
    QTimer startAnimationTimer;
    QVariantAnimation cursorAnimation;

    DPlatformBackingStore *m_store;
};

//class DXcbShmGraphicsBuffer : public QPlatformGraphicsBuffer
//{
//public:
//    DXcbShmGraphicsBuffer(QImage *image)
//        : QPlatformGraphicsBuffer(image->size(), QImage::toPixelFormat(image->format()))
//        , m_access_lock(QPlatformGraphicsBuffer::None)
//        , m_image(image)
//    { }

//    bool doLock(AccessTypes access, const QRect &rect) Q_DECL_OVERRIDE
//    {
//        Q_UNUSED(rect);
//        if (access & ~(QPlatformGraphicsBuffer::SWReadAccess | QPlatformGraphicsBuffer::SWWriteAccess))
//            return false;

//        m_access_lock |= access;
//        return true;
//    }
//    void doUnlock() Q_DECL_OVERRIDE { m_access_lock = None; }

//    const uchar *data() const Q_DECL_OVERRIDE { return m_image.bits(); }
//    uchar *data() Q_DECL_OVERRIDE { return m_image.bits(); }
//    int bytesPerLine() const Q_DECL_OVERRIDE { return m_image.bytesPerLine(); }

//    Origin origin() const Q_DECL_OVERRIDE { return QPlatformGraphicsBuffer::OriginTopLeft; }

//private:
//    AccessTypes m_access_lock;
//    QImage *m_image;
//};

DPlatformBackingStore::DPlatformBackingStore(QWindow *window, QXcbBackingStore *proxy)
    : QPlatformBackingStore(window)
    , m_proxy(proxy)
{
    m_eventListener = new WindowEventListener(this);
    shadowPixmap.fill(Qt::transparent);

    initUserPropertys();
    //! Warning: At this point you must be initialized window Margins and window Extents
    updateWindowMargins();
//    updateFrameExtents();

    VtableHook::overrideVfptrFun(static_cast<QXcbWindow*>(window->handle()), &QXcbWindowEventListener::handlePropertyNotifyEvent,
                                 this, &DPlatformBackingStore::handlePropertyNotifyEvent);

    QObject::connect(window, &QWindow::windowStateChanged,
                     m_eventListener, [this] {
        updateWindowMargins(false);
    });
}

DPlatformBackingStore::~DPlatformBackingStore()
{
    delete m_proxy;
    delete m_eventListener;

//    if (m_graphicsBuffer)
//        delete m_graphicsBuffer;

    VtableHook::clearGhostVtable(static_cast<QXcbWindowEventListener*>(static_cast<QXcbWindow*>(window()->handle())));
}

QPaintDevice *DPlatformBackingStore::paintDevice()
{
    return &m_image;
}

void DPlatformBackingStore::flush(QWindow *window, const QRegion &region, const QPoint &offset)
{
    Q_UNUSED(region)

    const QPoint &windowOffset = this->windowOffset();
    QRegion tmp_region;

    QPainter pa(m_proxy->paintDevice());

    pa.setCompositionMode(QPainter::CompositionMode_Source);
    pa.setRenderHint(QPainter::Antialiasing);
    pa.setClipPath(m_windowClipPath);

    for (const QRect &rect : region.rects()) {
        const QRect &tmp_rect = rect.translated(windowOffset);

        pa.drawImage(tmp_rect, m_image, rect);

        if (tmp_rect.size() == m_size) {
            tmp_region += QRect(QPoint(0, 0), m_size);
        } else {
            tmp_region += tmp_rect.united(QRect(windowOffset, m_image.size()));
        }
    }

    pa.end();

//    qDebug() << "flush" << window << tmp_region << offset;

    DPlatformWindowHook *window_hook = DPlatformWindowHook::getHookByWindow(window->handle());

    if (window_hook)
        window_hook->setWindowMargins(QMargins(0, 0, 0, 0));

    m_proxy->flush(window, tmp_region, offset);

    if (window_hook)
        window_hook->setWindowMargins(windowMargins);
}

#ifndef QT_NO_OPENGL
#if QT_VERSION < QT_VERSION_CHECK(5, 4, 0)
void DPlatformBackingStore::composeAndFlush(QWindow *window, const QRegion &region, const QPoint &offset,
                     QPlatformTextureList *textures, QOpenGLContext *context)
{
    Q_UNUSED(textures);
    Q_UNUSED(context);

    flush(window, region, offset);
}
#else
void DPlatformBackingStore::composeAndFlush(QWindow *window, const QRegion &region, const QPoint &offset,
                                       QPlatformTextureList *textures, QOpenGLContext *context,
                                       bool translucentBackground)
{
    Q_UNUSED(textures)
    Q_UNUSED(context)
    Q_UNUSED(translucentBackground)

    flush(window, region, offset);
}
#endif

QImage DPlatformBackingStore::toImage() const
{
    return m_image;
}

#if QT_VERSION < QT_VERSION_CHECK(5, 4, 0)
GLuint DPlatformBackingStore::toTexture(const QRegion &dirtyRegion, QSize *textureSize) const
{
    return m_proxy->toTexture(dirtyRegion, textureSize);
}
#elif QT_VERSION < QT_VERSION_CHECK(5, 5, 0)
GLuint DPlatformBackingStore::toTexture(const QRegion &dirtyRegion, QSize *textureSize, bool *needsSwizzle) const
{
    return m_proxy->toTexture(dirtyRegion, textureSize, needsSwizzle);
}
#else
GLuint DPlatformBackingStore::toTexture(const QRegion &dirtyRegion, QSize *textureSize, TextureFlags *flags) const
{
    return m_proxy->toTexture(dirtyRegion, textureSize, flags);
}
#endif
#endif

#if QT_VERSION >= QT_VERSION_CHECK(5, 5, 0)
QPlatformGraphicsBuffer *DPlatformBackingStore::graphicsBuffer() const
{
//    return m_graphicsBuffer;
    return m_proxy->graphicsBuffer();
}
#endif

void DPlatformBackingStore::resize(const QSize &size, const QRegion &staticContents)
{
//    qDebug() << "resize" << size << staticContents;
#if QT_VERSION > QT_VERSION_CHECK(5, 5, 1)
    if (size == m_image.size())
        return;

    m_image = QImage(size, QImage::Format_ARGB32_Premultiplied);
#else
    const int dpr = int(window()->devicePixelRatio());
    const QSize xSize = size * dpr;
    if (xSize == m_image.size() && dpr == m_image.devicePixelRatio())
        return;

    m_image = QImage(xSize, QImage::Format_ARGB32_Premultiplied);
    m_image.setDevicePixelRatio(dpr);
#endif

//    if (m_graphicsBuffer)
//        delete m_graphicsBuffer;

//    m_graphicsBuffer = new DXcbShmGraphicsBuffer(&m_image);

    m_size = QSize(size.width() + windowMargins.left() + windowMargins.right(),
                   size.height() + windowMargins.top() + windowMargins.bottom());

    m_proxy->resize(m_size, staticContents);

    updateClipPath();
    //! TODO: update window margins
    //    updateWindowMargins();

    if (!isUserSetClipPath || shadowPixmap.isNull()) {
        updateInputShapeRegion();
        updateWindowShadow();

        if (!m_blurAreaList.isEmpty() || m_enableBlurWindow)
            updateWindowBlurAreasForWM();
    } else {
        if (m_enableBlurWindow)
            updateWindowBlurAreasForWM();
    }

    paintWindowShadow();
}

void DPlatformBackingStore::beginPaint(const QRegion &region)
{
    if (m_translucentBackground) {
        QPainter p(paintDevice());
        p.setCompositionMode(QPainter::CompositionMode_Source);
        const QVector<QRect> rects = region.rects();
        const QColor blank = Qt::transparent;
        for (QVector<QRect>::const_iterator it = rects.begin(); it != rects.end(); ++it) {
            const QRect &rect = *it;
            p.fillRect(rect, blank);
        }
    }
}

void DPlatformBackingStore::endPaint()
{
//    m_proxy->endPaint();

//    qDebug() << "end paint";
}

void DPlatformBackingStore::initUserPropertys()
{
    updateWindowRadius();
    updateBorderWidth();
    updateBorderColor();
    updateUserClipPath();
    updateFrameMask();
    updateShadowRadius();
    updateShadowOffset();
    updateShadowColor();
    updateTranslucentBackground();
    updateEnableSystemMove();
    updateEnableSystemResize();
    updateEnableBlurWindow();
    updateWindowBlurAreas();
}

void DPlatformBackingStore::updateWindowMargins(bool repaintShadow)
{
    Qt::WindowState state = window()->windowState();

    const QMargins old_margins = windowMargins;
    const QRect &window_geometry = window()->geometry();

    if (state == Qt::WindowMaximized || state == Qt::WindowFullScreen) {
        setWindowMargins(QMargins(0, 0, 0, 0));
    } else if (state != Qt::WindowMinimized) {
        setWindowMargins(QMargins(m_shadowRadius - m_shadowOffset.x(),
                                  m_shadowRadius - m_shadowOffset.y(),
                                  m_shadowRadius + m_shadowOffset.x(),
                                  m_shadowRadius + m_shadowOffset.y()));
    }

    if (repaintShadow && old_margins != windowMargins) {
        window()->handle()->setGeometry(window_geometry);

        repaintWindowShadow();
    }
}

void DPlatformBackingStore::updateFrameExtents()
{
    const QMargins &borderMargins = QMargins(m_borderWidth, m_borderWidth, m_borderWidth, m_borderWidth);

    QMargins extentsMargins = windowMargins;

    if (canUseClipPath() && !isUserSetClipPath) {
        extentsMargins -= borderMargins;
    }

    Utility::setFrameExtents(window()->winId(), extentsMargins);
}

void DPlatformBackingStore::updateInputShapeRegion()
{
    if (isUserSetClipPath)
        return;

    QRegion region(windowGeometry().adjusted(-MOUSE_MARGINS, -MOUSE_MARGINS, MOUSE_MARGINS, MOUSE_MARGINS));

    Utility::setInputShapeRectangles(window()->winId(), region);
}

void DPlatformBackingStore::updateWindowRadius()
{
    const QVariant &v = window()->property(windowRadius);

    if (!v.isValid()) {
        window()->setProperty(windowRadius, m_windowRadius);

        return;
    }

    bool ok;
    int radius = v.toInt(&ok);

    if (ok && radius != m_windowRadius) {
        m_windowRadius = radius;

        updateClipPath();
    }
}

void DPlatformBackingStore::updateBorderWidth()
{
    const QVariant &v = window()->property(borderWidth);

    if (!v.isValid()) {
        window()->setProperty(borderWidth, m_borderWidth);

        return;
    }

    bool ok;
    int width = v.toInt(&ok);

    if (ok && width != m_borderWidth) {
        m_borderWidth = width;

        updateFrameExtents();
        doDelayedUpdateWindowShadow();
    }
}

void DPlatformBackingStore::updateBorderColor()
{
    const QVariant &v = window()->property(borderColor);

    if (!v.isValid()) {
        window()->setProperty(borderColor, m_borderColor);

        return;
    }

    const QColor &color = qvariant_cast<QColor>(v);

    if (color.isValid() && m_borderColor != color) {
        m_borderColor = color;

        doDelayedUpdateWindowShadow();
    }
}

void DPlatformBackingStore::updateUserClipPath()
{
    const QVariant &v = window()->property(clipPath);

    if (!v.isValid()) {
        window()->setProperty(clipPath, QVariant::fromValue(m_clipPath));

        return;
    }

    QPainterPath path;

    path = qvariant_cast<QPainterPath>(v);

    if (!isUserSetClipPath && path.isEmpty())
        return;

    isUserSetClipPath = !path.isEmpty();

    if (path.isEmpty())
        updateClipPath();
    else
        setClipPah(path);
}

void DPlatformBackingStore::updateClipPath()
{
    if (!isUserSetClipPath) {
        QPainterPath path;

        if (canUseClipPath())
            path.addRoundedRect(QRect(QPoint(0, 0), m_image.size()), m_windowRadius, m_windowRadius);
        else
            path.addRect(0, 0, m_image.width(), m_image.height());

        setClipPah(path);
    }
}

void DPlatformBackingStore::updateFrameMask()
{
    const QVariant &v = window()->property(frameMask);

    if (!v.isValid()) {
        return;
    }

    QRegion region = qvariant_cast<QRegion>(v);

    static_cast<QXcbWindow*>(window()->handle())->QXcbWindow::setMask(region);

    isUserSetFrameMask = !region.isEmpty();
}

void DPlatformBackingStore::updateShadowRadius()
{
    const QVariant &v = window()->property(shadowRadius);

    if (!v.isValid()) {
        window()->setProperty(shadowRadius, m_shadowRadius);

        return;
    }

    bool ok;
    int radius = v.toInt(&ok);

    if (ok && radius != m_shadowRadius) {
        m_shadowRadius = radius;

        updateWindowMargins();
        doDelayedUpdateWindowShadow();
    }
}

void DPlatformBackingStore::updateShadowOffset()
{
    const QVariant &v = window()->property(shadowOffset);

    if (!v.isValid()) {
        window()->setProperty(shadowOffset, m_shadowOffset);

        return;
    }

    const QPoint &offset = v.toPoint();

    if (!offset.isNull() && offset != m_shadowOffset) {
        m_shadowOffset = offset;

        updateWindowMargins();
        doDelayedUpdateWindowShadow();
    }
}

void DPlatformBackingStore::updateShadowColor()
{
    const QVariant &v = window()->property(shadowColor);

    if (!v.isValid()) {
        window()->setProperty(shadowColor, m_shadowColor);

        return;
    }

    const QColor &color = qvariant_cast<QColor>(v);

    if (color.isValid() && m_shadowColor != color) {
        m_shadowColor = color;

        doDelayedUpdateWindowShadow();
    }
}

void DPlatformBackingStore::updateTranslucentBackground()
{
    const QVariant &v = window()->property(translucentBackground);

    if (!v.isValid()) {
        window()->setProperty(translucentBackground, m_translucentBackground);

        return;
    }

    m_translucentBackground = v.toBool();
}

void DPlatformBackingStore::updateEnableSystemResize()
{
    const QVariant &v = window()->property(enableSystemResize);

    if (!v.isValid()) {
        window()->setProperty(enableSystemResize, m_enableSystemResize);

        return;
    }

    m_enableSystemResize = v.toBool();
}

void DPlatformBackingStore::updateEnableSystemMove()
{
    const QVariant &v = window()->property(enableSystemMove);

    if (!v.isValid()) {
        window()->setProperty(enableSystemMove, m_enableSystemMove);

        return;
    }

    m_enableSystemMove = v.toBool();
}

void DPlatformBackingStore::updateEnableBlurWindow()
{
    const QVariant &v = window()->property(enableBlurWindow);

    if (!v.isValid()) {
        window()->setProperty(enableBlurWindow, m_enableBlurWindow);

        return;
    }

    if (m_enableBlurWindow != v.toBool()) {
        m_enableBlurWindow = v.toBool();

#ifdef Q_OS_LINUX
        if (m_enableBlurWindow) {
            QObject::connect(DXcbWMSupport::instance(), &DXcbWMSupport::windowManagerChanged,
                             m_eventListener, &WindowEventListener::updateWindowBlurAreasForWM);
        } else {
            QObject::disconnect(DXcbWMSupport::instance(), &DXcbWMSupport::windowManagerChanged,
                                m_eventListener, &WindowEventListener::updateWindowBlurAreasForWM);
        }
#endif

        updateWindowBlurAreasForWM();
    }
}

void DPlatformBackingStore::updateWindowBlurAreas()
{
    const QVariant &v = window()->property(windowBlurAreas);
    const QVector<quint32> &tmpV = qvariant_cast<QVector<quint32>>(v);
    const QVector<Utility::BlurArea> &a = *(reinterpret_cast<const QVector<Utility::BlurArea>*>(&tmpV));

    if (a.isEmpty() && m_blurAreaList.isEmpty())
        return;

    m_blurAreaList = a;

    updateWindowBlurAreasForWM();
}

void DPlatformBackingStore::setWindowMargins(const QMargins &margins)
{
    if (windowMargins == margins)
        return;

    windowMargins = margins;
    m_windowClipPath = m_clipPath.translated(windowOffset());

    DPlatformWindowHook *hook = DPlatformWindowHook::getHookByWindow(m_proxy->window()->handle());

    if (hook) {
        hook->setWindowMargins(margins, true);
    }

    const QSize &tmp_size = m_image.size();

    m_size = QSize(tmp_size.width() + windowMargins.left() + windowMargins.right(),
                   tmp_size.height() + windowMargins.top() + windowMargins.bottom());

    m_proxy->resize(m_size, QRegion());

    updateInputShapeRegion();
    updateFrameExtents();

    if (!m_blurAreaList.isEmpty())
        updateWindowBlurAreasForWM();

    window()->setProperty(frameMargins, QVariant::fromValue(windowMargins));
}

void DPlatformBackingStore::setClipPah(const QPainterPath &path)
{
    if (m_clipPath != path) {
        m_clipPath = path;
        m_windowClipPath = m_clipPath.translated(windowOffset());
        windowValidRect = m_clipPath.boundingRect().toRect();

        if (isUserSetClipPath) {
            doDelayedUpdateWindowShadow();
        }
    }
}

void DPlatformBackingStore::paintWindowShadow(QRegion region)
{
    QPainter pa;

    /// begin paint window drop shadow
    pa.begin(m_proxy->paintDevice());
    pa.setCompositionMode(QPainter::CompositionMode_Source);
    pa.drawPixmap(0, 0, shadowPixmap);
    pa.end();

    DPlatformWindowHook *window_hook = DPlatformWindowHook::getHookByWindow(window()->handle());

    if (window_hook)
        window_hook->setWindowMargins(QMargins(0, 0, 0, 0));

    if (region.isEmpty()) {
        region += QRect(windowMargins.left(), 0, m_image.width(), windowMargins.top());
        region += QRect(windowMargins.left(), windowMargins.top() + m_image.height(), m_image.width(), windowMargins.bottom());
        region += QRect(0, 0, windowMargins.left(), m_size.height());
        region += QRect(windowMargins.left() + m_image.width(), 0, windowMargins.right(), m_size.height());
    }

    m_proxy->flush(window(), region, QPoint(0, 0));

    if (window_hook)
        window_hook->setWindowMargins(windowMargins);
    /// end
}

void DPlatformBackingStore::repaintWindowShadow()
{
    updateShadowTimer.stop();

    shadowPixmap = QPixmap();

    updateWindowShadow();
    paintWindowShadow(QRegion(0, 0, m_size.width(), m_size.height()));

    flush(window(), QRect(QPoint(0, 0), m_image.size()), QPoint(0, 0));
}

inline QSize margins2Size(const QMargins &margins)
{
    return QSize(margins.left() + margins.right(),
                 margins.top() + margins.bottom());
}

void DPlatformBackingStore::updateWindowShadow()
{
    QPixmap pixmap(m_image.size());

    if (pixmap.isNull())
        return;

    pixmap.fill(Qt::transparent);

    QPainter pa(&pixmap);

    pa.fillPath(m_clipPath, m_shadowColor);
    pa.end();

    bool paintShadow = isUserSetClipPath || shadowPixmap.isNull();

    if (!paintShadow) {
        QSize margins_size = margins2Size(windowMargins + m_windowRadius + m_borderWidth);

        if (margins_size.width() > qMin(m_size.width(), shadowPixmap.width())
                || margins_size.height() > qMin(m_size.height(), shadowPixmap.height())) {
            paintShadow = true;
        }
    }

    if (paintShadow) {
        QImage image = Utility::dropShadow(pixmap, m_shadowRadius, m_shadowColor);

        /// begin paint window border;
        QPainter pa(&image);
        QPainterPathStroker pathStroker;

        pathStroker.setWidth(m_borderWidth * 2);

        QTransform transform = pa.transform();
        const QRectF &clipRect = m_clipPath.boundingRect();

        transform.translate(windowMargins.left() + 2, windowMargins.top() + 2);
        transform.scale((clipRect.width() - 4) / clipRect.width(),
                        (clipRect.height() - 4) / clipRect.height());

//        pa.setCompositionMode(QPainter::CompositionMode_Source);
        pa.setRenderHint(QPainter::Antialiasing);
        pa.fillPath(pathStroker.createStroke(m_windowClipPath), m_borderColor);
        pa.setCompositionMode(QPainter::CompositionMode_Clear);
        pa.setRenderHint(QPainter::Antialiasing, false);
        pa.setTransform(transform);
        pa.fillPath(m_clipPath, Qt::transparent);
        pa.end();
        /// end

        shadowPixmap = QPixmap::fromImage(image);
    } else {
        shadowPixmap = QPixmap::fromImage(Utility::borderImage(shadowPixmap, windowMargins + m_windowRadius, m_size));
    }
}

bool DPlatformBackingStore::updateWindowBlurAreasForWM()
{
    QVector<Utility::BlurArea> newAreas;
    QRect windowValidRect = windowGeometry();

    if (m_enableBlurWindow) {
        Utility::BlurArea area;

        area.x = windowValidRect.x();
        area.y = windowValidRect.y();
        area.width = windowValidRect.width();
        area.height = windowValidRect.height();
        area.xRadius = m_windowRadius;
        area.yRaduis = m_windowRadius;

        newAreas.append(std::move(area));
    } else {
        newAreas.reserve(m_blurAreaList.size());

        foreach (Utility::BlurArea area, m_blurAreaList) {
            area.x += windowValidRect.x();
            area.y += windowValidRect.y();
            area.width = qMin(area.x + area.width, (quint32)windowValidRect.right() + 1) - area.x;
            area.height = qMin(area.y + area.height, (quint32)windowValidRect.bottom() + 1) - area.y;

            newAreas.append(std::move(area));
        }
    }

    return Utility::blurWindowBackground(window()->winId(), newAreas);
}

void DPlatformBackingStore::doDelayedUpdateWindowShadow(int delaye)
{
    if (m_eventListener)
        updateShadowTimer.start(delaye, m_eventListener);
}

bool DPlatformBackingStore::isWidgetWindow(const QWindow *window)
{
    return window->metaObject()->className() == QStringLiteral("QWidgetWindow");
}

QWidgetWindow *DPlatformBackingStore::widgetWindow() const
{
    return static_cast<QWidgetWindow*>(window());
}

bool DPlatformBackingStore::canUseClipPath() const
{
    QXcbWindow::NetWmStates states = (QXcbWindow::NetWmStates)window()->property(netWmStates).toInt();

    if (states & (QXcbWindow::NetWmStateFullScreen | QXcbWindow::NetWmStateMaximizedHorz | QXcbWindow::NetWmStateMaximizedVert)) {
        return false;
    }

    return true;
}

void DPlatformBackingStore::onWindowStateChanged()
{
    updateClipPath();
    updateFrameExtents();
    doDelayedUpdateWindowShadow();
}

void DPlatformBackingStore::handlePropertyNotifyEvent(const xcb_property_notify_event_t *event)
{
    DQXcbWindow *window = static_cast<DQXcbWindow*>(reinterpret_cast<QXcbWindowEventListener*>(this));
    QWindow *ww = window->window();

    Qt::WindowState oldState = ww->windowState();

    window->QXcbWindow::handlePropertyNotifyEvent(event);

    if (event->window == window->xcb_window()
            && event->atom == window->atom(QXcbAtom::_NET_WM_STATE)) {
        QXcbWindow::NetWmStates states = window->netWmStates();

        ww->setProperty(netWmStates, (int)states);
    }

    if (window->m_windowState != oldState) {
        ww->setWindowState(window->m_windowState);
    }
}

DPP_END_NAMESPACE

#include "dplatformbackingstore.moc"
