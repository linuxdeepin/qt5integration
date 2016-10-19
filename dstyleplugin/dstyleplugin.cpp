/**
 * Copyright (C) 2016 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#include "dstyleplugin.h"
#include "ddarkstyle.h"
#include "dlightstyle.h"


DStylePlugin::DStylePlugin(QObject *parent) :
    QStylePlugin(parent)
{
}

QStyle *DStylePlugin::create(const QString &key)
{
    if (key == "ddark") {
        return new DDarkStyle();
    } else if (key == "dlight") {
        return new DLightStyle();
    }

    return nullptr;
}

#if QT_VERSION < 0x050000
Q_EXPORT_PLUGIN2(DStylePlugin, DStylePlugin)
#endif // QT_VERSION < 0x050000
