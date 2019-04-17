/*
 * Copyright (C) 2017 ~ 2018 Deepin Technology Co., Ltd.
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

#include "qdeepintheme.h"
#include "qdeepinfiledialoghelper.h"
#include "diconproxyengine.h"
#include "filedialogmanager_interface.h"
#include "dthemesettings.h"
#include "diconengine.h"

#include <QVariant>
#include <QDebug>
#include <QGuiApplication>
#include <QPainter>
#include <QPalette>

#include <XdgIcon>
#if XDG_ICON_VERSION_MAR >= 3
#define private public
#include <private/xdgiconloader/xdgiconloader_p.h>
#undef private
#elif XDG_ICON_VERSION_MAR == 2
//这个版本中的xdgiconloader_p.h定义和qiconloader_p.h有冲突
//只能通过此方式提供创建XdgIconLoaderEngine对象的接口
#include "xdgiconenginecreator.h"
#endif

#include <private/qicon_p.h>
#include <private/qiconloader_p.h>
#define private public
#include <private/qhighdpiscaling_p.h>
#undef private
#include <private/qwindow_p.h>
#include <private/qguiapplication_p.h>
#include <qpa/qwindowsysteminterface_p.h>
#include <qpa/qplatformscreen.h>
#include <qpa/qplatformcursor.h>

#undef signals
#include <X11/Xlib.h>

#include <cxxabi.h>

#if XDG_ICON_VERSION_MAR >= 3
namespace DEEPIN_QT_THEME {
QThreadStorage<QString> colorScheme;
void(*setFollowColorScheme)(bool);
bool(*followColorScheme)();
}
#endif

#define DISABLE_UPDATE_WINDOW_GEOMETRY "D_DISABLE_UPDATE_WINDOW_GEOMETRY_FOR_SCALE"
#define DNOT_UPDATE_WINDOW_GEOMETRY "_d_disable_update_geometry_for_scale"
#define HOOK_UPDATE_WINDOW_GEOMETRY_OBJECT "_d_hookUpdateGeometryForScaleObject"
#define UPDATE_WINDOW_GEOMETRY_ENTRY "_d_updateGeometryForScaleEntry"
#define UPDATE_WINDOW_GEOMETRY_GEOMETRY "_d_updateGeometryForScaleGeometry"
#define UPDATE_WINDOW_GEOMETRY_EXIT "_d_updateGeometryForScaleExit"

QT_BEGIN_NAMESPACE

#if XDG_ICON_VERSION_MAR >= 3
class XdgIconProxyEngine : public QIconEngine
{
public:
    XdgIconProxyEngine(XdgIconLoaderEngine *proxy)
        : engine(proxy) {}
    ~XdgIconProxyEngine() {
        delete engine;
    }

    static quint64 entryCacheKey(const ScalableEntry *color_entry, const QIcon::Mode mode, const QIcon::State state)
    {
        return quint64(color_entry) ^ (quint64(mode) << 56) ^ (quint64(state) << 48);
    }

    QPixmap followColorPixmap(ScalableEntry *color_entry, const QSize &size, QIcon::Mode mode, QIcon::State state)
    {
        const QString &color_scheme = DEEPIN_QT_THEME::colorScheme.localData();
        quint64 cache_key = entryCacheKey(color_entry, mode, state);
        const QString &cache_color_scheme = entryToColorScheme.value(cache_key);
        QPixmap pm = color_scheme == cache_color_scheme ? color_entry->svgIcon.pixmap(size, mode, state) : QPixmap();
        // Note: not checking the QIcon::isNull(), because in Qt5.10 the isNull() is not reliable
        // for svg icons desierialized from stream (see https://codereview.qt-project.org/#/c/216086/)
        if (pm.isNull())
        {
            // The following lines are adapted and updated from KDE's "kiconloader.cpp" ->
            // KIconLoaderPrivate::processSvg() and KIconLoaderPrivate::createIconImage().
            // They read the SVG color scheme of SVG icons and give images based on the icon mode.
            QHash<int, QByteArray> svg_buffers;
            QFile device{color_entry->filename};
            if (device.open(QIODevice::ReadOnly))
            {
                // Note: indexes are assembled as in qtsvg (QSvgIconEnginePrivate::hashKey())
                QMap<int, QString> style_sheets;
                style_sheets[(mode<<4)|state] = QStringLiteral(".ColorScheme-Text{color:%1;}").arg(color_scheme);
                QMap<int, QSharedPointer<QXmlStreamWriter> > writers;
                for (auto i = style_sheets.cbegin(); i != style_sheets.cend(); ++i)
                {
                    writers[i.key()].reset(new QXmlStreamWriter{&svg_buffers[i.key()]});
                }

                QXmlStreamReader xmlReader(&device);
                while (!xmlReader.atEnd())
                {
                    if (xmlReader.readNext() == QXmlStreamReader::StartElement
                            && xmlReader.qualifiedName() == QLatin1String("style")
                            && xmlReader.attributes().value(QLatin1String("id")) == QLatin1String("current-color-scheme"))
                    {
                        for (auto i = style_sheets.cbegin(); i != style_sheets.cend(); ++i)
                        {
                            QXmlStreamWriter & writer = *writers[i.key()];
                            writer.writeStartElement(QLatin1String("style"));
                            writer.writeAttributes(xmlReader.attributes());
                            writer.writeCharacters(*i);
                            writer.writeEndElement();
                        }
                        while (xmlReader.tokenType() != QXmlStreamReader::EndElement)
                            xmlReader.readNext();
                    } else if (xmlReader.tokenType() != QXmlStreamReader::Invalid)
                    {
                        for (auto i = style_sheets.cbegin(); i != style_sheets.cend(); ++i)
                        {
                            writers[i.key()]->writeCurrentToken(xmlReader);
                        }
                    }
                }
                // duplicate the contets also for opposite state
//                svg_buffers[(QIcon::Normal<<4)|QIcon::On] = svg_buffers[(QIcon::Normal<<4)|QIcon::Off];
//                svg_buffers[(QIcon::Selected<<4)|QIcon::On] = svg_buffers[(QIcon::Selected<<4)|QIcon::Off];
            }
            // use the QSvgIconEngine
            //  - assemble the content as it is done by the operator <<(QDataStream &s, const QIcon &icon)
            //  (the QSvgIconEngine::key() + QSvgIconEngine::write())
            //  - create the QIcon from the content by usage of the QIcon::operator >>(QDataStream &s, const QIcon &icon)
            //  (icon with the (QSvgIconEngine) will be used)
            QByteArray icon_arr;
            QDataStream str{&icon_arr, QIODevice::WriteOnly};
            str.setVersion(QDataStream::Qt_4_4);
            QHash<int, QString> filenames;
            filenames[0] = color_entry->filename; // Note: filenames are ignored in the QSvgIconEngine::read()
            str << QStringLiteral("svg") << filenames << static_cast<int>(0)/*isCompressed*/ << svg_buffers << static_cast<int>(0)/*hasAddedPimaps*/;

            QDataStream str_read{&icon_arr, QIODevice::ReadOnly};
            str_read.setVersion(QDataStream::Qt_4_4);

            str_read >> color_entry->svgIcon;
            pm = color_entry->svgIcon.pixmap(size, mode, state);

            // load the icon directly from file, if still null
            if (pm.isNull())
            {
                color_entry->svgIcon = QIcon(color_entry->filename);
                pm = color_entry->svgIcon.pixmap(size, mode, state);
            }

            entryToColorScheme[cache_key] = color_scheme;
        }

        return pm;
    }

    QPixmap pixmapByEntry(QIconLoaderEngineEntry *entry, const QSize &size, QIcon::Mode mode, QIcon::State state)
    {
        if (!XdgIcon::followColorScheme()) {
            DEEPIN_QT_THEME::colorScheme.setLocalData(QString());

            return entry->pixmap(size, mode, state);
        }

        QPixmap pixmap;
        char *type_name = abi::__cxa_demangle(typeid(*entry).name(), 0, 0, 0);

        if (type_name == QByteArrayLiteral("ScalableFollowsColorEntry")) {
            if (DEEPIN_QT_THEME::colorScheme.localData().isEmpty()) {
                const QPalette &pal = qApp->palette();
                DEEPIN_QT_THEME::colorScheme.setLocalData(mode == QIcon::Selected ? pal.highlightedText().color().name() : pal.windowText().color().name());
            }

            pixmap = followColorPixmap(static_cast<ScalableEntry*>(entry), size, mode, state);
        } else {
            pixmap = entry->pixmap(size, mode, state);
        }

        free(type_name);
        DEEPIN_QT_THEME::colorScheme.setLocalData(QString());

        return pixmap;
    }
    void paint(QPainter *painter, const QRect &rect, QIcon::Mode mode, QIcon::State state) override {
        if (painter->device()->devType() == QInternal::Widget
                && XdgIcon::followColorScheme()
                && DEEPIN_QT_THEME::colorScheme.localData().isEmpty()) {
            const QPalette &pal = qvariant_cast<QPalette>(dynamic_cast<QObject*>(painter->device())->property("palette"));
            DEEPIN_QT_THEME::colorScheme.setLocalData(mode == QIcon::Selected ? pal.highlightedText().color().name() : pal.windowText().color().name());
        }

        const QPixmap pix = pixmap(rect.size(), mode, state);

        if (pix.isNull())
            return;

        painter->drawPixmap(rect, pix);
    }
    QPixmap pixmap(const QSize &size, QIcon::Mode mode, QIcon::State state) override {
        engine->ensureLoaded();

        QIconLoaderEngineEntry *entry = engine->entryForSize(size);

        if (!entry) {
            DEEPIN_QT_THEME::colorScheme.setLocalData(QString());

            return QPixmap();
        }

        return pixmapByEntry(entry, size, mode, state);
    }
    void addPixmap(const QPixmap &pixmap, QIcon::Mode mode, QIcon::State state) override {
        return engine->addPixmap(pixmap, mode, state);
    }
    void addFile(const QString &fileName, const QSize &size, QIcon::Mode mode, QIcon::State state) override {
        return engine->addFile(fileName, size, mode, state);
    }
    QString key() const override {
        return engine->key();
    }
    QSize actualSize(const QSize &size, QIcon::Mode mode, QIcon::State state) override {
        return engine->actualSize(size, mode, state);
    }
    QIconEngine *clone() const override {
        return new XdgIconProxyEngine(static_cast<XdgIconLoaderEngine*>(engine->clone()));
    }
    bool read(QDataStream &in) override {
        return engine->read(in);
    }
    bool write(QDataStream &out) const override {
        return engine->write(out);
    }
    void virtual_hook(int id, void *data) override {
#if QT_VERSION >= QT_VERSION_CHECK(5, 9, 0)
        if (id == QIconEngine::ScaledPixmapHook)
        {
            engine->ensureLoaded();

            QIconEngine::ScaledPixmapArgument &arg = *reinterpret_cast<QIconEngine::ScaledPixmapArgument*>(data);
            // QIcon::pixmap() multiplies size by the device pixel ratio.
            const int integerScale = qCeil(arg.scale);
            QIconLoaderEngineEntry *entry = engine->entryForSize(arg.size / integerScale, integerScale);
            arg.pixmap = entry ? pixmapByEntry(entry, arg.size, arg.mode, arg.state) : QPixmap();
            DEEPIN_QT_THEME::colorScheme.setLocalData(QString());

            return;
        }
#endif

        return engine->virtual_hook(id, data);
    }

    XdgIconLoaderEngine *engine;
    QHash<quint64, QString> entryToColorScheme;
};
#endif

const char *QDeepinTheme::name = "deepin";
bool QDeepinTheme::m_usePlatformNativeDialog = true;
QMimeDatabase QDeepinTheme::m_mimeDatabase;
DThemeSettings *QDeepinTheme::m_settings = 0;

extern void updateXdgIconSystemTheme();

static void onIconThemeSetCallback()
{
    QIconLoader::instance()->updateSystemTheme();
    updateXdgIconSystemTheme();

    if (qApp->inherits("Dtk::Widget::DApplication")) {
        // emit the signal: DApplication::iconThemeChanged
        qApp->metaObject()->invokeMethod(qApp, QT_STRINGIFY(iconThemeChanged));
    }
}

static void updateWindowGeometry(QWindow *w)
{
    if (w->type() == Qt::ForeignWindow || w->type() == Qt::Desktop) {
        return;
    }

    if (!w->handle() || !w->isTopLevel())
        return;

    if (w->property(DNOT_UPDATE_WINDOW_GEOMETRY).toBool()) {
        QWindowSystemInterfacePrivate::GeometryChangeEvent gce(w, QHighDpi::fromNativePixels(w->handle()->geometry(), w)
                                                   #if QT_VERSION < QT_VERSION_CHECK(5, 10, 0)
                                                               , QRect()
                                                   #endif
                                                               );
        QGuiApplicationPrivate::processGeometryChangeEvent(&gce);
    } else {// 通知窗口大小发送改变
        const QRect currentGeo = QWindowPrivate::get(w)->geometry;

        if (!currentGeo.isValid())
            return;

        // 对外提供能hook自动更新窗口大小的方法
        QObject *hook_obj = qvariant_cast<QObject*>(w->property(HOOK_UPDATE_WINDOW_GEOMETRY_OBJECT));

        if (!hook_obj) {
            hook_obj = w;
        }

        bool accept = true;
        // 通知窗口即将开始更新其geometry
        if (hook_obj->metaObject()->indexOfMethod(UPDATE_WINDOW_GEOMETRY_ENTRY "()") >= 0) {
            QMetaObject::invokeMethod(hook_obj, UPDATE_WINDOW_GEOMETRY_ENTRY,
                                      Qt::DirectConnection, Q_RETURN_ARG(bool, accept));
        }

        if (!accept) {
            // 中断操作
            return;
        }

        QRect nativeGeo;
        // 尝试从窗口对象获取新的geometry
        if (hook_obj->metaObject()->indexOfMethod(UPDATE_WINDOW_GEOMETRY_GEOMETRY "()") >= 0) {
            QMetaObject::invokeMethod(hook_obj, UPDATE_WINDOW_GEOMETRY_GEOMETRY,
                                      Qt::DirectConnection, Q_RETURN_ARG(QRect, nativeGeo));
        }

        if (!nativeGeo.isValid()) {
            nativeGeo = w->handle()->geometry();

            qreal scale = QHighDpiScaling::factor(w);
            const QPoint &cursor_pos = w->screen()->handle()->cursor()->pos();
            // 如果窗口是active的，且鼠标处于窗口内，应当移动窗口位置以保持鼠标相对窗口的位置
            if (w->isActive()) {
                const QMargins &frame_margins = w->handle()->frameMargins();
                const QRect &frame_rect = nativeGeo.marginsAdded(frame_margins);

                if (frame_rect.contains(cursor_pos)) {
                    nativeGeo.moveTopLeft(cursor_pos + (nativeGeo.topLeft() - cursor_pos) * currentGeo.width() * scale / nativeGeo.width());
                }
            }

            nativeGeo.setSize(currentGeo.size() * scale);
        }

        bool positionAutomaticSave = QWindowPrivate::get(w)->positionAutomatic;
        QWindowPrivate::get(w)->positionAutomatic = false;
        w->handle()->setGeometry(nativeGeo);
        QWindowPrivate::get(w)->positionAutomatic = positionAutomaticSave;
        // 请求重绘
        QGuiApplication::sendEvent(w, new QEvent(QEvent::UpdateRequest));
        // 通知窗口geometry更新结束
        if (hook_obj->metaObject()->indexOfMethod(UPDATE_WINDOW_GEOMETRY_EXIT "()") >= 0) {
            QMetaObject::invokeMethod(hook_obj, UPDATE_WINDOW_GEOMETRY_EXIT);
        }
    }
}

static void updateAllWindowGeometry()
{
    for (QWindow *w : qGuiApp->allWindows()) {
        updateWindowGeometry(w);
    }
}

// 延迟一段时间更新窗口geometry，防止屏幕缩放比在短时间内连续变化时导致窗口闪动
static void updateAllWindowGeometryDelay(int interval = 500)
{
    static QTimer *t = new QTimer();

    t->setSingleShot(true);
    t->setInterval(interval);
    t->connect(t, &QTimer::timeout, t, updateAllWindowGeometry, Qt::UniqueConnection);
    t->start();
}

static void notifyScreenScaleUpdated()
{
    for (QScreen *s : qGuiApp->screens()) {
        Q_EMIT s->geometryChanged(s->geometry());

        // 发射信号通知屏幕缩放比发生变化，DApplication中会定义此信号
        if (qGuiApp->metaObject()->indexOfSignal("screenDevicePixelRatioChanged(QScreen*)")) {
            qGuiApp->metaObject()->invokeMethod(qGuiApp, "screenDevicePixelRatioChanged", Q_ARG(QScreen*, s));
        }
    }
}

static bool updateScaleFactor(qreal value)
{
    if (qIsNull(value)) {
        value = 1.0;
    }

    if (qFuzzyCompare(QHighDpiScaling::m_factor, value)) {
        return false;
    }

    QHighDpiScaling::setGlobalFactor(value);

    return true;
}

static void onScaleFactorChanged(qreal value)
{
    if (updateScaleFactor(value)) {
        notifyScreenScaleUpdated();
        updateAllWindowGeometryDelay();
    }
}

// 用于窗口屏幕改变时更新窗口大小
class AutoScaleWindowObject : public QObject
{
    Q_OBJECT
public:
    explicit AutoScaleWindowObject(QObject *parent = nullptr)
        : QObject(parent) {
        qGuiApp->installEventFilter(this);
    }

    void onScreenChanged(QScreen *s)
    {
        Q_UNUSED(s);

        if (QWindow *w = qobject_cast<QWindow*>(sender())) {
            updateWindowGeometry(w);
        }
    }

private:
    bool eventFilter(QObject *watched, QEvent *event) override
    {
        if (event->type() != QEvent::PlatformSurface)
            return false;

        if (QWindow *w = qobject_cast<QWindow*>(watched)) {
            QPlatformSurfaceEvent *se = static_cast<QPlatformSurfaceEvent*>(event);

            if (se->surfaceEventType() == QPlatformSurfaceEvent::SurfaceCreated) {
                // 监听窗口屏幕变化信号，自动根据新的屏幕缩放比更新窗口大小
                connect(w, &QWindow::screenChanged,
                        this, &AutoScaleWindowObject::onScreenChanged,
                        Qt::DirectConnection);
            } else {
                disconnect(w, &QWindow::screenChanged,
                           this, &AutoScaleWindowObject::onScreenChanged);
            }
        }

        return false;
    }
};

// 判断ScreenScaleFactor的值是否应该应用于所有屏幕
static bool isGenericScreenScaleFactors(const QByteArray &value)
{
    for (char ch : value) {
        if (ch == '=' || ch == ';')
            return false;
    }

    return true;
}

static void onAutoScaleWindowChanged()
{
    bool on = QDeepinTheme::getSettings()->autoScaleWindow();

    if (on) {
        const QByteArray &multi_scale = QDeepinTheme::getSettings()->screenScaleFactors();

        // 只在针对多个屏幕分别设置了缩放比时开启此功能
        if (isGenericScreenScaleFactors(multi_scale)) {
            on = false;
        }
    }

    static AutoScaleWindowObject *event_fileter = nullptr;

    if (on) {
        if (event_fileter)
            return;

        event_fileter = new AutoScaleWindowObject(qGuiApp);
    } else {
        if (!event_fileter)
            return;

        event_fileter->deleteLater();
        event_fileter = nullptr;
    }
}

// 使用 QT_SCREEN_SCALE_FACTORS 为每个屏幕设置不同的缩放比之后，Qt会自动将 dpi 除以主屏的
// 缩放倍数，以此来避免字体被放大。font dpi会影响未设置pixel size的QFont，默认情况下，
// QGuiApplication::font() 不会设置pixel size，因此，使用分屏幕设置不同缩放比后，字体却还
// 是缩放前的大小。
// 此处，如果设置了 ScreenScaleFactors，但未指定 ScaleLogcailDpi 时，默认将其重设回主屏
// 的 logicalDpi。
static bool updateScaleLogcailDpi(const QPair<qreal, qreal> &dpi)
{
    bool ok = dpi.first >= 0 && dpi.second >= 0;

    if (dpi.first > 0) {
        QHighDpiScaling::m_logicalDpi.first = dpi.first;
    } else if (qIsNull(dpi.first)) {
        QHighDpiScaling::m_logicalDpi.first = qGuiApp->primaryScreen()->handle()->logicalDpi().first;
    }

    if (dpi.second > 0) {
        QHighDpiScaling::m_logicalDpi.second = dpi.second;
    } else if (qIsNull(dpi.second)) {
        QHighDpiScaling::m_logicalDpi.second = qGuiApp->primaryScreen()->handle()->logicalDpi().second;
    }

    return ok;
}

static QByteArray getEnvValueByScreenScaleFactors(const QByteArray &data)
{
    QByteArray envValue;

    if (!isGenericScreenScaleFactors(data)) {
        envValue = data;
    } else if (!data.isEmpty()) {
        // 这种情况下，在Qt的实现中，只会将值应用给第一个屏幕，因此此处需要给所有屏幕都设置通用的值
        int screen_count = qApp->screens().count();

        // 为除了最后一个屏幕的其它屏幕添加缩放比
        while (--screen_count) {
            envValue.append(data).append(';');
        }

        // 为最后一个屏幕添加缩放比
        envValue.append(data);
    }

    return envValue;
}

static bool updateScreenScaleFactors(DThemeSettings *s, const QByteArray &value, bool unsetenv = false)
{
    const QByteArray &envValue = getEnvValueByScreenScaleFactors(value);

    if (qgetenv("QT_SCREEN_SCALE_FACTORS") == envValue)
        return false;

    if (envValue.isEmpty()) {
        if (!unsetenv)
            return false;

        qunsetenv("QT_SCREEN_SCALE_FACTORS");
    } else {
        qputenv("QT_SCREEN_SCALE_FACTORS", envValue);
    }

    QHighDpiScaling::updateHighDpiScaling();
    qDebug() << QHighDpiScaling::logicalDpi();
    updateScaleLogcailDpi(s->scaleLogicalDpi());

    qDebug() << QHighDpiScaling::logicalDpi();

    return true;
}
static void onScreenScaleFactorsChanged(const QByteArray &value)
{
    if (updateScreenScaleFactors(QDeepinTheme::getSettings(), value, true)) {
        notifyScreenScaleUpdated();
        updateAllWindowGeometryDelay();
    }
}

static bool enabledRTScreenScale()
{
    // 应用中设置了和屏幕缩放相关的环境变量或启动相关属性后后不开启自动缩放功能
    static bool enable = !qEnvironmentVariableIsSet("D_DISABLE_RT_SCREEN_SCALE") &&
                            !qEnvironmentVariableIsSet("QT_DEVICE_PIXEL_RATIO") &&
                            !qEnvironmentVariableIsSet("QT_SCALE_FACTOR") &&
                            !qEnvironmentVariableIsSet("QT_AUTO_SCREEN_SCALE_FACTOR") &&
                            !qEnvironmentVariableIsSet("QT_SCREEN_SCALE_FACTORS") &&
                            !QCoreApplication::testAttribute(Qt::AA_DisableHighDpiScaling) &&
                            !QCoreApplication::testAttribute(Qt::AA_EnableHighDpiScaling);

    return enable;
}

QDeepinTheme::QDeepinTheme()
{
#if XDG_ICON_VERSION_MAR >= 3
    // 注意!!!, 此处还没有开始启动事件循环, 很多Qt类无法使用, 例如QTimer
    DEEPIN_QT_THEME::setFollowColorScheme = XdgIcon::setFollowColorScheme;
    DEEPIN_QT_THEME::followColorScheme = XdgIcon::followColorScheme;
#endif

    if (enabledRTScreenScale()) {
        QScopedPointer<DThemeSettings> setting(new DThemeSettings(false));
        // 程序启动时初始设置屏幕缩放比
        updateScaleFactor(setting->scaleFactor());

        if (!updateScreenScaleFactors(setting.data(), setting->screenScaleFactors())) {
            updateScaleLogcailDpi(setting->scaleLogicalDpi());
        }
    }
}

QDeepinTheme::~QDeepinTheme()
{
    if (QDeepinFileDialogHelper::manager) {
        QDeepinFileDialogHelper::manager->deleteLater();
        QDeepinFileDialogHelper::manager = Q_NULLPTR;
    }
}

bool QDeepinTheme::usePlatformNativeDialog(DialogType type) const
{
    if (type == FileDialog) {
        if (qgetenv("_d_disableDBusFileDialog") == "true")
            return false;

        static bool dbusDialogManagerInitialized = false;

        if (!dbusDialogManagerInitialized) {
            dbusDialogManagerInitialized = true;
            QDeepinFileDialogHelper::initDBusFileDialogManager();
        }

        return m_usePlatformNativeDialog
                && QDeepinFileDialogHelper::manager
                && QDeepinFileDialogHelper::manager->isUseFileChooserDialog();
    }

    return QGenericUnixTheme::usePlatformNativeDialog(type);
}

QPlatformDialogHelper *QDeepinTheme::createPlatformDialogHelper(DialogType type) const
{
    if (type == FileDialog && usePlatformNativeDialog(type))
        return new QDeepinFileDialogHelper();

    return QGenericUnixTheme::createPlatformDialogHelper(type);
}

QIconEngine *QDeepinTheme::createIconEngine(const QString &iconName) const
{
#ifdef DTHEMED_ICON_LOOKUP
    if (iconName.contains("/"))
        // handle Qt-standard paths.
        return QGenericUnixTheme::createIconEngine(iconName);
    else
        return new DIconEngine(iconName);
#elif XDG_ICON_VERSION_MAR < 3
    return XdgIconEngineCreator::create(iconName);
#else
    return new XdgIconProxyEngine(new XdgIconLoaderEngine(iconName));
#endif
}

QPixmap QDeepinTheme::standardPixmap(QPlatformTheme::StandardPixmap sp, const QSizeF &size) const
{
    switch (sp) {
    case ArrowRight:
        return QPixmap(":/images/arrow-light.png");
    default: break;
    }

    return QGenericUnixTheme::standardPixmap(sp, size);
}

#if QT_VERSION >= QT_VERSION_CHECK(5, 8, 0)
QIcon QDeepinTheme::fileIcon(const QFileInfo &fileInfo, QPlatformTheme::IconOptions iconOptions) const
{
    Q_UNUSED(iconOptions);

    return XdgIcon::fromTheme(m_mimeDatabase.mimeTypeForFile(fileInfo).iconName());
}
#else
QPixmap QDeepinTheme::fileIconPixmap(const QFileInfo &fileInfo, const QSizeF &size, QPlatformTheme::IconOptions iconOptions) const
{
    Q_UNUSED(iconOptions);

    return XdgIcon::fromTheme(m_mimeDatabase.mimeTypeForFile(fileInfo).iconName()).pixmap(size.toSize());
}
#endif

QVariant QDeepinTheme::themeHint(QPlatformTheme::ThemeHint hint) const
{
    switch (hint) {
    case QPlatformTheme::StyleNames: {
        if (settings()->isSetStyleNames() && !settings()->styleNames().isEmpty())
            return settings()->styleNames();

        break;
    }
    case QPlatformTheme::SystemIconThemeName:
        if (settings()->isSetIconThemeName())
            return settings()->iconThemeName();

        return QVariant();
    case QPlatformTheme::SystemIconFallbackThemeName:
        if (settings()->isSetFallbackIconThemeName())
            return settings()->fallbackIconThemeName();

        return QVariant("deepin");
    case QPlatformTheme::IconThemeSearchPaths:
        return QVariant(QGenericUnixTheme::xdgIconThemePaths() << QDir::homePath() + "/.local/share/icons");
    case UseFullScreenForPopupMenu:
        return true;
    default:
        break;
    }

    return QGenericUnixTheme::themeHint(hint);
}

const QFont *QDeepinTheme::font(QPlatformTheme::Font type) const
{
    if (!qApp->desktopSettingsAware())
        return QGenericUnixTheme::font(type);

    switch (type) {
    case SystemFont:
        if (settings()->isSetSystemFont()) {
            static QFont *system_font = new QFont("");

            if (!settings()->systemFont().isEmpty()) {
                system_font->setFamily(settings()->systemFont());
                system_font->setPointSizeF(settings()->systemFontPointSize());
            }

            return system_font;
        }
        break;
    case FixedFont:
        if (settings()->isSetSystemFixedFont()) {
            static QFont *fixed_font = new QFont("");

            if (!settings()->systemFixedFont().isEmpty()) {
                fixed_font->setFamily(settings()->systemFixedFont());
                fixed_font->setPointSizeF(settings()->systemFontPointSize());
            }

            return fixed_font;
        }
        break;
    default:
        break;
    }

    return QGenericUnixTheme::font(type);
}

static void compelledUpdateScaleLogcailDpi() {
    updateScaleLogcailDpi(QDeepinTheme::getSettings()->scaleLogicalDpi());
}

static void onScreenAdded(QScreen *s) {
    if (QHighDpiScaling::m_screenFactorSet) {
        auto setting = QDeepinTheme::getSettings();
        auto value = setting->screenScaleFactors();

        if (!value.isEmpty() && isGenericScreenScaleFactors(value)) {
            const QByteArray &envValue = getEnvValueByScreenScaleFactors(value);

            qputenv("QT_SCREEN_SCALE_FACTORS", envValue);
            bool ok = false;
            qreal scale = value.toDouble(&ok);

            // 为新的屏幕设置缩放比
            if (ok)
                QHighDpiScaling::setScreenFactor(s, scale);
        }
    }

    compelledUpdateScaleLogcailDpi();
}

DThemeSettings *QDeepinTheme::settings() const
{
    if (!m_settings) {
        m_settings = new DThemeSettings();

        qApp->setProperty("_d_theme_settings_object", (quintptr)m_settings);

        if (qApp->inherits("Dtk::Widget::DApplication")) {
            QObject::connect(m_settings, SIGNAL(iconThemeNameChanged(QString)),
                             qApp, SLOT(iconThemeChanged()), Qt::UniqueConnection);
        }

        auto updateSystemFont = [this] {
            qApp->setFont(*font(QPlatformTheme::SystemFont));
        };

        QObject::connect(m_settings, &DThemeSettings::systemFontChanged, m_settings, updateSystemFont, Qt::UniqueConnection);
        QObject::connect(m_settings, &DThemeSettings::systemFontPointSizeChanged, m_settings, updateSystemFont, Qt::UniqueConnection);
        QObject::connect(m_settings, &DThemeSettings::iconThemeNameChanged, m_settings, &onIconThemeSetCallback, Qt::UniqueConnection);

        if (enabledRTScreenScale()) {
#ifdef QT_NO_DEBUG
            if (!qEnvironmentVariableIsSet("D_ENABLE_RT_SCALE"))
                return m_settings;
#endif

            QObject::connect(m_settings, &DThemeSettings::scaleFactorChanged,
                             m_settings, onScaleFactorChanged, Qt::UniqueConnection);
            QObject::connect(m_settings, &DThemeSettings::screenScaleFactorsChanged,
                             m_settings, onScreenScaleFactorsChanged, Qt::UniqueConnection);
            QObject::connect(m_settings, &DThemeSettings::scaleLogicalDpiChanged,
                             m_settings, updateScaleLogcailDpi, Qt::UniqueConnection);

            // 当屏幕connected时，Qt库中会更新缩放值，导致dpi的设置被覆盖，此处应该再重新设置dpi值。
            // TODO(zccrs): 当最后一个屏幕disconnectd时，Qt不会移除它，在这种状态下，插入一个新的
            //              屏幕时，Qt只会更新遗留的screen对象的信息，因此不会有screenAdded信号，
            //              而且同样会更新dpi的值，因为目前无法通过Qt api检测到此状态，所以这种情
            //              况下无法自动更新dpi值，将导致应用字体显示大小不受缩放比控制。
            //              !!!
            //              针对 dtk 应用，将在dxcb插件中通知应用更新dpi值。
            qApp->setProperty("_d_updateScaleLogcailDpi", (quintptr)&compelledUpdateScaleLogcailDpi);
            QObject::connect(qApp, &QGuiApplication::screenAdded,
                             m_settings, onScreenAdded,
                             Qt::ConnectionType(Qt::QueuedConnection | Qt::UniqueConnection));

            if (!qEnvironmentVariableIsSet(DISABLE_UPDATE_WINDOW_GEOMETRY)) {
                QObject::connect(m_settings, &DThemeSettings::autoScaleWindowChanged,
                                 m_settings, onAutoScaleWindowChanged, Qt::UniqueConnection);
                QObject::connect(m_settings, &DThemeSettings::screenScaleFactorsChanged,
                                 m_settings, onAutoScaleWindowChanged, Qt::UniqueConnection);

                onAutoScaleWindowChanged();
            }
        }
    }

    return m_settings;
}

DThemeSettings *QDeepinTheme::getSettings()
{
    return m_settings;
}

QT_END_NAMESPACE

#include "qdeepintheme.moc"
