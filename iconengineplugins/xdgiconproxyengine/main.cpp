/*
 * SPDX-FileCopyrightText: 2021 - 2022 UnionTech Software Technology Co., Ltd.  
 * SPDX-License-Identifier: LGPL-3.0-or-later
 */
#include <qiconengineplugin.h>
#include <qstringlist.h>

#if XDG_ICON_VERSION_MAR >= 3
#include "xdgiconproxyengine.h"
#else
//这个版本中的xdgiconloader_p.h定义和qiconloader_p.h有冲突
//只能通过此方式创建XdgIconLoaderEngine对象
#include <private/xdgiconloader/xdgiconloader_p.h>
#endif

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
#if XDG_ICON_VERSION_MAR >=3
    return new XdgIconProxyEngine(new XdgIconLoaderEngine(iconName));
#else
    return new XdgIconLoaderEngine(iconName);
#endif
}

QT_END_NAMESPACE

#include "main.moc"
