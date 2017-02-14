#include "dplatformnativeinterface.h"
#include "global.h"
#include "utility.h"
#include "dplatformwindowhook.h"

DPP_BEGIN_NAMESPACE

DPlatformNativeInterface::DPlatformNativeInterface()
{

}

#if QT_VERSION >= QT_VERSION_CHECK(5, 4, 0)
QFunctionPointer DPlatformNativeInterface::platformFunction(const QByteArray &function) const
{
    if (function == setWmBlurWindowBackgroundArea) {
        return reinterpret_cast<QFunctionPointer>(&Utility::blurWindowBackground);
    } else if (function == hasBlurWindow) {
        return reinterpret_cast<QFunctionPointer>(&Utility::hasBlurWindow);
    }

    return QXcbNativeInterface::platformFunction(function);
}
#endif

DPP_END_NAMESPACE
