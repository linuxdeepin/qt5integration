/*
 * SPDX-FileCopyrightText: 2023 UnionTech Software Technology Co., Ltd.
 * SPDX-License-Identifier: LGPL-3.0-or-later
 */
#include <qiconengineplugin.h>
#include <qstringlist.h>

#include "diconproxyengine.h"

QT_BEGIN_NAMESPACE

class DIconProxyEnginePlugin : public QIconEnginePlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QIconEngineFactoryInterface" FILE "diconproxyengine.json")

public:
    QStringList keys() const;
    QIconEngine *create(const QString &iconName = QString());
};

QStringList DIconProxyEnginePlugin::keys() const
{
    return {"DIconProxyEngine"};
}

QIconEngine *DIconProxyEnginePlugin::create(const QString &iconName)
{
    return new DIconProxyEngine(iconName);
}

QT_END_NAMESPACE

#include "main.moc"
