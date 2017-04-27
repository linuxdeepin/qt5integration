/**
 * Copyright (C) 2017 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/
#include "dplatformwindowhelper.h"
#include "dframewindow.h"
#include "vtablehook.h"

#ifdef Q_OS_LINUX
#include "qxcbwindow.h"
#include "dxcbwmsupport.h"
#endif

#include <private/qwindow_p.h>
#include <private/qguiapplication_p.h>

DPP_BEGIN_NAMESPACE

#define HOOK_VFPTR(Fun) VtableHook::overrideVfptrFun(window, &QPlatformWindow::Fun, this, &DPlatformWindowHelper::Fun)
#define CALL this->window()->QNativeWindow

PUBLIC_CLASS(QWindow, DPlatformWindowHelper);
PUBLIC_CLASS(QMouseEvent, DPlatformWindowHelper);

QHash<const QPlatformWindow*, DPlatformWindowHelper*> DPlatformWindowHelper::mapped;

DPlatformWindowHelper::DPlatformWindowHelper(QNativeWindow *window)
    : QObject(window->window())
    , m_nativeWindow(window)
{
    mapped[window] = this;

    m_frameWindow = new DFrameWindow();
    m_frameWindow->setFlags((window->window()->flags() | Qt::FramelessWindowHint | Qt::CustomizeWindowHint) & ~Qt::WindowMinMaxButtonsHint);
    m_frameWindow->create();
    m_frameWindow->installEventFilter(this);
    m_frameWindow->setShadowRaduis(m_shadowRadius);
    m_frameWindow->setShadowColor(m_shadowColor);
    m_frameWindow->setShadowOffset(m_shadowOffset);
    m_frameWindow->setBorderWidth(m_borderWidth);
    m_frameWindow->setBorderColor(m_borderColor);
    m_frameWindow->setEnableSystemMove(m_enableSystemMove);
    m_frameWindow->setEnableSystemResize(m_enableSystemResize);

    window->setParent(m_frameWindow->handle());
    window->window()->installEventFilter(this);
    qt_window_private(window->window())->positionAutomatic = false;
    updateClipPathByWindowRadius(window->window()->size());

    updateClipPathFromProperty();
    updateWindowRadiusFromProperty();
    updateBorderWidthFromProperty();
    updateBorderColorFromProperty();
    updateShadowRadiusFromProperty();
    updateShadowOffsetFromProperty();
    updateShadowColorFromProperty();
    updateEnableSystemResizeFromProperty();
    updateEnableSystemMoveFromProperty();
    updateEnableBlurWindowFromProperty();
    updateWindowBlurAreasFromProperty();
    updateWindowBlurPathsFromProperty();
    updateAutoInputMaskByClipPathFromProperty();

    HOOK_VFPTR(setGeometry);
    HOOK_VFPTR(geometry);
    HOOK_VFPTR(normalGeometry);
    HOOK_VFPTR(frameMargins);
    HOOK_VFPTR(setVisible);
    HOOK_VFPTR(setWindowFlags);
    HOOK_VFPTR(setWindowState);
    HOOK_VFPTR(winId);
    HOOK_VFPTR(setParent);
    HOOK_VFPTR(setWindowTitle);
    HOOK_VFPTR(setWindowFilePath);
    HOOK_VFPTR(setWindowIcon);
    HOOK_VFPTR(raise);
    HOOK_VFPTR(lower);
//    HOOK_VFPTR(isExposed);
    HOOK_VFPTR(isEmbedded);
    HOOK_VFPTR(propagateSizeHints);
    HOOK_VFPTR(requestActivateWindow);
//    HOOK_VFPTR(setKeyboardGrabEnabled);
//    HOOK_VFPTR(setMouseGrabEnabled);
    HOOK_VFPTR(setWindowModified);
//    HOOK_VFPTR(windowEvent);
    HOOK_VFPTR(startSystemResize);
    HOOK_VFPTR(setFrameStrutEventsEnabled);
    HOOK_VFPTR(frameStrutEventsEnabled);
    HOOK_VFPTR(setAlertState);
    HOOK_VFPTR(isAlertState);

    connect(m_frameWindow, &DFrameWindow::contentMarginsHintChanged, this, &DPlatformWindowHelper::updateWindowBlurAreasForWM);
    connect(m_frameWindow, &DFrameWindow::contentMarginsHintChanged, this, &DPlatformWindowHelper::updateSizeHints);
}

DPlatformWindowHelper::~DPlatformWindowHelper()
{
    mapped.remove(m_nativeWindow);
    VtableHook::clearGhostVtable(static_cast<QPlatformWindow*>(m_nativeWindow));

    m_frameWindow->deleteLater();
}

DPlatformWindowHelper *DPlatformWindowHelper::me() const
{
    return DPlatformWindowHelper::mapped.value(window());
}

void DPlatformWindowHelper::setGeometry(const QRect &rect)
{
    DPlatformWindowHelper *helper = me();

    const QMargins &content_margins = helper->m_frameWindow->contentMarginsHint();

    helper->m_frameWindow->setGeometry(rect + content_margins);

    const QPoint window_offset(content_margins.left(), content_margins.top());

    window()->QNativeWindow::setGeometry(QRect(window_offset, rect.size()));
}

QRect DPlatformWindowHelper::geometry() const
{
    DPlatformWindowHelper *helper = me();

    QRect rect = helper->m_frameWindow->handle()->geometry() - helper->m_frameWindow->contentMarginsHint();

    rect.setSize(helper->m_nativeWindow->QNativeWindow::geometry().size());

    return rect;
}

QRect DPlatformWindowHelper::normalGeometry() const
{
    return me()->m_frameWindow->handle()->normalGeometry();
}

QMargins DPlatformWindowHelper::frameMargins() const
{
    return me()->m_frameWindow->handle()->frameMargins();
}

void DPlatformWindowHelper::setVisible(bool visible)
{
    me()->m_frameWindow->setVisible(visible);
    window()->setVisible(visible);

    if (visible) {
        me()->updateWindowBlurAreasForWM();
    }
}

void DPlatformWindowHelper::setWindowFlags(Qt::WindowFlags flags)
{
    me()->m_frameWindow->setFlags((flags | Qt::FramelessWindowHint | Qt::CustomizeWindowHint) & ~Qt::WindowMinMaxButtonsHint);
}

void DPlatformWindowHelper::setWindowState(Qt::WindowState state)
{
    me()->m_frameWindow->setWindowState(state);
}

WId DPlatformWindowHelper::winId() const
{
    return me()->m_frameWindow->handle()->winId();
}

void DPlatformWindowHelper::setParent(const QPlatformWindow *window)
{
    me()->m_frameWindow->handle()->setParent(window);
}

void DPlatformWindowHelper::setWindowTitle(const QString &title)
{
    me()->m_frameWindow->handle()->setWindowTitle(title);
}

void DPlatformWindowHelper::setWindowFilePath(const QString &title)
{
    me()->m_frameWindow->handle()->setWindowFilePath(title);
}

void DPlatformWindowHelper::setWindowIcon(const QIcon &icon)
{
    me()->m_frameWindow->handle()->setWindowIcon(icon);
}

void DPlatformWindowHelper::raise()
{
    me()->m_frameWindow->handle()->raise();
}

void DPlatformWindowHelper::lower()
{
    me()->m_frameWindow->handle()->lower();
}

bool DPlatformWindowHelper::isExposed() const
{
    return me()->m_frameWindow->handle()->isExposed();
}

bool DPlatformWindowHelper::isEmbedded(const QPlatformWindow *parentWindow) const
{
    return me()->m_frameWindow->handle()->isEmbedded(parentWindow);
}

void DPlatformWindowHelper::propagateSizeHints()
{
    me()->updateSizeHints();
}

void DPlatformWindowHelper::requestActivateWindow()
{
    me()->m_frameWindow->handle()->requestActivateWindow();
}

bool DPlatformWindowHelper::setKeyboardGrabEnabled(bool grab)
{
    return me()->m_frameWindow->handle()->setKeyboardGrabEnabled(grab);
}

bool DPlatformWindowHelper::setMouseGrabEnabled(bool grab)
{
    return me()->m_frameWindow->handle()->setMouseGrabEnabled(grab);
}

bool DPlatformWindowHelper::setWindowModified(bool modified)
{
    return me()->m_frameWindow->handle()->setWindowModified(modified);
}

bool DPlatformWindowHelper::startSystemResize(const QPoint &pos, Qt::Corner corner)
{
    return me()->m_frameWindow->handle()->startSystemResize(pos, corner);
}

void DPlatformWindowHelper::setFrameStrutEventsEnabled(bool enabled)
{
    me()->m_frameWindow->handle()->setFrameStrutEventsEnabled(enabled);
}

bool DPlatformWindowHelper::frameStrutEventsEnabled() const
{
    return me()->m_frameWindow->handle()->frameStrutEventsEnabled();
}

void DPlatformWindowHelper::setAlertState(bool enabled)
{
    me()->m_frameWindow->handle()->setAlertState(enabled);
}

bool DPlatformWindowHelper::isAlertState() const
{
    return me()->m_frameWindow->handle()->isAlertState();
}

bool DPlatformWindowHelper::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == m_frameWindow) {
        switch ((int)event->type()) {
        case QEvent::Close:
            m_nativeWindow->window()->close();
            break;
        case QEvent::KeyPress:
        case QEvent::KeyRelease:
        case QEvent::Move:
            QCoreApplication::sendEvent(m_nativeWindow->window(), event);
            return true;
        case QEvent::FocusIn:
            QWindowSystemInterface::handleWindowActivated(m_nativeWindow->window(), static_cast<QFocusEvent*>(event)->reason());
            break;
        case QEvent::WindowActivate:
            QWindowSystemInterface::handleWindowActivated(m_nativeWindow->window(), Qt::OtherFocusReason);
            break;
        case QEvent::Resize:
            m_nativeWindow->window()->resize((m_frameWindow->geometry() - m_frameWindow->contentMarginsHint()).size());
            break;
        case QEvent::MouseButtonPress:
        case QEvent::MouseButtonRelease: {
            DQMouseEvent *e = static_cast<DQMouseEvent*>(event);

            if (m_windowVaildGeometry.contains(e->pos())) {
                e->l = e->w = m_nativeWindow->window()->mapFromGlobal(e->globalPos());
                qApp->sendEvent(m_nativeWindow->window(), e);
            }

            return true;
        }
        case QEvent::WindowStateChange:
            qt_window_private(m_nativeWindow->window())->windowState = m_frameWindow->windowState();
            QCoreApplication::sendEvent(m_nativeWindow->window(), event);
            break;
        }
    } else if (watched == m_nativeWindow->window()) {
        switch ((int)event->type()) {
        case QEvent::MouseMove: {
            if (qApp->mouseButtons() != Qt::LeftButton)
                break;

            static QEvent *last_event = NULL;

            if (last_event == event) {
                last_event = NULL;

                return false;
            }

            last_event = event;
            QCoreApplication::sendEvent(watched, event);

            if (!event->isAccepted()) {
                DQMouseEvent *e = static_cast<DQMouseEvent*>(event);

                e->l = e->w = m_frameWindow->mapFromGlobal(e->globalPos());
                QGuiApplicationPrivate::setMouseEventSource(e, Qt::MouseEventSynthesizedByQt);
                m_frameWindow->mouseMoveEvent(e);
            }
            break;
        }

        case QEvent::DynamicPropertyChange: {
            QDynamicPropertyChangeEvent *e = static_cast<QDynamicPropertyChangeEvent*>(event);

            if (e->propertyName() == netWmStates) {
//                m_store->onWindowStateChanged();
            } else if (e->propertyName() == windowRadius) {
                updateWindowRadiusFromProperty();
            } else if (e->propertyName() == borderWidth) {
                updateBorderWidthFromProperty();
            } else if (e->propertyName() == borderColor) {
                updateBorderColorFromProperty();
            } else if (e->propertyName() == shadowRadius) {
                updateShadowRadiusFromProperty();
            } else if (e->propertyName() == shadowOffset) {
                updateShadowOffsetFromProperty();
            } else if (e->propertyName() == shadowColor) {
                updateShadowColorFromProperty();
            } else if (e->propertyName() == clipPath) {
                updateClipPathFromProperty();
            } else if (e->propertyName() == enableSystemResize) {
                updateEnableSystemResizeFromProperty();
            } else if (e->propertyName() == enableSystemMove) {
                updateEnableSystemMoveFromProperty();
            } else if (e->propertyName() == enableBlurWindow) {
                updateEnableBlurWindowFromProperty();
            } else if (e->propertyName() == windowBlurAreas) {
                updateWindowBlurAreasFromProperty();
            } else if (e->propertyName() == windowBlurPaths) {
                updateWindowBlurPathsFromProperty();
            } else if (e->propertyName() == autoInputMaskByClipPath) {
                updateAutoInputMaskByClipPathFromProperty();
            }

            break;
        }
        case QEvent::Resize:
            if (m_isUserSetClipPath) {
                if (!m_clipPath.isEmpty())
                    updateClipPathByWindowRadius(static_cast<QResizeEvent*>(event)->size());
            } else {
                updateClipPathByWindowRadius(static_cast<QResizeEvent*>(event)->size());

                if (!m_blurAreaList.isEmpty() || !m_blurPathList.isEmpty() || m_enableBlurWindow)
                    updateWindowBlurAreasForWM();
            }
            break;
        }
    }

    return false;
}

void DPlatformWindowHelper::updateClipPathByWindowRadius(const QSize &windowSize)
{
    if (!m_isUserSetClipPath) {
        m_windowVaildGeometry = QRect(QPoint(0, 0), windowSize);

        QPainterPath path;

        path.addRoundedRect(m_windowVaildGeometry, getWindowRadius(), getWindowRadius());
        setClipPath(path);
    }
}

void DPlatformWindowHelper::setClipPath(const QPainterPath &path)
{
    if (m_clipPath == path)
        return;

    m_clipPath = path;

    const QPoint window_offset(m_frameWindow->contentMarginsHint().left(), m_frameWindow->contentMarginsHint().top());

    if (m_isUserSetClipPath) {
        m_windowVaildGeometry = m_clipPath.boundingRect().toRect() & QRect(QPoint(0, 0), m_nativeWindow->window()->size());
        m_frameWindow->setContentPath(m_clipPath.translated(window_offset));
        updateWindowBlurAreasForWM();
    } else {
        m_frameWindow->setContentRoundedRect(m_windowVaildGeometry.translated(window_offset), getWindowRadius());
    }
}

bool DPlatformWindowHelper::updateWindowBlurAreasForWM()
{
    if (!m_frameWindow->isVisible())
        return false;

    const QRect &windowValidRect = m_windowVaildGeometry;

    if (windowValidRect.isEmpty())
        return false;

    quint32 top_level_w = Utility::getNativeTopLevelWindow(m_frameWindow->winId());
    QPoint offset(m_frameWindow->contentMarginsHint().left(), m_frameWindow->contentMarginsHint().top());

    if (top_level_w != m_frameWindow->winId()) {
        offset += Utility::translateCoordinates(QPoint(0, 0), m_frameWindow->winId(), top_level_w);
    }

    QVector<Utility::BlurArea> newAreas;

    if (m_enableBlurWindow) {
        Utility::BlurArea area;

        area.x = windowValidRect.x() + offset.x();
        area.y = windowValidRect.y() + offset.y();
        area.width = windowValidRect.width();
        area.height = windowValidRect.height();
        area.xRadius = getWindowRadius();
        area.yRaduis = getWindowRadius();

        newAreas.append(std::move(area));
    } else {
        newAreas.reserve(m_blurAreaList.size());

        foreach (Utility::BlurArea area, m_blurAreaList) {
            if (area.x < 0) {
                area.width += area.x;
                area.x = 0;
            }

            if (area.y < 0) {
                area.height += area.y;
                area.y = 0;
            }

            area.x += windowValidRect.x();
            area.y += windowValidRect.y();
            area.width = qMin(area.x + area.width, windowValidRect.right() + 1) - area.x;
            area.height = qMin(area.y + area.height, windowValidRect.bottom() + 1) - area.y;
            area.x += offset.x();
            area.y += offset.y();

            newAreas.append(std::move(area));
        }
    }

    if ((m_isUserSetClipPath && !m_clipPath.isEmpty()) || !m_blurPathList.isEmpty()) {
        QList<QPainterPath> newPathList;

        newPathList.reserve(m_blurPathList.size());

        foreach (const QPainterPath &path, m_blurPathList) {
            if (m_clipPath.isEmpty())
                newPathList << path.translated(windowValidRect.topLeft() + offset);
            else
                newPathList << path.translated(windowValidRect.topLeft()).intersected(m_clipPath).translated(offset);
        }

        foreach (const Utility::BlurArea &area, newAreas) {
            QPainterPath path;

            path.addRoundedRect(area.x, area.y, area.width, area.height, area.xRadius, area.yRaduis);

            if (m_clipPath.isEmpty())
                newPathList << path;
            else
                newPathList << path.intersected(m_clipPath.translated(offset));
        }

        return Utility::blurWindowBackgroundByPaths(top_level_w, newPathList);
    }

    return Utility::blurWindowBackground(top_level_w, newAreas);
}

void DPlatformWindowHelper::updateSizeHints()
{
    const QMargins &content_margins = m_frameWindow->contentMarginsHint();
    const QSize extra_size(content_margins.left() + content_margins.right(),
                           content_margins.top() + content_margins.bottom());

    qt_window_private(m_frameWindow)->minimumSize = m_nativeWindow->window()->minimumSize() + extra_size;
    qt_window_private(m_frameWindow)->maximumSize = m_nativeWindow->window()->maximumSize() + extra_size;
    qt_window_private(m_frameWindow)->baseSize = m_nativeWindow->window()->baseSize() + extra_size;
    qt_window_private(m_frameWindow)->sizeIncrement = m_nativeWindow->window()->sizeIncrement();

    m_frameWindow->handle()->propagateSizeHints();
}

int DPlatformWindowHelper::getWindowRadius() const
{
#ifdef Q_OS_LINUX
    return (m_isUserSetWindowRadius || DXcbWMSupport::instance()->hasComposite()) ? m_windowRadius : 0;
#else
    return m_windowRadius;
#endif
}

void DPlatformWindowHelper::updateWindowRadiusFromProperty()
{
    const QVariant &v = m_nativeWindow->window()->property(windowRadius);

    if (!v.isValid()) {
        m_nativeWindow->window()->setProperty(windowRadius, getWindowRadius());

        return;
    }

    bool ok;
    int radius = v.toInt(&ok);

    if (ok && radius != m_windowRadius) {
        m_windowRadius = radius;
        m_isUserSetWindowRadius = true;
        m_isUserSetClipPath = false;

        updateClipPathByWindowRadius(m_nativeWindow->window()->size());
    }
}

void DPlatformWindowHelper::updateBorderWidthFromProperty()
{
    const QVariant &v = m_nativeWindow->window()->property(borderWidth);

    if (!v.isValid()) {
        m_nativeWindow->window()->setProperty(borderWidth, m_borderWidth);

        return;
    }

    bool ok;
    int width = v.toInt(&ok);

    if (ok && width != m_borderWidth) {
        m_borderWidth = width;
        m_frameWindow->setBorderWidth(width);
    }
}

void DPlatformWindowHelper::updateBorderColorFromProperty()
{
    const QVariant &v = m_nativeWindow->window()->property(borderColor);

    if (!v.isValid()) {
        m_nativeWindow->window()->setProperty(borderColor, m_borderColor);

        return;
    }

    const QColor &color = qvariant_cast<QColor>(v);

    if (color.isValid() && m_borderColor != color) {
        m_borderColor = color;
        m_frameWindow->setBorderColor(color);
    }
}

void DPlatformWindowHelper::updateShadowRadiusFromProperty()
{
    const QVariant &v = m_nativeWindow->window()->property(shadowRadius);

    if (!v.isValid()) {
        m_nativeWindow->window()->setProperty(shadowRadius, m_shadowRadius);

        return;
    }

    bool ok;
    int radius = qMax(v.toInt(&ok), 0);

    if (ok && radius != m_shadowRadius) {
        m_shadowRadius = radius;
        m_frameWindow->setShadowRaduis(radius);
    }
}

void DPlatformWindowHelper::updateShadowOffsetFromProperty()
{
    const QVariant &v = m_nativeWindow->window()->property(shadowOffset);

    if (!v.isValid()) {
        m_nativeWindow->window()->setProperty(shadowOffset, m_shadowOffset);

        return;
    }

    const QPoint &offset = v.toPoint();

    if (offset != m_shadowOffset) {
        m_shadowOffset = offset;
        m_frameWindow->setShadowOffset(offset);
    }
}

void DPlatformWindowHelper::updateShadowColorFromProperty()
{
    const QVariant &v = m_nativeWindow->window()->property(shadowColor);

    if (!v.isValid()) {
        m_nativeWindow->window()->setProperty(shadowColor, m_shadowColor);

        return;
    }

    const QColor &color = qvariant_cast<QColor>(v);

    if (color.isValid() && m_shadowColor != color) {
        m_shadowColor = color;
        m_frameWindow->setShadowColor(color);
    }
}

void DPlatformWindowHelper::updateEnableSystemResizeFromProperty()
{
    const QVariant &v = m_nativeWindow->window()->property(enableSystemResize);

    if (!v.isValid()) {
        m_nativeWindow->window()->setProperty(enableSystemResize, m_enableSystemResize);

        return;
    }

    if (m_enableSystemResize == v.toBool())
        return;

    m_enableSystemResize = v.toBool();
    m_frameWindow->setEnableSystemResize(m_enableSystemResize);
}

void DPlatformWindowHelper::updateEnableSystemMoveFromProperty()
{
    const QVariant &v = m_nativeWindow->window()->property(enableSystemMove);

    if (!v.isValid()) {
        m_nativeWindow->window()->setProperty(enableSystemMove, m_enableSystemMove);

        return;
    }

    m_enableSystemMove = v.toBool();
    m_frameWindow->setEnableSystemMove(m_enableSystemMove);
}

void DPlatformWindowHelper::updateEnableBlurWindowFromProperty()
{
    const QVariant &v = m_nativeWindow->window()->property(enableBlurWindow);

    if (!v.isValid()) {
        m_nativeWindow->window()->setProperty(enableBlurWindow, m_enableBlurWindow);

        return;
    }

    if (m_enableBlurWindow != v.toBool()) {
        m_enableBlurWindow = v.toBool();

#ifdef Q_OS_LINUX
        if (m_enableBlurWindow) {
            QObject::connect(DXcbWMSupport::instance(), &DXcbWMSupport::windowManagerChanged,
                             this, &DPlatformWindowHelper::updateWindowBlurAreasForWM);
        } else {
            QObject::disconnect(DXcbWMSupport::instance(), &DXcbWMSupport::windowManagerChanged,
                                this, &DPlatformWindowHelper::updateWindowBlurAreasForWM);
        }
#endif

        updateWindowBlurAreasForWM();
    }
}

void DPlatformWindowHelper::updateWindowBlurAreasFromProperty()
{
    const QVariant &v = m_nativeWindow->window()->property(windowBlurAreas);
    const QVector<quint32> &tmpV = qvariant_cast<QVector<quint32>>(v);
    const QVector<Utility::BlurArea> &a = *(reinterpret_cast<const QVector<Utility::BlurArea>*>(&tmpV));

    if (a.isEmpty() && m_blurAreaList.isEmpty())
        return;

    m_blurAreaList = a;

    updateWindowBlurAreasForWM();
}

void DPlatformWindowHelper::updateWindowBlurPathsFromProperty()
{
    const QVariant &v = m_nativeWindow->window()->property(windowBlurPaths);
    const QList<QPainterPath> paths = qvariant_cast<QList<QPainterPath>>(v);

    if (paths.isEmpty() && m_blurPathList.isEmpty())
        return;

    m_blurPathList = paths;

    updateWindowBlurAreasForWM();
}

void DPlatformWindowHelper::updateAutoInputMaskByClipPathFromProperty()
{
    const QVariant &v = m_nativeWindow->window()->property(autoInputMaskByClipPath);

    if (!v.isValid()) {
        m_nativeWindow->window()->setProperty(autoInputMaskByClipPath, m_autoInputMaskByClipPath);

        return;
    }

    if (m_autoInputMaskByClipPath != v.toBool()) {
        m_autoInputMaskByClipPath = v.toBool();
    }
}

void DPlatformWindowHelper::updateClipPathFromProperty()
{
    const QVariant &v = m_nativeWindow->window()->property(clipPath);

    if (!v.isValid()) {
        return;
    }

    QPainterPath path;

    path = qvariant_cast<QPainterPath>(v);

    if (!m_isUserSetClipPath && path.isEmpty())
        return;

    m_isUserSetClipPath = !path.isEmpty();

    if (m_isUserSetClipPath)
        setClipPath(path);
    else
        updateClipPathByWindowRadius(m_nativeWindow->window()->size());
}

DPP_END_NAMESPACE
