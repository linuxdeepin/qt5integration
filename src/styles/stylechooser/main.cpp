/*
 * Copyright (C) 2021 ~ 2022 Deepin Technology Co., Ltd.
 *
 * Author:     ck <chenke@uniontech.com>
 *
 * Maintainer: ck <chenke@uniontech.com>
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

#include <QStylePlugin>
#include "dpluginloader.h"
class ChameleonStylePlugin : public QStylePlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QStyleFactoryInterface" FILE "chameleon.json")

public:
    explicit ChameleonStylePlugin(QObject *parent = nullptr)
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
