/*
 * SPDX-FileCopyrightText: 2017 - 2022 UnionTech Software Technology Co., Ltd.  
 * SPDX-License-Identifier: GPL-3.0-or-later
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

    PainterHelper::drawRect(painter, option->rect.adjusted(1, 1, -1, -1),
                            backgroundBrush, Metrics::Painter_PenWidth, borderBrush);

    return true;
}
}
