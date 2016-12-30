#include "dplatformnativeinterface.h"
#include "global.h"
#include "utility.h"
#include "dplatformwindowhook.h"

DPP_BEGIN_NAMESPACE

DPlatformNativeInterface::DPlatformNativeInterface()
{

}

QFunctionPointer DPlatformNativeInterface::platformFunction(const QByteArray &function) const
{
    if (function == setWmBlurWindowBackgroundRegion) {
        return reinterpret_cast<QFunctionPointer>(&Utility::blurWindowBackground);
    }

    return QXcbNativeInterface::platformFunction(function);
}

DPP_END_NAMESPACE
