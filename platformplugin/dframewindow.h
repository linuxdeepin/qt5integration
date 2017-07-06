/**
 * Copyright (C) 2017 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/
#ifndef DFRAMEWINDOW_H
#define DFRAMEWINDOW_H

#include "global.h"
#include "utility.h"

#include <QRasterWindow>
#include <QVariantAnimation>
#include <QTimer>
#include <QPointer>

DPP_BEGIN_NAMESPACE

class DFrameWindow : public QRasterWindow
{
    Q_OBJECT

public:
    explicit DFrameWindow();

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
    void sizeChanged(const QSize &size);
    void contentMarginsHintChanged(const QMargins &oldMargins) const;

protected:
    void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;
    void showEvent(QShowEvent *event) Q_DECL_OVERRIDE;
    void mouseMoveEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void resizeEvent(QResizeEvent *event) Q_DECL_OVERRIDE;
    bool event(QEvent *event) Q_DECL_OVERRIDE;

private:
    void setContentPath(const QPainterPath &path, bool isRoundedRect, int radius = 0);

    void updateShadowPixmap();
    void updateContentMarginsHint();
    void updateMask();
    inline int getShadowRadius() const
    { return m_enableShadow ? m_shadowRadius : 0;}

    bool canResize() const;
    void cancelAdsorbCursor();
    void adsorbCursor(Utility::CornerEdge cornerEdge);
    void startCursorAnimation();

    QPixmap m_shadowPixmap;
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

    bool m_enableShadow = true;
    bool m_enableSystemResize = true;
    bool m_enableSystemMove = true;
    bool m_enableAutoInputMaskByContentPath = true;

    bool m_canAdsorbCursor = false;
    Utility::CornerEdge m_lastCornerEdge;
    QTimer m_startAnimationTimer;
    QVariantAnimation m_cursorAnimation;

    QPointer<QWindow> m_contentWindow;

    friend class DPlatformWindowHelper;
    friend class DPlatformBackingStoreHelper;
    friend class DPlatformOpenGLContextHelper;
    friend class DPlatformIntegration;
    friend class WindowEventHook;
};

DPP_END_NAMESPACE

#endif // DFRAMEWINDOW_H
