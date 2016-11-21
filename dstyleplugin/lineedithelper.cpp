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
    const bool hasFocus( enabled && ( state & QStyle::State_HasFocus ) );

    // render
    const QBrush background( getBackgroundColor(style->m_palette, enabled, hasFocus) );
    const QColor outline( getBorderColor(style->m_palette, enabled, hasFocus) );
    CommonHelper::renderFrame( painter, rect, background, outline );

    return true;
}

QColor LineEditHelper::getBorderColor(PaletteExtended *plExt, bool enabled, bool hasFocus)
{
    if (!enabled) return plExt->brush(PaletteExtended::LineEdit_BorderDisabledColor).color();
    else if (hasFocus) return plExt->brush(PaletteExtended::LineEdit_BorderFocusedColor).color();
    else return plExt->brush(PaletteExtended::LineEdit_BorderNormalColor).color();
}

QBrush LineEditHelper::getBackgroundColor(PaletteExtended *plExt, bool enabled, bool hasFocus)
{
    if (!enabled) return plExt->brush(PaletteExtended::LineEdit_BackgroundDisabledColor);
    else if (hasFocus) return plExt->brush(PaletteExtended::LineEdit_BackgroundFocusedColor);
    else return plExt->brush(PaletteExtended::LineEdit_BackgroundNormalColor).color();
}

}
