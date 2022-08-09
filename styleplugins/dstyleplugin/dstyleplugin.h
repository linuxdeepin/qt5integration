/*
 * SPDX-FileCopyrightText: 2016-2022 UnionTech Software Technology Co., Ltd.  
 * SPDX-License-Identifier: GPL-3.0-or-later
 */
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
