/**
 * Copyright (C) 2017 by Deepin Technology Co., Ltd.
 *
 * Use of this source code is governed by GPLv3
 * that can be found in the LICENSE file.
 **/

#include <qpa/qplatformthemeplugin.h>
#include "qdeepintheme.h"

#include <qt5xdgiconloader/2.0.0/private/xdgiconloader/xdgiconloader_p.h>

QT_BEGIN_NAMESPACE

void updateXdgIconSystemTheme()
{
    XdgIconLoader::instance()->updateSystemTheme();
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
