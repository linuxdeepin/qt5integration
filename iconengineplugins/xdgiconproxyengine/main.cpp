/*
 * SPDX-FileCopyrightText: 2021 - 2022 UnionTech Software Technology Co., Ltd.  
 * SPDX-License-Identifier: LGPL-3.0-or-later
 */
#include <qiconengineplugin.h>
#include <qstringlist.h>

#include "xdgiconproxyengine.h"

QT_BEGIN_NAMESPACE

class XdgProxyIconPlugin : public QIconEnginePlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QIconEngineFactoryInterface" FILE "xdgproxyiconplugin.json")

public:
    QStringList keys() const;
    QIconEngine *create(const QString &filename = QString());
};

QStringList XdgProxyIconPlugin::keys() const
{
    return {"XdgIconProxyEngine"};
}

QIconEngine *XdgProxyIconPlugin::create(const QString &iconName)
{
    return new XdgIconProxyEngine(new XdgIconLoaderEngine(iconName));
}

QT_END_NAMESPACE

#include "main.moc"
