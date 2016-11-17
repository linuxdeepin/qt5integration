/**
 * Copyright (C) 2016 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#include "sliderhelper.h"
#include "common.h"
#include "colorutils.h"
#include "geometryutils.h"
#include "paletteextended.h"
#include "style.h"
#include "commonhelper.h"

#include <QStyleOptionSlider>

namespace dstyle {

bool SliderHelper::render(const QStyleOptionComplex *option, QPainter *painter, const QWidget *widget)
{
    Style *style = CommonHelper::widgetStyle(widget);
    if ( !style ) return true;

    // cast option and check
    const QStyleOptionSlider *sliderOption( qstyleoption_cast<const QStyleOptionSlider*>( option ) );
    if( !sliderOption ) return true;

    const PaletteExtended *plExt = style->m_palette;

    // copy rect and palette
    const QRect& rect( option->rect );
    const QPalette& palette( option->palette );

    // copy state
    const QStyle::State& state( option->state );
    const bool enabled( state & QStyle::State_Enabled );
    const bool mouseOver( enabled && ( state & QStyle::State_MouseOver ) );
    const bool hasFocus( enabled && ( state & QStyle::State_HasFocus ) );

    // direction
    const bool horizontal( sliderOption->orientation == Qt::Horizontal );

    // tickmarks
    if( sliderOption->subControls & QStyle::SC_SliderTickmarks )
    {
        const bool upsideDown( sliderOption->upsideDown );
        const int tickPosition( sliderOption->tickPosition );
        const int available( style->pixelMetric( QStyle::PM_SliderSpaceAvailable, option, widget ) );
        int interval = sliderOption->tickInterval;
        if( interval < 1 ) interval = sliderOption->pageStep;
        if( interval >= 1 )
        {
            const int fudge( style->pixelMetric( QStyle::PM_SliderLength, option, widget ) / 2 );
            int current( sliderOption->minimum );

            // store tick lines
            const QRect grooveRect( style->subControlRect( QStyle::CC_Slider, sliderOption, QStyle::SC_SliderGroove, widget ) );
            QList<QLine> tickLines;
            if( horizontal )
            {

                if( tickPosition & QSlider::TicksAbove ) tickLines.append( QLine( rect.left(), grooveRect.top() - Metrics::Slider_TickMarginWidth, rect.left(), grooveRect.top() - Metrics::Slider_TickMarginWidth - Metrics::Slider_TickLength ) );
                if( tickPosition & QSlider::TicksBelow ) tickLines.append( QLine( rect.left(), grooveRect.bottom() + Metrics::Slider_TickMarginWidth, rect.left(), grooveRect.bottom() + Metrics::Slider_TickMarginWidth + Metrics::Slider_TickLength ) );

            } else {

                if( tickPosition & QSlider::TicksAbove ) tickLines.append( QLine( grooveRect.left() - Metrics::Slider_TickMarginWidth, rect.top(), grooveRect.left() - Metrics::Slider_TickMarginWidth - Metrics::Slider_TickLength, rect.top() ) );
                if( tickPosition & QSlider::TicksBelow ) tickLines.append( QLine( grooveRect.right() + Metrics::Slider_TickMarginWidth, rect.top(), grooveRect.right() + Metrics::Slider_TickMarginWidth + Metrics::Slider_TickLength, rect.top() ) );

            }

            // colors
            const QColor base( palette.base().color() );
            const QColor highlight( plExt->color(PaletteExtended::Slider_TickmarkColor) );

            while( current <= sliderOption->maximum )
            {

                // adjust color
                const QColor color( (enabled && current <= sliderOption->sliderPosition) ? highlight:base );
                painter->setPen( color );

                // calculate positions and draw lines
                int position( style->sliderPositionFromValue( sliderOption->minimum, sliderOption->maximum, current, available ) + fudge );
                foreach( const QLine& tickLine, tickLines )
                {
                    if( horizontal ) painter->drawLine( tickLine.translated( upsideDown ? (rect.width() - position) : position, 0 ) );
                    else painter->drawLine( tickLine.translated( 0, upsideDown ? (rect.height() - position):position ) );
                }

                // go to next position
                current += interval;

            }
        }
    }

    // groove
    if( sliderOption->subControls & QStyle::SC_SliderGroove )
    {
        // retrieve groove rect
        QRect grooveRect( style->subControlRect( QStyle::CC_Slider, sliderOption, QStyle::SC_SliderGroove, widget ) );

        // base color
        const QColor grooveColor( plExt->color(PaletteExtended::Slider_GrooveColor) );

        if( !enabled ) renderSliderGroove( painter, grooveRect, grooveColor );
        else {

            const bool upsideDown( sliderOption->upsideDown );

            // handle rect
            QRect handleRect( style->subControlRect( QStyle::CC_Slider, sliderOption, QStyle::SC_SliderHandle, widget ) );

            // highlight color
            const QColor highlight( plExt->color(PaletteExtended::Slider_GrooveHighlightColor) );

            if( sliderOption->orientation == Qt::Horizontal )
            {

                QRect leftRect( grooveRect );
                leftRect.setRight( handleRect.right() - Metrics::Slider_ControlThickness/2 );
                renderSliderGroove( painter, leftRect, upsideDown ? grooveColor:highlight );

                QRect rightRect( grooveRect );
                rightRect.setLeft( handleRect.left() + Metrics::Slider_ControlThickness/2 );
                renderSliderGroove( painter, rightRect, upsideDown ? highlight:grooveColor );

            } else {

                QRect topRect( grooveRect );
                topRect.setBottom( handleRect.bottom() - Metrics::Slider_ControlThickness/2 );
                renderSliderGroove( painter, topRect, upsideDown ? grooveColor:highlight );

                QRect bottomRect( grooveRect );
                bottomRect.setTop( handleRect.top() + Metrics::Slider_ControlThickness/2 );
                renderSliderGroove( painter, bottomRect, upsideDown ? highlight:grooveColor );

            }

        }

    }

    // handle
    if( sliderOption->subControls & QStyle::SC_SliderHandle )
    {

        // get rect and center
        QRect handleRect( style->subControlRect( QStyle::CC_Slider, sliderOption, QStyle::SC_SliderHandle, widget ) );

        // handle state
//        const bool handleActive( sliderOption->activeSubControls & QStyle::SC_SliderHandle );
//        const bool sunken( state & ( QStyle::State_On|QStyle::State_Sunken) );

        // define colors
        const QColor background( plExt->color(PaletteExtended::Slider_HandleColor) );

        // TODO: calculate outline shadow with some sort of algorithm.
        //        const QColor outline( _helper->sliderOutlineColor( palette, handleActive && mouseOver, hasFocus, opacity, mode ) );
        //        const QColor shadow( _helper->shadowColor( palette ) );
        const QColor outline( Qt::lightGray );

        // render
        renderSliderHandle( painter, handleRect, background, outline );
    }

    return true;
}

void SliderHelper::renderSliderGroove(QPainter *painter, const QRect &rect, const QColor &color)
{
    // setup painter
    painter->setRenderHint( QPainter::Antialiasing, true );

    const QRectF baseRect( rect );
    const qreal radius( 0.5*Metrics::Slider_GrooveThickness );

    // content
    if( color.isValid() )
    {
        painter->setPen( Qt::NoPen );
        painter->setBrush( color );
        painter->drawRoundedRect( baseRect, radius, radius );
    }

    return;
}

void SliderHelper::renderSliderHandle( QPainter* painter, const QRect& rect, const QColor& color, const QColor& outline )
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

    // set pen
    if( outline.isValid() )
    {
        painter->setPen( outline );
        frameRect.adjust( 0.5, 0.5, -0.5, -0.5 );
    } else painter->setPen( Qt::NoPen );

    // set brush
    if( color.isValid() ) painter->setBrush( color );
    else painter->setBrush( Qt::NoBrush );

    // render
    painter->drawEllipse( frameRect );

}

} // end namespace dstyle
