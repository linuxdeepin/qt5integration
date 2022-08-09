/*
 * SPDX-FileCopyrightText: 2016 - 2022 UnionTech Software Technology Co., Ltd.  
 * SPDX-License-Identifier: GPL-3.0-or-later
 */
#include "dstyleplugin.h"
#include "style.h"

DStylePlugin::DStylePlugin(QObject *parent) :
    QStylePlugin(parent)
{
}

QStyle *DStylePlugin::create(const QString &key)
{
    if (key == "ddark") {
        return new dstyle::Style(dstyle::StyleDark);
    } else if (key == "dlight") {
        return new dstyle::Style(dstyle::StyleLight);
    } else if (key == "dsemidark") {
        return new dstyle::Style(dstyle::StyleSemiDark);
    } else if (key == "dsemilight") {
        return new dstyle::Style(dstyle::StyleSemiLight);
    }

    return nullptr;
}

#if QT_VERSION < 0x050000
Q_EXPORT_PLUGIN2(DStylePlugin, DStylePlugin)
#endif // QT_VERSION < 0x050000
