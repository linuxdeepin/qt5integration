/*
 * Copyright (C) 2016 ~ 2017 Deepin Technology Co., Ltd.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "style.h"

#include <QPainter>
#include <QDebug>
#include <QComboBox>

namespace dstyle {

bool Style::drawFramePrimitive(const QStyleOption *option, QPainter *painter, const QWidget *widget) const
{
    // combo box drop-list.
    // line edit completer drop-list
    if (widgetIsComboBoxPopupFramePrimitive(widget)) {
        drawComboBoxPopupFramePrimitive(option, painter);
        return true;
    }

    return true;
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
