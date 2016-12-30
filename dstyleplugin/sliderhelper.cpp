/**
 * Copyright (C) 2016 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#include "style.h"

#include "common.h"
#include "colorutils.h"
#include "geometryutils.h"
#include "paletteextended.h"
#include "commonhelper.h"
#include "painterhelper.h"

#include <QStyleOptionSlider>
#include <QDebug>
#include <QPainterPath>

namespace dstyle {

static const char *SliderTickmarkPositionsProp = "tickmarkPositions";

#define SliderHandleTypeNormal "Normal"
#define SliderHandleTypeVernier "Vernier"
#define SliderHandleTypeNone "None"

QRect Style::sliderSubControlRect(const QStyleOptionComplex *option, QStyle::SubControl subControl, const QWidget *widget) const
{
    // cast option and check
    const QStyleOptionSlider* sliderOption( qstyleoption_cast<const QStyleOptionSlider*>( option ) );
    if( !sliderOption ) return QCommonStyle::subControlRect( CC_Slider, option, subControl, widget );

    switch( subControl )
    {
    case SC_SliderGroove:
    {

        // direction
        const bool horizontal( sliderOption->orientation == Qt::Horizontal );

        // get base class rect
        QRect grooveRect( QCommonStyle::subControlRect( CC_Slider, option, subControl, widget ) );
        grooveRect = GeometryUtils::insideMargin( grooveRect, pixelMetric( PM_DefaultFrameWidth, option, widget ) );

        // centering
        if( horizontal ) {
            grooveRect = GeometryUtils::centerRect( grooveRect, grooveRect.width(), Metrics::Slider_GrooveThickness );
        } else {
            grooveRect = GeometryUtils::centerRect( grooveRect, Metrics::Slider_GrooveThickness, grooveRect.height() );
        }

        return grooveRect;

    }

    default: return QCommonStyle::subControlRect( CC_Slider, option, subControl, widget );
    }
}

bool Style::drawSlider(ComplexControl control, const QStyleOptionComplex *option, QPainter *painter, const QWidget *widget) const
{
    Q_UNUSED(control)

    // cast option and check
    const QStyleOptionSlider *sliderOption( qstyleoption_cast<const QStyleOptionSlider*>( option ) );
    if( !sliderOption ) return true;

    const PaletteExtended *plExt = m_palette;

    // copy rect and palette
    const QRect& rect( option->rect );
    //    const QPalette& palette( option->palette );

    // copy state
    const QStyle::State& state( option->state );
    const bool enabled( state & QStyle::State_Enabled );
    //    const bool mouseOver( enabled && ( state & QStyle::State_MouseOver ) );
//    const bool hasFocus( enabled && ( state & QStyle::State_HasFocus ) );

    // direction
    const bool horizontal( sliderOption->orientation == Qt::Horizontal );

    // tickmarks
    if( sliderOption->subControls & QStyle::SC_SliderTickmarks )
    {
        const bool upsideDown( sliderOption->upsideDown );
        const int tickPosition( sliderOption->tickPosition );
        const int available( pixelMetric( QStyle::PM_SliderSpaceAvailable, option, widget ) );
        int interval = sliderOption->tickInterval;
        if( interval < 1 ) interval = sliderOption->pageStep;
        if( interval >= 1 )
        {
            const int fudge( pixelMetric( QStyle::PM_SliderLength, option, widget ) / 2 );
            int current( sliderOption->minimum );

            // store tick lines
            const QRect grooveRect( subControlRect( QStyle::CC_Slider, sliderOption, QStyle::SC_SliderGroove, widget ) );
            QList<QLine> tickLines;
            if( horizontal )
            {

                if( tickPosition & QSlider::TicksAbove ) tickLines.append( QLine( rect.left(), grooveRect.top() - Metrics::Slider_TickMarginWidth, rect.left(), grooveRect.top() - Metrics::Slider_TickMarginWidth - Metrics::Slider_TickLength ) );
                if( tickPosition & QSlider::TicksBelow ) tickLines.append( QLine( rect.left(), grooveRect.bottom() + Metrics::Slider_TickMarginWidth, rect.left(), grooveRect.bottom() + Metrics::Slider_TickMarginWidth + Metrics::Slider_TickLength ) );

            } else {

                if( tickPosition & QSlider::TicksAbove ) tickLines.append( QLine( grooveRect.left() - Metrics::Slider_TickMarginWidth, rect.top(), grooveRect.left() - Metrics::Slider_TickMarginWidth - Metrics::Slider_TickLength, rect.top() ) );
                if( tickPosition & QSlider::TicksBelow ) tickLines.append( QLine( grooveRect.right() + Metrics::Slider_TickMarginWidth, rect.top(), grooveRect.right() + Metrics::Slider_TickMarginWidth + Metrics::Slider_TickLength, rect.top() ) );

            }

            QList<QVariant> tickmarkPositions;
            while( current <= sliderOption->maximum )
            {

                // adjust pen
                painter->setPen( QPen(plExt->brush(PaletteExtended::Slider_TickmarkColor), Metrics::Painter_PenWidth) );

                // calculate positions and draw lines
                int position( sliderPositionFromValue( sliderOption->minimum, sliderOption->maximum, current, available ) + fudge );
                tickmarkPositions.append(position);

                foreach( const QLine& tickLine, tickLines )
                {
                    if( horizontal ) painter->drawLine( tickLine.translated( upsideDown ? (rect.width() - position) : position, 0 ) );
                    else painter->drawLine( tickLine.translated( 0, upsideDown ? (rect.height() - position):position ) );
                }

                // go to next position
                current += interval;
            }

            QWidget *volatileWidget = const_cast<QWidget*>(widget);
            // Tickmark positions are valuable to both drawing annotations and tickmark magnetic,
            // so we record it here to its properties.
            volatileWidget->setProperty(SliderTickmarkPositionsProp, tickmarkPositions);
        }
    }

    // groove
    if( sliderOption->subControls & QStyle::SC_SliderGroove )
    {
        // retrieve groove rect
        QRect grooveRect( subControlRect( QStyle::CC_Slider, sliderOption, QStyle::SC_SliderGroove, widget ) );

        // base brush
        const QBrush grooveBrush( plExt->brush(PaletteExtended::Slider_GrooveColor) );

        if( !enabled ) drawSliderGroove( painter, grooveRect, grooveBrush );
        else {

            const bool upsideDown( sliderOption->upsideDown );

            // handle rect
            QRect handleRect( subControlRect( QStyle::CC_Slider, sliderOption, QStyle::SC_SliderHandle, widget ) );

            // highlight brush
            const QBrush highlight( plExt->brush(PaletteExtended::Slider_GrooveHighlightColor) );

            if( sliderOption->orientation == Qt::Horizontal )
            {

                QRect leftRect( grooveRect );
                leftRect.setRight( handleRect.right() - Metrics::Slider_ControlThickness/2 );
                drawSliderGroove( painter, leftRect, upsideDown ? grooveBrush:highlight );

                QRect rightRect( grooveRect );
                rightRect.setLeft( handleRect.left() + Metrics::Slider_ControlThickness/2 );
                drawSliderGroove( painter, rightRect, upsideDown ? highlight:grooveBrush );

            } else {

                QRect topRect( grooveRect );
                topRect.setBottom( handleRect.bottom() - Metrics::Slider_ControlThickness/2 );
                drawSliderGroove( painter, topRect, upsideDown ? grooveBrush:highlight );

                QRect bottomRect( grooveRect );
                bottomRect.setTop( handleRect.top() + Metrics::Slider_ControlThickness/2 );
                drawSliderGroove( painter, bottomRect, upsideDown ? highlight:grooveBrush );

            }

        }

    }

    // handle
    if( sliderOption->subControls & QStyle::SC_SliderHandle )
    {

        // get rect and center
        QRect handleRect( subControlRect( QStyle::CC_Slider, sliderOption, QStyle::SC_SliderHandle, widget ) );

        // render
        const QString handleType = widget->property("handleType").toString();
        drawSliderHandle( painter, sliderOption, handleRect, handleType);
    }

    return true;
}

void Style::drawSliderGroove(QPainter *painter, const QRect &rect, const QBrush &brush) const
{
    // setup painter
    painter->setRenderHint( QPainter::Antialiasing, true );

    const QRectF baseRect( rect );
    const qreal radius( 0.5*Metrics::Slider_GrooveThickness );

    // content
    if( brush.style() != Qt::NoBrush )
    {
        painter->setPen( Qt::NoPen );
        painter->setBrush( brush );
        painter->drawRoundedRect( baseRect, radius, radius );
    }

    return;
}

void Style::drawSliderHandle(QPainter* painter, const QStyleOptionSlider *option, const QRect& rect, const QString &type) const
{
    // setup painter
    painter->setRenderHint( QPainter::Antialiasing, true );

    // copy rect
    QRectF frameRect( rect );
    frameRect.adjust( 1, 1, -1, -1 );

    // shadow
    //    if( shadow.isValid() && !sunken )
    //    {

    //        painter->setPen( QPen( shadow, 2 ) );
    //        painter->setBrush( Qt::NoBrush );
    //        painter->drawEllipse( frameRect );

    //    }

    if (type == SliderHandleTypeNone) {
        return; // draw no handle
    } else if (type == SliderHandleTypeVernier) {
        // draw vernier handle
        fillBrush(painter, rect, m_palette->brush(PaletteExtended::Slider_VernierHandleBrush, option));
    } else {
        // draw circle handle
        fillBrush(painter, rect, m_palette->brush(PaletteExtended::Slider_HandleBrush, option));
    }
}

bool Style::drawSliderTickmarkLabels(const QStyleOption *option, QPainter *painter, const QWidget *widget) const
{
    QRect rect( option->rect );

    QVariantList labels = widget->property("tickmarkLabels").toList();
    if (labels.length() == 0) return false;

    QWidget *parent = widget->parentWidget();
    if (parent && parent->inherits("dcc::widgets::DCCSliderAnnotated")) {
        QSlider *slider = parent->findChild<QSlider*>();
        if (slider) {
            QList<QVariant> positions = slider->property(SliderTickmarkPositionsProp).toList();

            PaletteExtended *plExt = m_palette;
            painter->setPen(QPen(plExt->brush(PaletteExtended::Slider_TickmarkColor), Metrics::Painter_PenWidth));

            for (int i = 0; i < positions.length() && i < labels.length(); i++) {
                const QString text = labels.at(i).toString();
                const int rWidth = option->fontMetrics.width(text);

                const int deltaX = slider->x() - widget->x();
                const int x = positions.at(i).toInt() + deltaX;
                const int rX = qMax(0.0, qMin(rect.width() - rWidth / 1.0, x - rWidth / 2.0));

                QRect r ( rX, rect.y(), rWidth, rect.height() );
                painter->drawText(r, Qt::AlignTop, text);
            }
        }
    }

    return true;
}

} // end namespace dstyle
