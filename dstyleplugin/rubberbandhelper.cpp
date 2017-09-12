/**
 * Copyright (C) 2017 by Deepin Technology Co., Ltd.
 *
 * Use of this source code is governed by GPLv3
 * that can be found in the LICENSE file.
 **/

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

    PainterHelper::drawRect(painter, option->rect, backgroundBrush, Metrics::Painter_PenWidth, borderBrush);

    return true;
}
}
