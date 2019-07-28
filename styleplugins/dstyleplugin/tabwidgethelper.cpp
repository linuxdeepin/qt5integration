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
#include "geometryutils.h"
#include "painterhelper.h"

#include <QPainter>
#include <QTabWidget>
#include <QDebug>

namespace dstyle {
bool Style::drawFrameTabWidgetPrimitive( const QStyleOption* option, QPainter* painter, const QWidget* widget ) const
{
    Q_UNUSED(widget)

    // cast option and check
    const QStyleOptionTabWidgetFrameV2* tabOption( qstyleoption_cast<const QStyleOptionTabWidgetFrameV2*>( option ) );
    if( !tabOption ) return true;

    // do nothing if tabbar is hidden
    const bool isQtQuickControl( false );
    if( tabOption->tabBarSize.isEmpty() && !isQtQuickControl ) return true;

    // adjust rect to handle overlaps
    QRect rect( option->rect );

    const QRect tabBarRect( tabOption->tabBarRect );
    const QSize tabBarSize( tabOption->tabBarSize );
    Corners corners = AllCorners;

    // adjust corners to deal with oversized tabbars
    switch( tabOption->shape )
    {
    case QTabBar::RoundedNorth:
    case QTabBar::TriangularNorth:
        if( isQtQuickControl ) rect.adjust( -1, -1, 1, 0 );
        if( tabBarSize.width() >= rect.width() - 2*Metrics::Frame_FrameRadius ) corners &= ~CornersTop;
        if( tabBarRect.left() < rect.left() + Metrics::Frame_FrameRadius ) corners &= ~CornerTopLeft;
        if( tabBarRect.right() > rect.right() - Metrics::Frame_FrameRadius ) corners &= ~CornerTopRight;
        break;

    case QTabBar::RoundedSouth:
    case QTabBar::TriangularSouth:
        if( isQtQuickControl ) rect.adjust( -1, 0, 1, 1 );
        if( tabBarSize.width() >= rect.width()-2*Metrics::Frame_FrameRadius ) corners &= ~CornersBottom;
        if( tabBarRect.left() < rect.left() + Metrics::Frame_FrameRadius ) corners &= ~CornerBottomLeft;
        if( tabBarRect.right() > rect.right() - Metrics::Frame_FrameRadius ) corners &= ~CornerBottomRight;
        break;

    case QTabBar::RoundedWest:
    case QTabBar::TriangularWest:
        if( isQtQuickControl ) rect.adjust( -1, 0, 0, 0 );
        if( tabBarSize.height() >= rect.height()-2*Metrics::Frame_FrameRadius ) corners &= ~CornersLeft;
        if( tabBarRect.top() < rect.top() + Metrics::Frame_FrameRadius ) corners &= ~CornerTopLeft;
        if( tabBarRect.bottom() > rect.bottom() - Metrics::Frame_FrameRadius ) corners &= ~CornerBottomLeft;
        break;

    case QTabBar::RoundedEast:
    case QTabBar::TriangularEast:
        if( isQtQuickControl ) rect.adjust( 0, 0, 1, 0 );
        if( tabBarSize.height() >= rect.height()-2*Metrics::Frame_FrameRadius ) corners &= ~CornersRight;
        if( tabBarRect.top() < rect.top() + Metrics::Frame_FrameRadius ) corners &= ~CornerTopRight;
        if( tabBarRect.bottom() > rect.bottom() - Metrics::Frame_FrameRadius ) corners &= ~CornerBottomRight;
        break;

    default: break;
    }

    // define colors
//    const QPalette& palette( option->palette );
    const QColor background;
    const QColor outline( Qt::gray );

    painter->setRenderHint( QPainter::Antialiasing );

    QRectF frameRect( rect.adjusted( 1, 1, -1, -1 ) );
    qreal radius( GeometryUtils::frameRadius() );

    // set pen
    if( outline.isValid() )
    {

        painter->setPen( outline );
        frameRect.adjust( 0.5, 0.5, -0.5, -0.5 );
        radius = qMax( radius-1, qreal( 0.0 ) );

    } else painter->setPen( Qt::NoPen );

    // set brush
    if( background.isValid() ) painter->setBrush( background );
    else painter->setBrush( Qt::NoBrush );

    // render
    QPainterPath path( PainterHelper::roundedPath( frameRect, corners, radius ) );
    QWidget *current_widget = qobject_cast<const QTabWidget*>(widget)->currentWidget();
    QColor fill_color = painter->pen().color();

    fill_color.setAlphaF(0.2);
    fill_color = PainterHelper::colorBlend(option->palette.color(QPalette::Window), fill_color);

    if (current_widget) {
        painter->fillPath(path, fill_color);

        QPainterPath inside_path;
        QRectF rect = current_widget->parentWidget()->geometry();

        inside_path.addRoundedRect(rect.adjusted(5.5, 5.5, -5.5, -5.5), radius, radius);

        painter->fillPath(inside_path, option->palette.color(QPalette::Window));
        painter->drawPath(inside_path);
    }

    QRectF selectedTabRect = QRectF(tabOption->selectedTabRect).adjusted(1.5, 0.5, -0.5, -0.5);
    QPainterPath selectedtabBarButtonPath(PainterHelper::roundedPath(selectedTabRect, CornerTopLeft | CornerTopRight, radius));
    QPainterPath tabBarButtonPath;

    tabBarButtonPath.addRoundedRect(tabOption->tabBarRect.adjusted(1, 0, -1, 0), radius, radius);

    painter->fillPath(tabBarButtonPath, Qt::gray);
    painter->drawPath( path );
    painter->fillPath(selectedtabBarButtonPath, fill_color);
    painter->drawPath(selectedtabBarButtonPath);
    painter->setPen(fill_color);
    selectedTabRect.adjust(1, 0, -1, 0);
    painter->drawLine(selectedTabRect.bottomLeft(), selectedTabRect.bottomRight());

    return true;
}
}
