/*
 * Copyright (C) 2017 ~ 2017 Deepin Technology Co., Ltd.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

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
