/*
 * Copyright (C) 2016 ~ 2018 Deepin Technology Co., Ltd.
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

bool Style::drawProgressBarContentsControl(const QStyleOption *option, QPainter *painter, const QWidget *widget) const
{
    // QStyleSheetStyle won't draw progress bar groove which is necessary for us.
    if (widget &&
        widget->style() &&
        widget->style()->metaObject() &&
        widget->style()->metaObject()->className() == QLatin1String("QStyleSheetStyle")) {
        drawProgressBarGrooveControl(option, painter, widget);
    }

    const QStyleOptionProgressBar* progressBarOption( qstyleoption_cast<const QStyleOptionProgressBar*>( option ) );
    if( !progressBarOption ) return false;

    const bool horizontal( !progressBarOption || progressBarOption->orientation == Qt::Horizontal );

    // copy rect and palette
    const qreal radius( (horizontal ? option->rect.height() : option->rect.width()) / 2.0 );

    // get progress and steps
//    const qreal progress( progressBarOption->progress - progressBarOption->minimum );
//    const int steps( qMax( progressBarOption->maximum  - progressBarOption->minimum, 1 ) );

    //Calculate width fraction
//    const qreal widthFrac = qMin( qreal(1), progress/steps );

    QRect rect( option->rect );

    // convert the pixel width
//    const int indicatorSize( widthFrac*( horizontal ? rect.width():rect.height() ) );

//    rect.setWidth(indicatorSize);

    QPainterPath path;
    path.addRoundedRect(rect, radius, radius);

    painter->save();
    painter->setRenderHints(painter->renderHints() | QPainter::Antialiasing);
    painter->setClipPath(path);
    painter->fillRect(rect, m_palette->brush(PaletteExtended::ProgressBar_ContentBackground, option));
    painter->strokePath(path, QPen(m_palette->brush(PaletteExtended::ProgressBar_ContentBorder), ProgressBar_BorderWidth));
    painter->restore();

    return true;
}

bool Style::drawProgressBarGrooveControl(const QStyleOption *option, QPainter *painter, const QWidget *widget) const
{
    Q_UNUSED(widget)

    const QStyleOptionProgressBar* progressBarOption( qstyleoption_cast<const QStyleOptionProgressBar*>( option ) );
    if( !progressBarOption ) return false;
    const bool horizontal( !progressBarOption || progressBarOption->orientation == Qt::Horizontal );

    const QRect rect( option->rect );
    const qreal radius( (horizontal ? rect.height() : rect.height()) / 2.0 );

    const PaletteExtended *plExt = m_palette;
    const QBrush brush( plExt->brush(PaletteExtended::ProgressBar_GrooveBackground, option) );

    QPainterPath path;
    path.addRoundedRect(rect, radius, radius);

    painter->save();
    painter->setRenderHints(painter->renderHints() | QPainter::Antialiasing);
    painter->setClipPath(path);
    painter->fillRect(rect, brush);
    painter->strokePath(path, QPen(plExt->brush(PaletteExtended::ProgressBar_GrooveBorder), ProgressBar_BorderWidth));
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
