/**
 * Copyright (C) 2017 by Deepin Technology Co., Ltd.
 *
 * Use of this source code is governed by GPLv3
 * that can be found in the LICENSE file.
 **/

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
