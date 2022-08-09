/*
 * SPDX-FileCopyrightText: 2021 - 2022 UnionTech Software Technology Co., Ltd.  
 * SPDX-License-Identifier: GPL-3.0-or-later
 */
#include <qiconengineplugin.h>
#include "dpluginloader.h"
QT_BEGIN_NAMESPACE

class XdgProxyIconPlugin : public QIconEnginePlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QIconEngineFactoryInterface" FILE "xdgiconproxyengine.json")

public:
    XdgProxyIconPlugin(QObject *parent = nullptr);
    QIconEngine *create(const QString &iconName = QString());

private:
    QScopedPointer<QIconEnginePlugin> m_pluginProxy;
};

XdgProxyIconPlugin::XdgProxyIconPlugin(QObject *parent)
    : QIconEnginePlugin(parent)
{
    m_pluginProxy.reset(DPluginLoader::load<QIconEnginePlugin>(TARGET));
}

QIconEngine *XdgProxyIconPlugin::create(const QString &iconName)
{
    return m_pluginProxy ? m_pluginProxy->create(iconName) : nullptr;
}

QT_END_NAMESPACE

#include "main.moc"
