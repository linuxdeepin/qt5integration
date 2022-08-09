/*
 * SPDX-FileCopyrightText: 2016 - 2022 UnionTech Software Technology Co., Ltd.  
 * SPDX-License-Identifier: GPL-3.0-or-later
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
