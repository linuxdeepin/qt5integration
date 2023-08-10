/*
 * SPDX-FileCopyrightText: 2017 - 2023 UnionTech Software Technology Co., Ltd.
 * SPDX-License-Identifier: LGPL-3.0-or-later
 */
#include <qpa/qplatformthemeplugin.h>
#include "qdeepintheme.h"

QT_BEGIN_NAMESPACE
class QDeepinFileDialogThemePlugin : public QPlatformThemePlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID QPlatformThemeFactoryInterface_iid FILE "deepinfiledialog.json")

public:
    QPlatformTheme *create(const QString &key, const QStringList &params) Q_DECL_OVERRIDE;
};

QPlatformTheme *QDeepinFileDialogThemePlugin::create(const QString &key, const QStringList &params)
{
    Q_UNUSED(params);
    const QStringList &keys = {"deepinfiledialog", QLatin1String(QDeepinTheme::name), QLatin1String("DDE")};
    if (keys.contains(key, Qt::CaseInsensitive))
        return new QDeepinTheme;

    return nullptr;
}

QT_END_NAMESPACE

#include "main.moc"
