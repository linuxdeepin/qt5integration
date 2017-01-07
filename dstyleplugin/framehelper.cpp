/**
 * Copyright (C) 2016 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#include "style.h"

#include <QPainter>
#include <QDebug>
#include <QComboBox>

namespace dstyle {

bool Style::drawFramePrimitive(const QStyleOption *option, QPainter *painter, const QWidget *widget) const
{
    // combo box drop-list.
    if (widget->inherits("QComboBoxPrivateContainer")) {
        drawComboBoxPopupFramePrimitive(option, painter);
        return true;
    }

    // line edit completer drop-list
    if (widget->inherits("QAbstractItemView") || !widget->parentWidget()) {
        drawComboBoxPopupFramePrimitive(option, painter);
        return true;
    }

    return false;
}

bool Style::drawFrameFocusRectPrimitive(const QStyleOption *option, QPainter *painter, const QWidget *widget) const
{
    Q_UNUSED(option)
    Q_UNUSED(painter)
    Q_UNUSED(widget)

    // draw nothing
    return true;
}

bool Style::drawShapedFrameControl(const QStyleOption *option, QPainter *painter, const QWidget *widget) const
{
    Q_UNUSED(option)
    Q_UNUSED(painter)
    Q_UNUSED(widget)

    return false;
}

}
