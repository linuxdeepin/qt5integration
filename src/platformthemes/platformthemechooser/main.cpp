/*
 * Copyright (C) 2021 ~ 2022 Deepin Technology Co., Ltd.
 *
 * Author:     ck <chenke@uniontech.com>
 *
 * Maintainer: ck <chenke@uniontech.com>
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

#include <qpa/qplatformthemeplugin.h>
#include "dpluginloader.h"

class QDeepinThemePlugin : public QPlatformThemePlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID QPlatformThemeFactoryInterface_iid FILE "deepin.json")

public:
    QDeepinThemePlugin(QObject *parent = nullptr);
    QPlatformTheme *create(const QString &key, const QStringList &params) Q_DECL_OVERRIDE;

private:
    QScopedPointer<QPlatformThemePlugin> m_pluginProxy;
};

QDeepinThemePlugin::QDeepinThemePlugin(QObject *parent)
    : QPlatformThemePlugin(parent)
{
    m_pluginProxy.reset(DPluginLoader::load<QPlatformThemePlugin>(TARGET));
}

QPlatformTheme *QDeepinThemePlugin::create(const QString &key, const QStringList &params)
{
    return m_pluginProxy ? m_pluginProxy->create(key, params) : nullptr;
}

QT_END_NAMESPACE

#include "main.moc"
