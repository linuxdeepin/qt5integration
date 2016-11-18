#include "qdeepintheme.h"
#include "qdeepinfiledialoghelper.h"
#include "qdeepinplatformmenu.h"
#include "qdeepinplatformmenuitem.h"
#include "diconproxyengine.h"

#include <dfmglobal.h>

#include <QVariant>
#include <QDebug>

#include <XdgIcon>

#include <private/qicon_p.h>

#undef signals
#include <gtk/gtk.h>

#include <X11/Xlib.h>

QT_BEGIN_NAMESPACE

const char *QDeepinTheme::name = "deepin";
bool QDeepinTheme::m_usePlatformNativeDialog = true;
bool QDeepinTheme::m_usePlatformNativeMenu = true;

static QString gtkSetting(const gchar *propertyName)
{
    GtkSettings *settings = gtk_settings_get_default();
    gchararray value;
    g_object_get(settings, propertyName, &value, NULL);
    QString str = QString::fromUtf8(value);
    g_free(value);
    return str;
}

QDeepinTheme::QDeepinTheme()
{
    DFMGlobal::installTranslator();

    // gtk_init will reset the Xlib error handler, and that causes
    // Qt applications to quit on X errors. Therefore, we need to manually restore it.
    int (*oldErrorHandler)(Display *, XErrorEvent *) = XSetErrorHandler(NULL);

    gtk_init(0, 0);

    XSetErrorHandler(oldErrorHandler);
}

//QPlatformMenuItem *QDeepinTheme::createPlatformMenuItem() const
//{
//    if (!m_usePlatformNativeMenu)
//        return 0;

//    return new QDeepinPlatformMenuItem;
//}

//QPlatformMenu *QDeepinTheme::createPlatformMenu() const
//{
//    if (!m_usePlatformNativeMenu)
//        return 0;

//    return new QDeepinPlatformMenu;
//}

bool QDeepinTheme::usePlatformNativeDialog(DialogType type) const
{
    if (type == FileDialog)
        return m_usePlatformNativeDialog;

    return QGenericUnixTheme::usePlatformNativeDialog(type);
}

QPlatformDialogHelper *QDeepinTheme::createPlatformDialogHelper(DialogType type) const
{
    if (type == FileDialog)
        return m_usePlatformNativeDialog ? new QDeepinFileDialogHelper() : Q_NULLPTR;

    return QGenericUnixTheme::createPlatformDialogHelper(type);
}

QIconEngine *QDeepinTheme::createIconEngine(const QString &iconName) const
{
    QIcon icon = XdgIcon::fromTheme(iconName);

    if (icon.availableSizes().isEmpty())
        return 0;

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

QVariant QDeepinTheme::themeHint(QPlatformTheme::ThemeHint hint) const
{
    switch (hint) {
    case QPlatformTheme::StyleNames: {
        QStringList styleNames;
        // TODO(hualet): Make ddark&dlight styles ready!
        // styleNames << QStringLiteral("dlight");
#ifndef QT_NO_STYLE_GTK
        styleNames << QStringLiteral("GTK+");
#endif
        styleNames << QStringLiteral("fusion");
        return QVariant(styleNames);
    }
    case QPlatformTheme::SystemIconThemeName:
        return QVariant(gtkSetting("gtk-icon-theme-name"));
    case QPlatformTheme::SystemIconFallbackThemeName:
        return QVariant(gtkSetting("gtk-fallback-icon-theme"));
    default:
        break;
    }

    return QGenericUnixTheme::themeHint(hint);
}

QT_END_NAMESPACE
