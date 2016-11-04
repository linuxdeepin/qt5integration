/**
 * Copyright (C) 2016 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#ifndef PUSHBUTTONHELPER_H
#define PUSHBUTTONHELPER_H

#include <QStyleOption>
#include <QPainter>
#include <QWidget>

namespace dstyle {

class PushButtonHelper
{
public:
    static bool drawPushButtonBevel(const QStyleOption*, QPainter*, const QWidget*);
    static bool drawPushButtonLabel(const QStyleOption *, QPainter *, const QWidget *);
    static bool drawFlatButtonFrame(QPainter* painter, const QRect& rect, const QColor& color, bool sunken);
    static bool drawPushButtonFrame(QPainter* painter, const QRect& rect, const QColor& color, const QColor& outline, const QColor& shadow);

    static QColor getButtonBorderColor(bool enabled, bool mouseOver, bool hasFocus, bool sunken);
    static QColor getButtonColor(bool enabled, bool mouseOver, bool hasFocus, bool sunken);
    static QColor getButtonTextColor(bool enabled, bool mouseOver, bool hasFocus, bool sunken);
};

}

#endif // PUSHBUTTONHELPER_H
