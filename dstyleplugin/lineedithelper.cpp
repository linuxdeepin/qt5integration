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
