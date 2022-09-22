/*
 * SPDX-FileCopyrightText: 2021 - 2022 UnionTech Software Technology Co., Ltd.  
 * SPDX-License-Identifier: LGPL-3.0-or-later
 */
#include <qiconengineplugin.h>
#include "dpluginloader.h"
QT_BEGIN_NAMESPACE

class DBuiltinIconEnginePlugin : public QIconEnginePlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QIconEngineFactoryInterface" FILE "builtinengine.json")

public:
    DBuiltinIconEnginePlugin(QObject *parent = nullptr);
    QIconEngine *create(const QString &iconName = QString());

private:
    QScopedPointer<QIconEnginePlugin> m_pluginProxy;
};

DBuiltinIconEnginePlugin::DBuiltinIconEnginePlugin(QObject *parent)
    : QIconEnginePlugin(parent)
{
    m_pluginProxy.reset(DPluginLoader::load<QIconEnginePlugin>(TARGET));
}

QIconEngine *DBuiltinIconEnginePlugin::create(const QString &iconName)
{
    return m_pluginProxy ? m_pluginProxy->create(iconName) : nullptr;
}

QT_END_NAMESPACE

#include "main.moc"
