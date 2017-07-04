/**
 * Copyright (C) 2017 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
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
