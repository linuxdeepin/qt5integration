/**
 * Copyright (C) 2017 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/
#include "dplatformopenglcontexthelper.h"
#include "vtablehook.h"
#include "dplatformwindowhelper.h"
#include "dframewindow.h"

#include <qpa/qplatformsurface.h>
#include <qpa/qplatformopenglcontext.h>

#include <QOpenGLPaintDevice>
#include <QPainter>
#include <QOpenGLFunctions>
#include <QPainterPathStroker>
#include <QDebug>

DPP_BEGIN_NAMESPACE

DPlatformOpenGLContextHelper::DPlatformOpenGLContextHelper()
{

}

bool DPlatformOpenGLContextHelper::addOpenGLContext(QOpenGLContext *object, QPlatformOpenGLContext *context)
{
    QObject::connect(object, &QOpenGLContext::aboutToBeDestroyed, object, [context] {
        VtableHook::clearGhostVtable(context);
    });

    return VtableHook::overrideVfptrFun(context, &QPlatformOpenGLContext::swapBuffers, this, &DPlatformOpenGLContextHelper::swapBuffers);
}

void DPlatformOpenGLContextHelper::swapBuffers(QPlatformSurface *surface)
{
    if (surface->surface()->surfaceClass() == QSurface::Window) {
        QWindow *window = static_cast<QWindow*>(surface->surface());
        DPlatformWindowHelper *window_helper = DPlatformWindowHelper::mapped.value(window->handle());

        if (!window_helper)
            goto end;

        if (!window_helper->m_isUserSetClipPath && window_helper->m_windowRadius <= 0)
            goto end;

        QPainterPath path;
        const QSize &window_size = window->size();

        path.addRect(QRect(QPoint(0, 0), window_size));
        path -= window_helper->m_clipPath;

        if (path.isEmpty())
            goto end;

        QOpenGLPaintDevice device(window_size);
        QPainter pa_device(&device);

        window_helper->m_frameWindow->setClearContentAreaForShadowPixmap(true);

        pa_device.drawPixmap(window_helper->m_windowVaildGeometry.topLeft(),
                             window_helper->m_frameWindow->m_shadowPixmap,
                             window_helper->m_frameWindow->m_contentGeometry);
        pa_device.setCompositionMode(QPainter::CompositionMode_Source);
        pa_device.setClipPath(path);
        pa_device.drawPixmap(window_helper->m_windowVaildGeometry.topLeft(),
                             window_helper->m_frameWindow->m_shadowPixmap,
                             window_helper->m_frameWindow->m_contentGeometry);
        pa_device.end();
    }

end:
    VtableHook::callOriginalFun(this->context(), &QPlatformOpenGLContext::swapBuffers, surface);
}

DPP_END_NAMESPACE
