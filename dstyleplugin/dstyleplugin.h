/**
 * Copyright (C) 2016 by Deepin Technology Co., Ltd.
 *
 * Use of this source code is governed by GPLv3
 * that can be found in the LICENSE file.
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
