/**
 * Copyright (C) 2016 by Deepin Technology Co., Ltd.
 *
 * Use of this source code is governed by GPLv3
 * that can be found in the LICENSE file.
 **/

#include "commonhelper.h"
#include "common.h"
#include "style.h"
#include "paletteextended.h"

namespace dstyle {

bool Style::drawFrameLineEditPrimitive(const QStyleOption *option, QPainter *painter, const QWidget *widget) const
{
    Q_UNUSED(widget)

    const QRect& rect( option->rect );

    // render
    const QBrush background(m_palette->brush(PaletteExtended::LineEdit_BackgroundBrush, option) );
    const QBrush outline(m_palette->brush(PaletteExtended::LineEdit_BorderBrush, option) );
    CommonHelper::renderFrame( painter, rect, background, outline );

    return true;
}
}// end namespace dstyle
