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

    QFunctionPointer platformFunction(const QByteArray &function) const Q_DECL_OVERRIDE;
};

DPP_END_NAMESPACE

#endif // DPLATFORMNATIVEINTERFACE_H
