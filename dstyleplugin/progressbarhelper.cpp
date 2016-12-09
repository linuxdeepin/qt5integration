/**
 * Copyright (C) 2016 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#include "style.h"

#include <QStyleOption>
#include <QStyleOptionProgressBarV2>
#include <QPainter>
#include <QPainterPath>
#include <QDebug>

#include "geometryutils.h"
#include "paletteextended.h"

namespace dstyle {

bool Style::drawProgressBarControl(const QStyleOption *option, QPainter *painter, const QWidget *widget) const
{
    const QStyleOptionProgressBar* progressBarOption( qstyleoption_cast<const QStyleOptionProgressBar*>( option ) );
    if( !progressBarOption ) return false;

    // render groove
    QStyleOptionProgressBarV2 progressBarOption2 = *progressBarOption;
    progressBarOption2.rect = subElementRect( SE_ProgressBarGroove, progressBarOption, widget );
    drawControl( CE_ProgressBarGroove, &progressBarOption2, painter, widget );

    // render contents
    progressBarOption2.rect = subElementRect( SE_ProgressBarContents, progressBarOption, widget );
    drawControl( CE_ProgressBarContents, &progressBarOption2, painter, widget );

    // render text
    const bool textVisible( progressBarOption->textVisible );
    const bool busy( progressBarOption->minimum == 0 && progressBarOption->maximum == 0 );
    if( textVisible && !busy )
    {
        progressBarOption2.rect = subElementRect( SE_ProgressBarLabel, progressBarOption, widget );
        drawControl( CE_ProgressBarLabel, &progressBarOption2, painter, widget );
    }

    return true;
}

bool Style::drawProgressBarContentsControl(const QStyleOption *option, QPainter *painter, const QWidget *) const
{
    const QStyleOptionProgressBar* progressBarOption( qstyleoption_cast<const QStyleOptionProgressBar*>( option ) );
    if( !progressBarOption ) return false;

    // copy rect and palette
    const QPalette& palette( option->palette );
    const QRect rect( option->rect );
    const qreal radius( GeometryUtils::frameRadius() );

    QPainterPath path;
    path.addRoundedRect(rect, radius, radius);

    painter->save();
    painter->setRenderHints(painter->renderHints() | QPainter::Antialiasing);
    painter->setClipPath(path);
    painter->fillRect(rect, palette.highlight());
    painter->restore();

    return true;
}

bool Style::drawProgressBarGrooveControl(const QStyleOption *option, QPainter *painter, const QWidget *widget) const
{
    Q_UNUSED(widget)

    const QRect rect( option->rect );
    const qreal radius( GeometryUtils::frameRadius() );

    const PaletteExtended *plExt = m_palette;
    const QBrush brush( plExt->brush(PaletteExtended::PushButton_BackgroundBrush, option) );

    QPainterPath path;
    path.addRoundedRect(rect, radius, radius);

    painter->save();
    painter->setRenderHints(painter->renderHints() | QPainter::Antialiasing);
    painter->setClipPath(path);
    painter->fillRect(rect, brush);
    painter->restore();

    return true;
}

bool Style::drawProgressBarLabelControl(const QStyleOption *option, QPainter *painter, const QWidget *) const
{
    // cast option and check
    const QStyleOptionProgressBar* progressBarOption( qstyleoption_cast<const QStyleOptionProgressBar*>( option ) );
    if( !progressBarOption ) return true;

    // get direction and check
    const QStyleOptionProgressBarV2* progressBarOption2( qstyleoption_cast<const QStyleOptionProgressBarV2*>( option ) );
    const bool horizontal = !progressBarOption2 || progressBarOption2->orientation == Qt::Horizontal;
    if( !horizontal ) return true;

    // store rect and palette
    const QRect& rect( option->rect );
    const QPalette& palette( option->palette );

    // store state and direction
    const State& state( option->state );
    const bool enabled( state & State_Enabled );

    // define text rect
    Qt::Alignment hAlign( ( progressBarOption->textAlignment == Qt::AlignLeft ) ? Qt::AlignHCenter : progressBarOption->textAlignment );
    drawItemText( painter, rect, Qt::AlignVCenter | hAlign, palette, enabled, progressBarOption->text, QPalette::WindowText );

    return true;
}

QRect Style::progressBarGrooveRect(const QStyleOption *option, const QWidget *widget) const
{
    // cast option and check
    const QStyleOptionProgressBar* progressBarOption( qstyleoption_cast<const QStyleOptionProgressBar*>( option ) );
    if( !progressBarOption ) return option->rect;

    // get flags and orientation
    const bool textVisible( progressBarOption->textVisible );
    const bool busy( progressBarOption->minimum == 0 && progressBarOption->maximum == 0 );

    const QStyleOptionProgressBarV2* progressBarOption2( qstyleoption_cast<const QStyleOptionProgressBarV2*>( option ) );
    const bool horizontal( !progressBarOption2 || progressBarOption2->orientation == Qt::Horizontal );

    // copy rectangle and adjust
    QRect rect( option->rect );
    const int frameWidth( pixelMetric( PM_DefaultFrameWidth, option, widget ) );
    if( horizontal ) rect = GeometryUtils::insideMargin( rect, frameWidth, 0 );
    else rect = GeometryUtils::insideMargin( rect, 0, frameWidth );

    if( textVisible && !busy && horizontal )
    {
        QRect textRect( subElementRect( SE_ProgressBarLabel, option, widget ) );
        textRect = visualRect( option->direction, option->rect, textRect );
        rect = visualRect( option->direction, option->rect, rect );
        rect = GeometryUtils::centerRect( rect, rect.width(), Metrics::ProgressBar_Thickness );
    } else if( horizontal ) {
        rect = GeometryUtils::centerRect( rect, rect.width(), Metrics::ProgressBar_Thickness );
    } else {
        rect = GeometryUtils::centerRect( rect, Metrics::ProgressBar_Thickness, rect.height() );
    }

    return rect;
}

QRect Style::progressBarContentsRect(const QStyleOption *option, const QWidget *widget) const
{
    // cast option and check
    const QStyleOptionProgressBar* progressBarOption( qstyleoption_cast<const QStyleOptionProgressBar*>( option ) );
    if( !progressBarOption ) return QRect();

    // get groove rect
    const QRect rect( progressBarGrooveRect( option, widget ) );

    // in busy mode, grooveRect is used
    const bool busy( progressBarOption->minimum == 0 && progressBarOption->maximum == 0 );
    if( busy ) return rect;

    // get orientation
    const QStyleOptionProgressBarV2* progressBarOption2( qstyleoption_cast<const QStyleOptionProgressBarV2*>( option ) );
    const bool horizontal( !progressBarOption2 || progressBarOption2->orientation == Qt::Horizontal );

    // check inverted appearance
    const bool inverted( progressBarOption2 ? progressBarOption2->invertedAppearance : false );

    // get progress and steps
    const qreal progress( progressBarOption->progress - progressBarOption->minimum );
    const int steps( qMax( progressBarOption->maximum  - progressBarOption->minimum, 1 ) );

    //Calculate width fraction
    const qreal widthFrac = qMin( qreal(1), progress/steps );

    // convert the pixel width
    const int indicatorSize( widthFrac*( horizontal ? rect.width():rect.height() ) );

    QRect indicatorRect;
    if( horizontal )
    {

        indicatorRect = QRect( inverted ? ( rect.right() - indicatorSize + 1):rect.left(), rect.y(), indicatorSize, rect.height() );
        indicatorRect = visualRect( option->direction, rect, indicatorRect );

    } else indicatorRect = QRect( rect.x(), inverted ? rect.top() : (rect.bottom() - indicatorSize + 1), rect.width(), indicatorSize );

    return indicatorRect;
}

QRect Style::progressBarLabelRect(const QStyleOption *option, const QWidget *) const
{
    // cast option and check
    const QStyleOptionProgressBar* progressBarOption( qstyleoption_cast<const QStyleOptionProgressBar*>( option ) );
    if( !progressBarOption ) return QRect();

    // get flags and check
    const bool textVisible( progressBarOption->textVisible );
    const bool busy( progressBarOption->minimum == 0 && progressBarOption->maximum == 0 );
    if( !textVisible || busy ) return QRect();

    // get direction and check
    const QStyleOptionProgressBarV2* progressBarOption2( qstyleoption_cast<const QStyleOptionProgressBarV2*>( option ) );
    const bool horizontal( !progressBarOption2 || progressBarOption2->orientation == Qt::Horizontal );
    if( !horizontal ) return QRect();

    int textWidth = qMax(
        option->fontMetrics.size( Qt::TextHideMnemonic, progressBarOption->text ).width(),
        option->fontMetrics.size( Qt::TextHideMnemonic, QStringLiteral( "100%" ) ).width() );
    int textHeight = option->fontMetrics.size( Qt::TextHideMnemonic, QStringLiteral( "100%" ) ).height();

    QRect rect( GeometryUtils::insideMargin( option->rect, Metrics::Frame_FrameWidth, 0 ) );

    return GeometryUtils::centerRect(rect, QSize(textWidth, textHeight));
}

}
