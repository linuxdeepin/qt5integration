#include "dplatformnativeinterface.h"
#include "global.h"
#include "utility.h"
#include "dplatformwindowhook.h"

#ifdef Q_OS_LINUX
#include "dxcbwmsupport.h"
#endif

DPP_BEGIN_NAMESPACE

DPlatformNativeInterface::DPlatformNativeInterface()
{

}

#if QT_VERSION >= QT_VERSION_CHECK(5, 4, 0)
QFunctionPointer DPlatformNativeInterface::platformFunction(const QByteArray &function) const
{
#ifdef Q_OS_LINUX
    if (function == setWmBlurWindowBackgroundArea) {
        return reinterpret_cast<QFunctionPointer>(&Utility::blurWindowBackground);
    } else if (function == setWmBlurWindowBackgroundPathList) {
        return reinterpret_cast<QFunctionPointer>(&Utility::blurWindowBackgroundByPaths);
    } else if (function == setWmBlurWindowBackgroundMaskImage) {
        return reinterpret_cast<QFunctionPointer>(&Utility::blurWindowBackgroundByImage);
    } else if (function == hasBlurWindow) {
        return reinterpret_cast<QFunctionPointer>(&Utility::hasBlurWindow);
    } else if (function == hasComposite) {
        return reinterpret_cast<QFunctionPointer>(&Utility::hasComposite);
    } else if (function == connectWindowManagerChangedSignal) {
        return reinterpret_cast<QFunctionPointer>(&DXcbWMSupport::connectWindowManagerChangedSignal);
    } else if (function == connectHasBlurWindowChanged) {
        return reinterpret_cast<QFunctionPointer>(&DXcbWMSupport::connectHasBlurWindowChanged);
    } else if (function == connectHasCompositeChanged) {
        return reinterpret_cast<QFunctionPointer>(&DXcbWMSupport::connectHasCompositeChanged);
    } else if (function == getWindows) {
        return reinterpret_cast<QFunctionPointer>(&Utility::getWindows);
    } else if (function == getCurrentWorkspaceWindows) {
        return reinterpret_cast<QFunctionPointer>(&Utility::getCurrentWorkspaceWindows);
    } else if (function == connectWindowListChanged) {
        return reinterpret_cast<QFunctionPointer>(&DXcbWMSupport::connectWindowListChanged);
    }
#endif

    return DPlatformNativeInterfaceParent::platformFunction(function);
}
#endif

DPP_END_NAMESPACE
