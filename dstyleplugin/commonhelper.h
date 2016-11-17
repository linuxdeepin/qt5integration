/**
 * Copyright (C) 2016 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#ifndef COMMONHELPER_H
#define COMMONHELPER_H

#include <QPainter>

namespace dstyle {

class Style;

class CommonHelper
{
public:
    static void renderFrame(QPainter* painter, const QRect& rect, const QColor& color, const QColor& outline);

    static Style *widgetStyle(const QWidget *widget);
};

}

#endif // COMMONHELPER_H
