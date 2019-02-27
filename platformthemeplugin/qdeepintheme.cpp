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
#if XDG_ICON_VERSION_MAR >= 2
#define private public
#include <private/xdgiconloader/xdgiconloader_p.h>
#undef private
#endif

#include <private/qicon_p.h>
#include <private/qiconloader_p.h>

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

QDeepinTheme::QDeepinTheme()
{
#if XDG_ICON_VERSION_MAR >= 3
    // 注意!!!, 此处还没有开始启动事件循环, 很多Qt类无法使用, 例如QTimer
    DEEPIN_QT_THEME::setFollowColorScheme = XdgIcon::setFollowColorScheme;
    DEEPIN_QT_THEME::followColorScheme = XdgIcon::followColorScheme;
#endif
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
    return new XdgIconLoaderEngine(iconName);
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
    }

    return m_settings;
}

QT_END_NAMESPACE
