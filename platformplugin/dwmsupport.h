/**
 * Copyright (C) 2017 by Deepin Technology Co., Ltd.
 *
 * Use of this source code is governed by GPLv3
 * that can be found in the LICENSE file.
 **/

#ifndef DWMSUPPORT_H
#define DWMSUPPORT_H

#include <QtGlobal>

#include "global.h"

DPP_USE_NAMESPACE

#ifdef Q_OS_LINUX
#include "dxcbwmsupport.h"
typedef DXcbWMSupport DWMSupport;
#elif defined(Q_OS_WIN)
#endif

#endif // DWMSUPPORT_H
