/**
 * Copyright (C) 2016 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#ifndef DDESTYLEPLUGIN_H
#define DDESTYLEPLUGIN_H

#include <QStylePlugin>


class DDEStylePlugin : public QStylePlugin
{
    Q_OBJECT
#if QT_VERSION >= 0x050000
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QStyleFactoryInterface" FILE "ddestyleplugin.json")
#endif // QT_VERSION >= 0x050000

public:
    DDEStylePlugin(QObject *parent = 0);

    QStyle* create(const QString &key) Q_DECL_OVERRIDE;
};

#endif // DDESTYLEPLUGIN_H
