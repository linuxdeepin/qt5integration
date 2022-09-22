/*
 * SPDX-FileCopyrightText: 2021 - 2022 UnionTech Software Technology Co., Ltd.  
 * SPDX-License-Identifier: LGPL-3.0-or-later
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
    QSvgPlugin(QObject *parent = nullptr);
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
