#include "dplatformintegration.h"
#include "dplatformbackingstore.h"
#include "global.h"
#include "dplatformwindowhook.h"
#include "dforeignplatformwindow.h"
#ifdef Q_OS_LINUX
#include "windoweventhook.h"
#include "xcbnativeeventfilter.h"
#include "dplatformnativeinterface.h"

#include "qxcbscreen.h"
#include "qxcbbackingstore.h"
#endif

#if QT_VERSION < QT_VERSION_CHECK(5, 4, 0)
#include <private/qwidgetwindow_qpa_p.h>
#else
#include <private/qwidgetwindow_p.h>
#endif

#include <QWidget>
#include <QGuiApplication>

DPP_BEGIN_NAMESPACE

#ifdef Q_OS_LINUX
#if QT_VERSION < QT_VERSION_CHECK(5, 5, 0)
DPlatformIntegration *DPlatformIntegration::m_instance = Q_NULLPTR;
#endif
#endif

DPlatformIntegration::DPlatformIntegration(const QStringList &parameters, int &argc, char **argv)
    : QXcbIntegration(parameters, argc, argv)
{
#ifdef Q_OS_LINUX
#if QT_VERSION < QT_VERSION_CHECK(5, 5, 0)
    m_instance = this;
#endif
#endif

    m_nativeInterface.reset(new DPlatformNativeInterface());
}

DPlatformIntegration::~DPlatformIntegration()
{
#ifdef Q_OS_LINUX
    if (!m_eventFilter)
        return;

    qApp->removeNativeEventFilter(m_eventFilter);
    delete m_eventFilter;
#endif
}

QPlatformWindow *DPlatformIntegration::createPlatformWindow(QWindow *window) const
{
    qDebug() << __FUNCTION__ << window << window->type() << window->parent();

    // handle foreign native window
    if (window->type() == Qt::ForeignWindow && window->property("_q_foreignWinId").isValid()) {
        return new DForeignPlatformWindow(window);
    }

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
    QXcbWindow *xw = static_cast<QXcbWindow*>(w);

    if (isUseDxcb) {
        Q_UNUSED(new DPlatformWindowHook(xw))
    }

#ifdef Q_OS_LINUX
    if (window->type() == Qt::Widget
            || window->type() == Qt::Window
            || window->type() == Qt::Dialog) {
        Q_UNUSED(new WindowEventHook(xw))
    }
#endif

    return xw;
}

QPlatformBackingStore *DPlatformIntegration::createPlatformBackingStore(QWindow *window) const
{
    qDebug() << __FUNCTION__ << window << window->type() << window->parent();

    QPlatformBackingStore *store = QXcbIntegration::createPlatformBackingStore(window);

    if (window->type() != Qt::Desktop && window->property(useDxcb).toBool())
        return new DPlatformBackingStore(window, static_cast<QXcbBackingStore*>(store));

    return store;
}

QPlatformNativeInterface *DPlatformIntegration::nativeInterface() const
{
    return m_nativeInterface.data();
}

QStringList DPlatformIntegration::themeNames() const
{
    QStringList list = DPlatformIntegrationParent::themeNames();

    list.prepend("deepin");

    return list;
}

#ifdef Q_OS_LINUX
void DPlatformIntegration::initialize()
{
    QXcbIntegration::initialize();

    m_eventFilter = new XcbNativeEventFilter(defaultConnection());
    qApp->installNativeEventFilter(m_eventFilter);
}
#endif

DPP_END_NAMESPACE
