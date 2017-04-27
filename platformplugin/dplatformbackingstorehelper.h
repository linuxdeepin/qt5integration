/**
 * Copyright (C) 2017 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/
#ifndef DPLATFORMBACKINGSTOREHELPER_H
#define DPLATFORMBACKINGSTOREHELPER_H

#include <QtGlobal>

#include "global.h"

QT_BEGIN_NAMESPACE
class QPlatformBackingStore;
class QWindow;
class QRegion;
class QPoint;
QT_END_NAMESPACE

DPP_BEGIN_NAMESPACE

class DPlatformBackingStoreHelper
{
public:
    DPlatformBackingStoreHelper();

    bool addBackingStore(QPlatformBackingStore *store);

    QPlatformBackingStore *backingStore() const
    { return reinterpret_cast<QPlatformBackingStore*>(const_cast<DPlatformBackingStoreHelper*>(this));}

    void flush(QWindow *window, const QRegion &region, const QPoint &offset);
};

DPP_END_NAMESPACE

#endif // DPLATFORMBACKINGSTOREHELPER_H
