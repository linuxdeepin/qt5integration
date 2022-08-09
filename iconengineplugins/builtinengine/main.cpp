/*
 * SPDX-FileCopyrightText: 2019 - 2022 UnionTech Software Technology Co., Ltd.  
 * SPDX-License-Identifier: GPL-3.0-or-later
 */
#include <qiconengineplugin.h>
#include <qstringlist.h>

#include "dbuiltiniconengine.h"

#include <qiodevice.h>
#include <qbytearray.h>
#include <qdebug.h>

QT_BEGIN_NAMESPACE

class DBuiltinIconEnginePlugin : public QIconEnginePlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QIconEngineFactoryInterface" FILE "builtinengine.json")

public:
    QStringList keys() const;
    QIconEngine *create(const QString &iconName = QString());
};

QStringList DBuiltinIconEnginePlugin::keys() const
{
    return {"DBuiltinIconEngine"};
}

QIconEngine *DBuiltinIconEnginePlugin::create(const QString &iconName)
{
    DBuiltinIconEngine *engine = new DBuiltinIconEngine(iconName);

    return engine;
}

QT_END_NAMESPACE

#include "main.moc"
