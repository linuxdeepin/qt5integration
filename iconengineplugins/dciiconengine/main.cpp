// SPDX-FileCopyrightText: 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#include <qiconengineplugin.h>
#include <qstringlist.h>

#include "dciiconengine.h"

#include <qiodevice.h>
#include <qbytearray.h>
#include <qdebug.h>

QT_BEGIN_NAMESPACE

class DDciIconEnginePlugin : public QIconEnginePlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QIconEngineFactoryInterface" FILE "dciiconengine.json")

public:
    QStringList keys() const;
    QIconEngine *create(const QString &iconName = QString());
};

QStringList DDciIconEnginePlugin::keys() const
{
    return {"DDciIconEngine"};
}

QIconEngine *DDciIconEnginePlugin::create(const QString &iconName)
{
    DDciIconEngine *engine = new DDciIconEngine(iconName);

    return engine;
}

QT_END_NAMESPACE

#include "main.moc"
