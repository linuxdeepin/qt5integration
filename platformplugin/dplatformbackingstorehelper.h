/**
 * Copyright (C) 2017 by Deepin Technology Co., Ltd.
 *
 * Use of this source code is governed by GPLv3
 * that can be found in the LICENSE file.
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
