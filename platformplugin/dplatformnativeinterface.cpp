/**
 * Copyright (C) 2017 by Deepin Technology Co., Ltd.
 *
 * Use of this source code is governed by GPLv3
 * that can be found in the LICENSE file.
 **/

#include "dplatformnativeinterface.h"
#include "global.h"
#include "utility.h"
#include "dplatformwindowhook.h"

#include "dwmsupport.h"

DPP_BEGIN_NAMESPACE

DPlatformNativeInterface::DPlatformNativeInterface()
{

}

#if QT_VERSION >= QT_VERSION_CHECK(5, 4, 0)
QFunctionPointer DPlatformNativeInterface::platformFunction(const QByteArray &function) const
{
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
        return reinterpret_cast<QFunctionPointer>(&DWMSupport::connectWindowManagerChangedSignal);
    } else if (function == connectHasBlurWindowChanged) {
        return reinterpret_cast<QFunctionPointer>(&DWMSupport::connectHasBlurWindowChanged);
    } else if (function == connectHasCompositeChanged) {
        return reinterpret_cast<QFunctionPointer>(&DWMSupport::connectHasCompositeChanged);
    } else if (function == getWindows) {
        return reinterpret_cast<QFunctionPointer>(&Utility::getWindows);
    } else if (function == getCurrentWorkspaceWindows) {
        return reinterpret_cast<QFunctionPointer>(&Utility::getCurrentWorkspaceWindows);
    } else if (function == connectWindowListChanged) {
        return reinterpret_cast<QFunctionPointer>(&DWMSupport::connectWindowListChanged);
    }

    return DPlatformNativeInterfaceParent::platformFunction(function);
}
#endif

DPP_END_NAMESPACE
