/**
 * Copyright (C) 2016 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#include "commonhelper.h"
#include "common.h"
#include "style.h"
#include "paletteextended.h"

namespace dstyle {

bool Style::drawFrameLineEditPrimitive(QStyle::PrimitiveElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget) const
{
    Q_UNUSED(element)
    Q_UNUSED(widget)

    const QRect& rect( option->rect );

    // render
    const QBrush background(m_palette->brush(PaletteExtended::LineEdit_BackgroundBrush, option) );
    const QBrush outline(m_palette->brush(PaletteExtended::LineEdit_BorderBrush, option) );
    CommonHelper::renderFrame( painter, rect, background, outline );

    return true;
}
}// end namespace dstyle
