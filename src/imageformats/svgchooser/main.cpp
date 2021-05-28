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

#include <qimageiohandler.h>
#include "dpluginloader.h"
#if !defined(QT_NO_SVGRENDERER)

QT_BEGIN_NAMESPACE

class QSvgPlugin : public QImageIOPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QImageIOHandlerFactoryInterface" FILE "dsvg.json")

public:
    explicit QSvgPlugin(QObject *parent = nullptr);
    Capabilities capabilities(QIODevice *device, const QByteArray &format) const;
    QImageIOHandler *create(QIODevice *device, const QByteArray &format = QByteArray()) const;

private:
    QScopedPointer<QImageIOPlugin> m_pluginProxy;
};

QSvgPlugin::QSvgPlugin(QObject *parent)
    : QImageIOPlugin(parent)
{
    m_pluginProxy.reset(DPluginLoader::load<QImageIOPlugin>(TARGET));
}

QImageIOPlugin::Capabilities QSvgPlugin::capabilities(QIODevice *device, const QByteArray &format) const
{
    return m_pluginProxy ? m_pluginProxy->capabilities(device, format) : nullptr;
}

QImageIOHandler *QSvgPlugin::create(QIODevice *device, const QByteArray &format) const
{
    return m_pluginProxy ? m_pluginProxy->create(device, format) : nullptr;
}

QT_END_NAMESPACE

#include "main.moc"

#endif // !QT_NO_IMAGEFORMATPLUGIN
