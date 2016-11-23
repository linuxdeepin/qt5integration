/**
 * Copyright (C) 2016 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#include <QDebug>

#include "lineedithelper.h"
#include "commonhelper.h"
#include "common.h"
#include "style.h"

namespace dstyle {

bool LineEditHelper::drawFrameLineEditPrimitive(const QStyleOption *option, QPainter *painter, const QWidget *widget)
{
    Style *style = CommonHelper::widgetStyle(widget);
    if (!style) return false;

    const QRect& rect( option->rect );

    // copy state
    const QStyle::State& state( option->state );
    const bool enabled( state & QStyle::State_Enabled );
    const bool hasFocus( state & QStyle::State_HasFocus );

    // render
    const QBrush background(style->m_palette->brush(PaletteExtended::LineEdit_BackgroundBrush, enabled, false, hasFocus) );
    const QBrush outline(style->m_palette->brush(PaletteExtended::LineEdit_BorderBrush, enabled, false, hasFocus) );
    CommonHelper::renderFrame( painter, rect, background, outline );

    return true;
}
}// end namespace dstyle
