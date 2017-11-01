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

#ifndef DFRAMEWINDOW_H
#define DFRAMEWINDOW_H

#include "global.h"
#include "utility.h"

#include <QPaintDeviceWindow>
#include <QVariantAnimation>
#include <QTimer>
#include <QPointer>

QT_BEGIN_NAMESPACE
class QPlatformBackingStore;
QT_END_NAMESPACE

DPP_BEGIN_NAMESPACE

class DFrameWindow : public QPaintDeviceWindow
{
    Q_OBJECT

public:
    explicit DFrameWindow();
    ~DFrameWindow();

    int shadowRadius() const;
    void setShadowRaduis(int radius);
    QPoint shadowOffset() const;
    void setShadowOffset(const QPoint &offset);
    QColor shadowColor() const;
    void setShadowColor(const QColor &color);

    int borderWidth() const;
    void setBorderWidth(int width);
    QColor borderColor() const;
    void setBorderColor(const QColor &color);

    QPainterPath contentPath() const;
    void setContentPath(const QPainterPath &path);
    void setContentRoundedRect(const QRect &rect, int radius = 0);

    QMargins contentMarginsHint() const;
    QPoint contentOffsetHint() const;

    bool isClearContentAreaForShadowPixmap() const;
    void setClearContentAreaForShadowPixmap(bool clear);

    bool isEnableSystemResize() const;
    void setEnableSystemResize(bool enable);
    bool isEnableSystemMove() const;
    void setEnableSystemMove(bool enable);

signals:
    void contentMarginsHintChanged(const QMargins &oldMargins) const;

protected:
    void paintEvent(QPaintEvent *) Q_DECL_OVERRIDE;
    void showEvent(QShowEvent *event) Q_DECL_OVERRIDE;
    void mouseMoveEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void mouseReleaseEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void resizeEvent(QResizeEvent *event) Q_DECL_OVERRIDE;
    bool event(QEvent *event) Q_DECL_OVERRIDE;

private:
    QPaintDevice *redirected(QPoint *) const Q_DECL_OVERRIDE;

    void setContentPath(const QPainterPath &path, bool isRoundedRect, int radius = 0);

    void updateShadow();
    void updateContentMarginsHint();
    void updateMask();
    void updateFrameMask();

    bool canResize() const;
    void cancelAdsorbCursor();
    void adsorbCursor(Utility::CornerEdge cornerEdge);
    void startCursorAnimation();

    static QList<DFrameWindow*> frameWindowList;

    QPlatformBackingStore *platformBackingStore;

    QImage m_shadowImage;
    bool m_clearContent = false;

    int m_shadowRadius = 60;
    QPoint m_shadowOffset = QPoint(0, 16);
    QColor m_shadowColor = QColor(0, 0, 0, 255 * 0.6);

    int m_borderWidth = 1;
    QColor m_borderColor = QColor(0, 0, 0, 255 * 0.15);
    QPainterPath m_clipPathOfContent;
    QRect m_contentGeometry;
    QMargins m_contentMarginsHint;
    bool m_pathIsRoundedRect = true;
    int m_roundedRectRadius = 0;

    bool m_enableSystemResize = true;
    bool m_enableSystemMove = true;
    bool m_enableAutoInputMaskByContentPath = true;
    bool m_enableAutoFrameMask = true;

    bool m_canAdsorbCursor = false;
    bool m_isSystemMoveResizeState = false;
    Utility::CornerEdge m_lastCornerEdge;
    QTimer m_startAnimationTimer;
    QVariantAnimation m_cursorAnimation;

    QPointer<QWindow> m_contentWindow;

    friend class DPlatformWindowHelper;
    friend class DPlatformBackingStoreHelper;
    friend class DPlatformOpenGLContextHelper;
    friend class DPlatformIntegration;
    friend class WindowEventHook;
    friend class DXcbWMSupport;
    friend class DFrameWindowPrivate;
};

DPP_END_NAMESPACE

#endif // DFRAMEWINDOW_H
