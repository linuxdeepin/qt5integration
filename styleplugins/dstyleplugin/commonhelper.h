/*
 * SPDX-FileCopyrightText: 2016-2022 UnionTech Software Technology Co., Ltd.  
 * SPDX-License-Identifier: GPL-3.0-or-later
 */
#ifndef COMMONHELPER_H
#define COMMONHELPER_H

#include <QPainter>

namespace dstyle {

class Style;

class CommonHelper
{
public:
    static void renderFrame(QPainter* painter, const QRect& rect, const QBrush &brush, const QBrush &outline);

    static Style *widgetStyle(const QWidget *widget);
};

}

#endif // COMMONHELPER_H
