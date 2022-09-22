/*
 * SPDX-FileCopyrightText: 2021 - 2022 UnionTech Software Technology Co., Ltd.  
 * SPDX-License-Identifier: LGPL-3.0-or-later
 */
#include <QStylePlugin>
#include "dpluginloader.h"
class ChameleonStylePlugin : public QStylePlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QStyleFactoryInterface" FILE "chameleon.json")

public:
    ChameleonStylePlugin(QObject *parent = nullptr)
        : QStylePlugin(parent)
    {
        m_pluginProxy.reset(DPluginLoader::load<QStylePlugin>(TARGET));
    }

    QStyle *create(const QString &key) override
    {
        return m_pluginProxy ? m_pluginProxy->create(key) : nullptr;
    }

private:
    QScopedPointer<QStylePlugin> m_pluginProxy;
};

#include "main.moc"
