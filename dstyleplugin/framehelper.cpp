/**
 * Copyright (C) 2016 by Deepin Technology Co., Ltd.
 *
 * Use of this source code is governed by GPLv3
 * that can be found in the LICENSE file.
 **/

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
