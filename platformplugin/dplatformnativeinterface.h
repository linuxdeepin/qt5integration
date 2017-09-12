/**
 * Copyright (C) 2017 by Deepin Technology Co., Ltd.
 *
 * Use of this source code is governed by GPLv3
 * that can be found in the LICENSE file.
 **/

#ifndef DPLATFORMNATIVEINTERFACE_H
#define DPLATFORMNATIVEINTERFACE_H

#include <QtGlobal>

#ifdef Q_OS_LINUX
#include "qxcbnativeinterface.h"
typedef QXcbNativeInterface DPlatformNativeInterfaceParent;
#elif defined(Q_OS_WIN)
#include "qwindowsgdinativeinterface.h"
typedef QWindowsGdiNativeInterface DPlatformNativeInterfaceParent;
#endif

#include "global.h"

DPP_BEGIN_NAMESPACE

class DPlatformNativeInterface : public DPlatformNativeInterfaceParent
{
public:
    DPlatformNativeInterface();

#if QT_VERSION >= QT_VERSION_CHECK(5, 4, 0)
    QFunctionPointer platformFunction(const QByteArray &function) const Q_DECL_OVERRIDE;
#endif
};

DPP_END_NAMESPACE

#endif // DPLATFORMNATIVEINTERFACE_H
