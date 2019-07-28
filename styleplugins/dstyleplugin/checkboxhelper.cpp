/*
 * Copyright (C) 2017 ~ 2018 Deepin Technology Co., Ltd.
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
#include "paletteextended.h"

#include <QPainter>
#include <qdrawutil.h>
#include <QDebug>

namespace dstyle {
bool Style::drawIndicatorCheckBoxPrimitive(const QStyleOption *opt, QPainter *p, const QWidget *widget) const
{
    Q_UNUSED(widget)

    fillBrush(p, opt->rect, m_palette->brush(PaletteExtended::CheckBox_BackgroundBrush, opt));

    return true;
}
}
