/**
 * Copyright (C) 2017 by Deepin Technology Co., Ltd.
 *
 * Use of this source code is governed by GPLv3
 * that can be found in the LICENSE file.
 **/

#ifndef GENERICPLUGIN_H
#define GENERICPLUGIN_H

#include "global.h"

#include <QtGlobal>

#ifdef Q_OS_LINUX
#include "qxcbintegration.h"
typedef QXcbIntegration DPlatformIntegrationParent;
#elif defined(Q_OS_WIN)
#include "qwindowsgdiintegration.h"
typedef QWindowsGdiIntegration DPlatformIntegrationParent;
#endif

DPP_BEGIN_NAMESPACE

class DPlatformWindowHook;
class XcbNativeEventFilter;
class DPlatformBackingStoreHelper;
class DPlatformOpenGLContextHelper;

class DPlatformIntegration : public DPlatformIntegrationParent
{
public:
    DPlatformIntegration(const QStringList &parameters, int &argc, char **argv);
    ~DPlatformIntegration();

    QPlatformWindow *createPlatformWindow(QWindow *window) const Q_DECL_OVERRIDE;
    QPlatformBackingStore *createPlatformBackingStore(QWindow *window) const Q_DECL_OVERRIDE;
    QPlatformOpenGLContext *createPlatformOpenGLContext(QOpenGLContext *context) const Q_DECL_OVERRIDE;

    QPlatformNativeInterface *nativeInterface()const Q_DECL_OVERRIDE;

    QStringList themeNames() const Q_DECL_OVERRIDE;

    void initialize() Q_DECL_OVERRIDE;

#ifdef Q_OS_LINUX

#if QT_VERSION < QT_VERSION_CHECK(5, 5, 0)
    static DPlatformIntegration *instance() { return m_instance; }

private:
    static DPlatformIntegration *m_instance;
#endif

    inline static QXcbConnection *xcbConnection()
    { return instance()->defaultConnection();}

private:
    XcbNativeEventFilter *m_eventFilter = Q_NULLPTR;
#endif
private:
    // handle the DFrameWindow modal blocked state
    bool isWindowBlockedHandle(QWindow *window, QWindow **blockingWindow);

    QScopedPointer<QPlatformNativeInterface> m_nativeInterface;
    DPlatformBackingStoreHelper *m_storeHelper;
    DPlatformOpenGLContextHelper *m_contextHelper;
};

DPP_END_NAMESPACE

#endif // GENERICPLUGIN_H
