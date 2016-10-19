/**
 * Copyright (C) 2016 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#ifndef DStylePlugin_H
#define DStylePlugin_H

#include <QStylePlugin>


class DStylePlugin : public QStylePlugin
{
    Q_OBJECT
#if QT_VERSION >= 0x050000
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QStyleFactoryInterface" FILE "dstyleplugin.json")
#endif // QT_VERSION >= 0x050000

public:
    DStylePlugin(QObject *parent = 0);

    QStyle* create(const QString &key) Q_DECL_OVERRIDE;
};

#endif // DStylePlugin_H
