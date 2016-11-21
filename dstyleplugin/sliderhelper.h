/**
 * Copyright (C) 2016 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#ifndef SLIDERHELPER_H
#define SLIDERHELPER_H

#include <QRect>
#include <QStyle>
#include <QStyleOptionComplex>
#include <QPainter>
#include <QWidget>

namespace dstyle {

class SliderHelper
{
public:
    static bool render(const QStyleOptionComplex *opt, QPainter *p, const QWidget *w);

    static void renderSliderGroove(QPainter*, const QRect&, const QBrush & brush);
    static void renderSliderHandle(QPainter* painter, const QRect& rect, const QBrush &brush, const QColor& outline);
};

}

#endif // SLIDERHELPER_H
