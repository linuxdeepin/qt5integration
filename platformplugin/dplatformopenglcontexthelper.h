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

#ifndef DPLATFORMOPENGLCONTEXTHELPER_H
#define DPLATFORMOPENGLCONTEXTHELPER_H

#include <QtGlobal>

#include "global.h"

QT_BEGIN_NAMESPACE
class QPlatformOpenGLContext;
class QPlatformSurface;
class QOpenGLContext;
QT_END_NAMESPACE

DPP_BEGIN_NAMESPACE

class DPlatformOpenGLContextHelper
{
public:
    DPlatformOpenGLContextHelper();

    bool addOpenGLContext(QOpenGLContext *object, QPlatformOpenGLContext *context);

    QPlatformOpenGLContext *context() const
    { return reinterpret_cast<QPlatformOpenGLContext*>(const_cast<DPlatformOpenGLContextHelper*>(this));}

    void initialize();
    void swapBuffers(QPlatformSurface *surface);
};

DPP_END_NAMESPACE

#endif // DPLATFORMOPENGLCONTEXTHELPER_H
