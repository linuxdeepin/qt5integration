/*
 * SPDX-FileCopyrightText: 2017 - 2022 UnionTech Software Technology Co., Ltd.  
 * SPDX-License-Identifier: GPL-3.0-or-later
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
