#include "dxcbnativeinterface.h"
#include "global.h"
#include "utility.h"
#include "xcbwindowhook.h"

DXcbNativeInterface::DXcbNativeInterface()
{

}

QFunctionPointer DXcbNativeInterface::platformFunction(const QByteArray &function) const
{
    if (function == setWmBlurWindowBackgroundRegion) {
        return reinterpret_cast<QFunctionPointer>(&Utility::blurWindowBackground);
    }

    return QXcbNativeInterface::platformFunction(function);
}
