#include "dplatformintegration.h"
#include "dplatformbackingstore.h"
#include "global.h"
#include "dplatformwindowhelper.h"
#include "dforeignplatformwindow.h"
#include "dplatformbackingstorehelper.h"
#include "dplatformopenglcontexthelper.h"
#include "dframewindow.h"
#include "vtablehook.h"
#include "dplatformwindowhook.h"

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

#include <private/qguiapplication_p.h>

DPP_BEGIN_NAMESPACE

#ifdef Q_OS_LINUX
#if QT_VERSION < QT_VERSION_CHECK(5, 5, 0)
DPlatformIntegration *DPlatformIntegration::m_instance = Q_NULLPTR;
#endif
#endif

DPlatformIntegration::DPlatformIntegration(const QStringList &parameters, int &argc, char **argv)
    : DPlatformIntegrationParent(parameters, argc, argv)
    , m_storeHelper(new DPlatformBackingStoreHelper)
    , m_contextHelper(new DPlatformOpenGLContextHelper)
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

    delete m_storeHelper;
    delete m_contextHelper;
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

    QPlatformWindow *w = DPlatformIntegrationParent::createPlatformWindow(window);
    QNativeWindow *xw = static_cast<QNativeWindow*>(w);

    if (isUseDxcb) {
#ifdef USE_NEW_IMPLEMENTING
        Q_UNUSED(new DPlatformWindowHelper(xw))
#else
        Q_UNUSED(new DPlatformWindowHook(xw))
#endif
    }

#ifdef Q_OS_LINUX
    if (window->type() == Qt::Widget
            || window->type() == Qt::Window
            || window->type() == Qt::Dialog) {
        Q_UNUSED(new WindowEventHook(xw))
    }
#endif

    QWindow *tp_for_window = window->transientParent();

    if (tp_for_window) {
        // reset transient parent
        if (DPlatformWindowHelper *helper = DPlatformWindowHelper::mapped.value(tp_for_window->handle())) {
            window->setTransientParent(helper->m_frameWindow);
        }
    }

    return xw;
}

QPlatformBackingStore *DPlatformIntegration::createPlatformBackingStore(QWindow *window) const
{
    qDebug() << __FUNCTION__ << window << window->type() << window->parent();

    QPlatformBackingStore *store = DPlatformIntegrationParent::createPlatformBackingStore(window);

    if (window->type() != Qt::Desktop && window->property(useDxcb).toBool())
#ifdef USE_NEW_IMPLEMENTING
        m_storeHelper->addBackingStore(store);
#else
        return new DPlatformBackingStore(window, static_cast<QXcbBackingStore*>(store));
#endif

    return store;
}

QPlatformOpenGLContext *DPlatformIntegration::createPlatformOpenGLContext(QOpenGLContext *context) const
{
    QPlatformOpenGLContext *p_context = DPlatformIntegrationParent::createPlatformOpenGLContext(context);

//    m_contextHelper->addOpenGLContext(context, p_context);

    return p_context;
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

void DPlatformIntegration::initialize()
{
    QXcbIntegration::initialize();

#ifdef Q_OS_LINUX
    m_eventFilter = new XcbNativeEventFilter(defaultConnection());
    qApp->installNativeEventFilter(m_eventFilter);
#endif

    VtableHook::overrideVfptrFun(qApp->d_func(), &QGuiApplicationPrivate::isWindowBlocked,
                                 this, &DPlatformIntegration::isWindowBlockedHandle);
}

bool DPlatformIntegration::isWindowBlockedHandle(QWindow *window, QWindow **blockingWindow)
{
    if (DFrameWindow *frame = qobject_cast<DFrameWindow*>(window)) {
        return VtableHook::callOriginalFun(qApp->d_func(), &QGuiApplicationPrivate::isWindowBlocked, frame->m_contentWindow, blockingWindow);
    }

    return VtableHook::callOriginalFun(qApp->d_func(), &QGuiApplicationPrivate::isWindowBlocked, window, blockingWindow);
}

DPP_END_NAMESPACE
