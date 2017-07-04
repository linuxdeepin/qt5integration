/**
 * Copyright (C) 2017 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/
#ifndef DPLATFORMWINDOWHELPER_H
#define DPLATFORMWINDOWHELPER_H

#include <QtGlobal>

#ifdef Q_OS_LINUX
#define private public
#include "qxcbwindow.h"
#include "qxcbclipboard.h"
typedef QXcbWindow QNativeWindow;
#undef private
#elif defined(Q_OS_WIN)
#include "qwindowswindow.h"
typedef QWindowsWindow QNativeWindow;
#endif

#include "global.h"
#include "utility.h"

DPP_BEGIN_NAMESPACE

class DFrameWindow;
class DPlatformWindowHelper : public QObject
{
public:
    explicit DPlatformWindowHelper(QNativeWindow *window);
    ~DPlatformWindowHelper();

    QNativeWindow *window() const
    { return static_cast<QNativeWindow*>(reinterpret_cast<QPlatformWindow*>(const_cast<DPlatformWindowHelper*>(this)));}

    DPlatformWindowHelper *me() const;

    void setGeometry(const QRect &rect);
    QRect geometry() const;
    QRect normalGeometry() const;

    QMargins frameMargins() const;

    void setVisible(bool visible);
    void setWindowFlags(Qt::WindowFlags flags);
    void setWindowState(Qt::WindowState state);

    WId winId() const;
    void setParent(const QPlatformWindow *window);

    void setWindowTitle(const QString &title);
    void setWindowFilePath(const QString &title);
    void setWindowIcon(const QIcon &icon);
    void raise();
    void lower();

    bool isExposed() const;
    bool isActive() const;
    bool isEmbedded(const QPlatformWindow *parentWindow = 0) const;

    void propagateSizeHints();

    void requestActivateWindow();

    bool setKeyboardGrabEnabled(bool grab);
    bool setMouseGrabEnabled(bool grab);

    bool setWindowModified(bool modified);

    bool startSystemResize(const QPoint &pos, Qt::Corner corner);

    void setFrameStrutEventsEnabled(bool enabled);
    bool frameStrutEventsEnabled() const;

    void setAlertState(bool enabled);
    bool isAlertState() const;

private:
    bool eventFilter(QObject *watched, QEvent *event) Q_DECL_OVERRIDE;

    void updateClipPathByWindowRadius(const QSize &windowSize);
    void setClipPath(const QPainterPath &path);
    bool updateWindowBlurAreasForWM();
    void updateSizeHints();

    int getWindowRadius() const;

    // update propertys
    void updateClipPathFromProperty();
    void updateWindowRadiusFromProperty();
    void updateBorderWidthFromProperty();
    void updateBorderColorFromProperty();
    void updateShadowRadiusFromProperty();
    void updateShadowOffsetFromProperty();
    void updateShadowColorFromProperty();
    void updateEnableSystemResizeFromProperty();
    void updateEnableSystemMoveFromProperty();
    void updateEnableBlurWindowFromProperty();
    void updateWindowBlurAreasFromProperty();
    void updateWindowBlurPathsFromProperty();
    void updateAutoInputMaskByClipPathFromProperty();

    void onFrameWindowContentMarginsHintChanged();

    static QHash<const QPlatformWindow*, DPlatformWindowHelper*> mapped;

    QNativeWindow *m_nativeWindow;
    DFrameWindow *m_frameWindow;

    QRect m_windowVaildGeometry;

    // propertys
    bool m_isUserSetClipPath = false;
    QPainterPath m_clipPath;

    int m_windowRadius = 4;
    bool m_isUserSetWindowRadius = false;

    int m_borderWidth = 1;
    QColor m_borderColor = QColor(0, 0, 0, 255 * 0.15);

    int m_shadowRadius = 60;
    QPoint m_shadowOffset = QPoint(0, 16);
    QColor m_shadowColor = QColor(0, 0, 0, 255 * 0.6);

    bool m_enableSystemResize = true;
    bool m_enableSystemMove = true;
    bool m_enableBlurWindow = false;
    bool m_autoInputMaskByClipPath = true;
    bool m_enableShadow = true;

    QVector<Utility::BlurArea> m_blurAreaList;
    QList<QPainterPath> m_blurPathList;

    friend class DPlatformBackingStoreHelper;
    friend class DPlatformOpenGLContextHelper;
};

DPP_END_NAMESPACE

Q_DECLARE_METATYPE(QPainterPath)

#endif // DPLATFORMWINDOWHELPER_H
