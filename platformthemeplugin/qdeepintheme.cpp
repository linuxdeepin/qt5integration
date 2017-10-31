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

#include "qdeepintheme.h"
#include "qdeepinfiledialoghelper.h"
#include "diconproxyengine.h"
#include "filedialogmanager_interface.h"
#include "dthemesettings.h"

#include <QVariant>
#include <QDebug>
#include <QGuiApplication>

#include <XdgIcon>

#include <private/qicon_p.h>
#include <private/qiconloader_p.h>

#undef signals
#include <gtk/gtk.h>

#include <X11/Xlib.h>

QT_BEGIN_NAMESPACE

const char *QDeepinTheme::name = "deepin";
bool QDeepinTheme::m_usePlatformNativeDialog = true;
QMimeDatabase QDeepinTheme::m_mimeDatabase;
DThemeSettings *QDeepinTheme::m_settings = 0;

static QString gtkSetting(const gchar *propertyName)
{
    GtkSettings *settings = gtk_settings_get_default();
    gchararray value;
    g_object_get(settings, propertyName, &value, NULL);
    QString str = QString::fromUtf8(value);
    g_free(value);
    return str;
}

extern void updateXdgIconSystemTheme();

static void gtkIconThemeSetCallback()
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
    // gtk_init will reset the Xlib error handler, and that causes
    // Qt applications to quit on X errors. Therefore, we need to manually restore it.
    int (*oldErrorHandler)(Display *, XErrorEvent *) = XSetErrorHandler(NULL);

    gtk_init(0, 0);

    XSetErrorHandler(oldErrorHandler);

    static GtkSettings *settings = Q_NULLPTR;

    if (!settings) {
        settings = gtk_settings_get_default();

        if (settings) {
            g_signal_connect(settings, "notify::gtk-icon-theme-name", G_CALLBACK(gtkIconThemeSetCallback), 0);
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
    QIcon icon = XdgIcon::fromTheme(iconName);

    if (icon.availableSizes().isEmpty())
        return QGenericUnixTheme::createIconEngine(iconName);

    return icon.data_ptr()->engine->clone();
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

        QStringList styleNames;
        // TODO(hualet): Make ddark&dlight styles ready!
        // styleNames << QStringLiteral("dlight");
#ifndef QT_NO_STYLE_GTK
        styleNames << QStringLiteral("gtk2");
        styleNames << QStringLiteral("GTK+");
#endif
        styleNames << QStringLiteral("fusion");
        return QVariant(styleNames);
    }
    case QPlatformTheme::SystemIconThemeName:
        if (settings()->isSetIconThemeName())
            return settings()->iconThemeName();

        return QVariant(gtkSetting("gtk-icon-theme-name"));
    case QPlatformTheme::SystemIconFallbackThemeName:
        if (settings()->isSetFallbackIconThemeName())
            return settings()->fallbackIconThemeName();

        return QVariant(gtkSetting("gtk-fallback-icon-theme"));
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
    switch (type) {
    case SystemFont:
        if (qApp->desktopSettingsAware() && settings()->isSetSystemFont()) {
            static QFont *system_font = new QFont("");

            if (!settings()->systemFont().isEmpty()) {
                system_font->setFamily(settings()->systemFont());
                system_font->setPointSizeF(settings()->systemFontPointSize());
            }

            return system_font;
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
    }

    return m_settings;
}

QT_END_NAMESPACE
