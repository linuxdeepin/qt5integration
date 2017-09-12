/**
 * Copyright (C) 2016 by Deepin Technology Co., Ltd.
 *
 * Use of this source code is governed by GPLv3
 * that can be found in the LICENSE file.
 **/

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
