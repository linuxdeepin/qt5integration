/*
 * SPDX-FileCopyrightText: 2019 - 2022 UnionTech Software Technology Co., Ltd.  
 * SPDX-License-Identifier: LGPL-3.0-or-later
 */
#include <qiconengineplugin.h>
#include <qstringlist.h>
#include <qiodevice.h>
#include <qbytearray.h>
#include <qdebug.h>

#include <DIconTheme>

QT_BEGIN_NAMESPACE
DGUI_USE_NAMESPACE
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
    QIconEngine *engine = DIconTheme::createIconEngine(iconName);
    return engine;
}

QT_END_NAMESPACE

#include "main.moc"
