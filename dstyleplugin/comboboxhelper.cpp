/**
 * Copyright (C) 2016 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#include "style.h"

#include <QPainter>
#include <QStyleOption>

#include "paletteextended.h"

namespace dstyle {

void Style::drawComboBoxPopupFramePrimitive(const QStyleOption *option, QPainter *painter) const {
    const QRect rect( option->rect );

    painter->setRenderHints(painter->renderHints() | QPainter::Antialiasing);

    painter->setPen(m_palette->brush(PaletteExtended::Menu_BorderColor).color());
    painter->setBrush(m_palette->brush(PaletteExtended::Menu_BackgroundBrush));

    painter->drawRoundedRect(rect, 4, 4);
}

}
