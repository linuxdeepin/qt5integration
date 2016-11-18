#include "dxcbintegration.h"
#include "xcbwindowhook.h"
#include "dxcbbackingstore.h"
#include "global.h"
#include "windoweventhook.h"

#include "qxcbscreen.h"
#include "qxcbbackingstore.h"

#if QT_VERSION < QT_VERSION_CHECK(5, 4, 0)
#include <private/qwidgetwindow_qpa_p.h>
#else
#include <private/qwidgetwindow_p.h>
#endif

#include <QWidget>

DXcbIntegration::DXcbIntegration(const QStringList &parameters, int &argc, char **argv)
    : QXcbIntegration(parameters, argc, argv)
{

}

QPlatformWindow *DXcbIntegration::createPlatformWindow(QWindow *window) const
{
    qDebug() << __FUNCTION__ << window << window->type() << window->parent();

    bool isUseDxcb = window->type() != Qt::Desktop && window->property(useDxcb).toBool();

    if (isUseDxcb) {
        QSurfaceFormat format = window->format();

        const int oldAlpha = format.alphaBufferSize();
        const int newAlpha = 8;

        if (oldAlpha != newAlpha) {
            format.setAlphaBufferSize(newAlpha);
            window->setFormat(format);
        }
    }

    QPlatformWindow *w = QXcbIntegration::createPlatformWindow(window);
    QXcbWindow *xw = dynamic_cast<QXcbWindow*>(w);

    if (isUseDxcb) {
        Q_UNUSED(new XcbWindowHook(xw))
    }

    if (window->type() == Qt::Widget
            || window->type() == Qt::Window
            || window->type() == Qt::Dialog) {
        Q_UNUSED(new WindowEventHook(xw))
    }

    return w;
}

QPlatformBackingStore *DXcbIntegration::createPlatformBackingStore(QWindow *window) const
{
    qDebug() << __FUNCTION__ << window << window->type() << window->parent();

    QPlatformBackingStore *store = QXcbIntegration::createPlatformBackingStore(window);

    if (window->type() != Qt::Desktop && window->property(useDxcb).toBool())
        return new DXcbBackingStore(window, static_cast<QXcbBackingStore*>(store));

    return store;
}
