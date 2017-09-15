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

#include "dplatformbackingstorehelper.h"
#include "vtablehook.h"
#include "dplatformwindowhelper.h"
#include "dframewindow.h"
#include "dwmsupport.h"

#include <qpa/qplatformbackingstore.h>

#include <QPainter>
#include <QOpenGLPaintDevice>

DPP_BEGIN_NAMESPACE

DPlatformBackingStoreHelper::DPlatformBackingStoreHelper()
{

}

bool DPlatformBackingStoreHelper::addBackingStore(QPlatformBackingStore *store)
{
    QObject::connect(store->window(), &QWindow::destroyed, store->window(), [store] {
        VtableHook::clearGhostVtable(store);
    });

    return VtableHook::overrideVfptrFun(store, &QPlatformBackingStore::flush, this, &DPlatformBackingStoreHelper::flush);
}

void DPlatformBackingStoreHelper::flush(QWindow *window, const QRegion &region, const QPoint &offset)
{
    if (Q_LIKELY(DWMSupport::instance()->hasComposite())) {
        DPlatformWindowHelper *window_helper = DPlatformWindowHelper::mapped.value(window->handle());

        if (window_helper && (window_helper->m_isUserSetClipPath || window_helper->m_windowRadius > 0)) {
            QPainterPath path;

            path.addRegion(region);
            path -= window_helper->m_clipPath;

            if (path.isEmpty())
                goto end;

            QPainter pa(backingStore()->paintDevice());

            pa.setCompositionMode(QPainter::CompositionMode_Source);
            pa.setRenderHints(QPainter::Antialiasing);
            pa.setClipPath(path);
            pa.drawPixmap(window_helper->m_windowVaildGeometry.topLeft(), window_helper->m_frameWindow->m_shadowPixmap, window_helper->m_frameWindow->m_contentGeometry);
            pa.end();
        }
    }

end:
    return VtableHook::callOriginalFun(this->backingStore(), &QPlatformBackingStore::flush, window, region, offset);
}

DPP_END_NAMESPACE
