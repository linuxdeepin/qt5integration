/*
 * Copyright (C) 2019 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     zccrs <zccrs@live.com>
 *
 * Maintainer: zccrs <zhangjide@deepin.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
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
