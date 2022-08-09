/*
 * SPDX-FileCopyrightText: 2021 - 2022 UnionTech Software Technology Co., Ltd.  
 * SPDX-License-Identifier: GPL-3.0-or-later
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
