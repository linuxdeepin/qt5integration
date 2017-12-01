/*
 * Copyright (C) 2017 ~ 2017 Deepin Technology Co., Ltd.
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
#include "painterhelper.h"
#include "paletteextended.h"
#include "common.h"

namespace dstyle {
bool Style::drawRubberBandControl(const QStyleOption *option, QPainter *painter, const QWidget *widget) const
{
    Q_UNUSED(widget)

    const QBrush &backgroundBrush = m_palette->brush(PaletteExtended::RubberBand_BackgroundBrush, option);
    const QBrush &borderBrush = m_palette->brush(PaletteExtended::RubberBand_BorderBrush, option);

    PainterHelper::drawRect(painter, option->rect.adjusted(0, 0, -1, -1),
                            backgroundBrush, Metrics::Painter_PenWidth, borderBrush);

    return true;
}
}
