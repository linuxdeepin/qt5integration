/**
 * Copyright (C) 2016 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#include <QWidget>

#include <private/qstylesheetstyle_p.h>

#include "style.h"
#include "commonhelper.h"
#include "geometryutils.h"

namespace dstyle {

void CommonHelper::renderFrame(QPainter *painter, const QRect &rect, const QBrush &brush, const QBrush &outline)
{
    painter->setRenderHint( QPainter::Antialiasing );

    QRectF frameRect( rect );
    qreal radius( GeometryUtils::frameRadius() );

    // set pen
    if( outline.style() != Qt::NoBrush )
    {
        painter->setPen( QPen(outline, Metrics::Painter_PenWidth) );
    } else {
        painter->setPen( Qt::NoPen );
    }

    // set brush
    painter->setBrush( brush );

    // render
    painter->drawRoundedRect( frameRect, radius, radius );
}

Style *CommonHelper::widgetStyle(const QWidget *widget)
{
    Style *style (Q_NULLPTR);
    style = qobject_cast<Style*>(widget->style());
    if (!style) {
        // FIXME(hualet): why qobject_cast won't work and cause the whole plugin
        // to stop loading.
        QStyleSheetStyle * sstyle = static_cast<QStyleSheetStyle*>(widget->style());
        if (!sstyle) return Q_NULLPTR;

        // FIXME(hualet): don't know why QStyleSheetStyle::base can be empty sometime,
        // and also don't know why QStyleSheetStyle::baseStyle is not exported or something,
        // causing the program to crash if I use baseStyle() instead of base directly here.
        // Basically below 9 lines are copied from QStyleSheetStyle::baseStyle implementation.
        QStyle *base ( Q_NULLPTR );
        if (sstyle->base) {
            base = sstyle->base;
        } else if (QStyleSheetStyle *me = static_cast<QStyleSheetStyle*>(qApp->style())) {
            base = me->base;
        } else {
            base = qApp->style();
        }

        style = qobject_cast<Style*>(base);
    }
    return style;
}

}
