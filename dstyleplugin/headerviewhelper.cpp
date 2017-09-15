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
#include "paletteextended.h"

namespace dstyle {
bool Style::drawIndicatorHeaderArrowPrimitive(const QStyleOption *option, QPainter *painter, const QWidget *widget) const
{
    Q_UNUSED(widget)

    if (const QStyleOptionHeader *header = qstyleoption_cast<const QStyleOptionHeader *>(option)) {
        QBrush brush;

        if (header->sortIndicator & QStyleOptionHeader::SortUp) {
            brush = m_palette->brush(PaletteExtended::HeaderView_ArrowIndicatorDownIcon);
        } else if (header->sortIndicator & QStyleOptionHeader::SortDown) {
            brush = m_palette->brush(PaletteExtended::HeaderView_ArrowIndicatorUpIcon);
        }

        if (brush.style() != Qt::TexturePattern)
            return false;

        fillBrush(painter, option->rect, brush);
    }

    return true;
}

QRect Style::headerArrowRect(const QStyleOption *opt, const QWidget *widget) const
{
    QRect r;

    int h = opt->rect.height();
    int w = opt->rect.width();
    int x = opt->rect.x();
    int y = opt->rect.y();
    int margin = proxy()->pixelMetric(QStyle::PM_HeaderMargin, opt, widget);

    if (opt->state & State_Horizontal) {
        int horiz_size = Metrics::Header_ArrowSize;
        r.setRect(x + w - margin * 2 - horiz_size, y + (h - horiz_size) / 2,
                  horiz_size, horiz_size);
    } else {
        int vert_size = Metrics::Header_ArrowSize;
        r.setRect(x + (w - vert_size) / 2, y + h - margin * 2 - vert_size,
                  vert_size, vert_size);
    }
    r = visualRect(opt->direction, opt->rect, r);

    return r;
}
}
