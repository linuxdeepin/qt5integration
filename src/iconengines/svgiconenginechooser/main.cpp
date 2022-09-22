/*
 * SPDX-FileCopyrightText: 2021 - 2022 UnionTech Software Technology Co., Ltd.  
 * SPDX-License-Identifier: LGPL-3.0-or-later
 */
#include <qiconengineplugin.h>
#include "dpluginloader.h"

QT_BEGIN_NAMESPACE

class QSvgIconPlugin : public QIconEnginePlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QIconEngineFactoryInterface" FILE "qsvgiconengine.json")

public:
    QSvgIconPlugin(QObject *parent = nullptr);
    QIconEngine *create(const QString &filename = QString());

private:
    QScopedPointer<QIconEnginePlugin> m_pluginProxy;
};

QSvgIconPlugin::QSvgIconPlugin(QObject *parent)
    : QIconEnginePlugin(parent)
{
    m_pluginProxy.reset(DPluginLoader::load<QIconEnginePlugin>(TARGET));
}

QIconEngine *QSvgIconPlugin::create(const QString &file)
{
    return m_pluginProxy ? m_pluginProxy->create(file) : nullptr;
}

QT_END_NAMESPACE

#include "main.moc"
