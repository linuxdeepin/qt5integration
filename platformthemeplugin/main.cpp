/*
 * SPDX-FileCopyrightText: 2017 - 2022 UnionTech Software Technology Co., Ltd.  
 * SPDX-License-Identifier: LGPL-3.0-or-later
 */
#include <qpa/qplatformthemeplugin.h>
#include "qdeepintheme.h"

//#include <private/xdgiconloader/xdgiconloader_p.h>

#include <QDBusConnection>
#include <QDBusMessage>

QT_BEGIN_NAMESPACE

void updateXdgIconSystemTheme()
{
//    XdgIconLoader::instance()->updateSystemTheme();
    QDBusMessage message = QDBusMessage::createSignal(QStringLiteral("/KIconLoader"), QStringLiteral("org.kde.KIconLoader"), QStringLiteral("iconChanged"));
    message << 0;
    QDBusConnection::sessionBus().send(message);
}

class QDeepinThemePlugin : public QPlatformThemePlugin
{
   Q_OBJECT
   Q_PLUGIN_METADATA(IID QPlatformThemeFactoryInterface_iid FILE "deepin.json")

public:
    QPlatformTheme *create(const QString &key, const QStringList &params) Q_DECL_OVERRIDE;
};

QPlatformTheme *QDeepinThemePlugin::create(const QString &key, const QStringList &params)
{
    Q_UNUSED(params);
    if (!key.compare(QLatin1String(QDeepinTheme::name), Qt::CaseInsensitive))
        return new QDeepinTheme;

    return 0;
}

QT_END_NAMESPACE

#include "main.moc"
