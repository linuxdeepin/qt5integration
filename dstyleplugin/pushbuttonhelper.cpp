/**
 * Copyright (C) 2016 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#include "pushbuttonhelper.h"
#include "common.h"
#include "geometryutils.h"
#include "style.h"
#include "commonhelper.h"

#include <QDebug>
#include <QStyleOptionButton>

namespace dstyle {

bool PushButtonHelper::drawPushButtonBevel(const QStyleOption *option, QPainter *painter, const QWidget *widget)
{
    Style *style = CommonHelper::widgetStyle(widget);
    if (!style) return false;

    // cast option and check
    const QStyleOptionButton* buttonOption( qstyleoption_cast< const QStyleOptionButton* >( option ) );
    if( !buttonOption ) return true;

    // rect and palette
    const QRect& rect( option->rect );

    // button state
    const QStyle::State& state( option->state );
    const bool enabled( state & QStyle::State_Enabled );
    const bool mouseOver(state & QStyle::State_MouseOver);
    const bool hasFocus((state & QStyle::State_HasFocus ) && !( widget && widget->focusProxy()));
    const bool sunken( state & ( QStyle::State_On|QStyle::State_Sunken ) );
    const bool flat( buttonOption->features & QStyleOptionButton::Flat );

    // update animation state
    // mouse over takes precedence over focus
    if( flat )
    {
        // just draw text on flat button.
        // const QColor color( getFlatButtonColor(enabled, mouseOver, hasFocus, sunken) );
        // drawFlatButtonFrame( painter, rect, color, sunken );
    } else {
        // TODO(hualet): update button color from palette in case button is default
        const QColor shadow( Qt::transparent );
        const QBrush outline( getButtonBorderBrush(style->m_palette, enabled, mouseOver, hasFocus, sunken) );
        const QBrush background( getButtonBackgroundBrush(style->m_palette, enabled, mouseOver, hasFocus, sunken) );

        // render
        drawPushButtonFrame(painter, rect, background, outline, shadow );
    }

    return true;
}

bool PushButtonHelper::drawPushButtonLabel(const QStyleOption *option, QPainter *painter, const QWidget *widget)
{
    Style *style = CommonHelper::widgetStyle(widget);
    if (!style) return false;

    // cast option and check
    const QStyleOptionButton* buttonOption( qstyleoption_cast<const QStyleOptionButton*>( option ) );
    if( !buttonOption ) return true;

    // copy rect and palette
    const QRect& rect( option->rect );

    // state
    const QStyle::State& state( option->state );
    const bool enabled( state & QStyle::State_Enabled );
    const bool sunken( state & (QStyle::State_On | QStyle::State_Sunken) );
    const bool mouseOver(option->state & QStyle::State_MouseOver);
    const bool hasFocus(option->state & QStyle::State_HasFocus);
    const bool flat( buttonOption->features & QStyleOptionButton::Flat );

    // content
    const bool hasText( !buttonOption->text.isEmpty() );
    const bool hasIcon( ( flat || !hasText ) && !buttonOption->icon.isNull() );

    // contents
    QRect contentsRect( rect );
    if( sunken && !flat ) contentsRect.translate( 1, 1 );

    // menu arrow
    /*
    if( buttonOption->features & QStyleOptionButton::HasMenu )
    {

        // define rect
        QRect arrowRect( contentsRect );
        arrowRect.setLeft( contentsRect.right() - Metrics::MenuButton_IndicatorWidth + 1 );
        arrowRect = GeometryUtils::centerRect( arrowRect, Metrics::MenuButton_IndicatorWidth, Metrics::MenuButton_IndicatorWidth );

        contentsRect.setRight( arrowRect.left() - Metrics::Button_ItemSpacing - 1  );
        contentsRect.adjust( Metrics::Button_MarginWidth, 0, 0, 0 );

        arrowRect = widget->style()->visualRect( option->direction, option->rect, arrowRect );

        // define color
        const QColor arrowColor( _helper->arrowColor( palette, textRole ) );
        _helper->renderArrow( painter, arrowRect, arrowColor, ArrowDown );

    }
    */

    // icon size
    QSize iconSize;
    if( hasIcon )
    {
        iconSize = buttonOption->iconSize;
        if( !iconSize.isValid() )
        {
            const int metric( widget->style()->pixelMetric( QStyle::PM_SmallIconSize, option, widget ) );
            iconSize = QSize( metric, metric );
        }
    }

    // text size
    const int textFlags( Qt::AlignCenter );
    const QSize textSize( option->fontMetrics.size( textFlags, buttonOption->text ) );

    // adjust text and icon rect based on options
    QRect iconRect;
    QRect textRect;

    if( hasText && !hasIcon ) textRect = contentsRect;
    else if( hasIcon && !hasText ) iconRect = contentsRect;
    else {

        const int contentsWidth( iconSize.width() + textSize.width() + Metrics::Button_ItemSpacing );
        iconRect = QRect( QPoint( contentsRect.left() + (contentsRect.width() - contentsWidth )/2, contentsRect.top() + (contentsRect.height() - iconSize.height())/2 ), iconSize );
        textRect = QRect( QPoint( iconRect.right() + Metrics::ToolButton_ItemSpacing + 1, contentsRect.top() + (contentsRect.height() - textSize.height())/2 ), textSize );

    }

    // handle right to left
    if( iconRect.isValid() ) iconRect = widget->style()->visualRect( option->direction, option->rect, iconRect );
    if( textRect.isValid() ) textRect = widget->style()->visualRect( option->direction, option->rect, textRect );

    // make sure there is enough room for icon
    if( iconRect.isValid() ) iconRect = GeometryUtils::centerRect( iconRect, iconSize );

    // render icon
    if( hasIcon && iconRect.isValid() ) {

        // icon state and mode
        const QIcon::State iconState( sunken ? QIcon::On : QIcon::Off );
        QIcon::Mode iconMode;
        if( !enabled ) iconMode = QIcon::Disabled;
        else if( !flat && hasFocus ) iconMode = QIcon::Selected;
        else if( mouseOver && flat ) iconMode = QIcon::Active;
        else iconMode = QIcon::Normal;

        const QPixmap pixmap = buttonOption->icon.pixmap( iconSize, iconMode, iconState );
        widget->style()->drawItemPixmap( painter, iconRect, Qt::AlignCenter, pixmap );

    }

    // render text
    if( hasText && textRect.isValid() ) {
        painter->setPen(getButtonTextColor(style->m_palette, flat, enabled, mouseOver, hasFocus, sunken));
        painter->drawText(textRect, textFlags, buttonOption->text);
    }

    return true;
}

bool PushButtonHelper::drawFlatButtonFrame( QPainter* painter, const QRect& rect, const QBrush& brush, bool sunken )
{
    // do nothing for invalid color
    if( brush.style() == Qt::NoBrush ) return false;

    // setup painter
    painter->setRenderHints( QPainter::Antialiasing );

    const QRectF baseRect( rect );

    if( sunken )
    {
        const qreal radius( GeometryUtils::frameRadius() );

        painter->setPen( Qt::NoPen );
        painter->setBrush( brush );

        const QRectF contentRect( baseRect.adjusted( 1, 1, -1, -1 ) );
        painter->drawRoundedRect( contentRect, radius, radius );

    } else {
        const qreal radius( GeometryUtils::frameRadius(-0.5) );

        painter->setPen( QPen(brush, Metrics::Painter_PenWidth) );
        painter->setBrush( Qt::NoBrush );
        const QRectF outlineRect( baseRect.adjusted( 1.5, 1.5, -1.5, -1.5 ) );
        painter->drawRoundedRect( outlineRect, radius, radius );

    }

    return true;
}

bool PushButtonHelper::drawPushButtonFrame( QPainter* painter, const QRect& rect, const QBrush& brush, const QBrush& outline, const QColor& shadow )
{
    Q_UNUSED(shadow)

    // setup painter
    painter->setRenderHint( QPainter::Antialiasing, true );

    // copy rect
    QRectF frameRect( rect );
    qreal radius( GeometryUtils::frameRadius() );

    painter->setBrush(brush);
    painter->setPen(QPen(outline, Metrics::Painter_PenWidth));

    // render
    painter->drawRoundedRect( frameRect, radius, radius );

    return true;
}

QBrush PushButtonHelper::getButtonBorderBrush(PaletteExtended *plExt, bool enabled, bool mouseOver, bool hasFocus, bool sunken)
{
    const QBrush &normal = plExt->brush(PaletteExtended::PushButton_BorderColor);

    if (!enabled)
        return plExt->brush(PaletteExtended::PushButton_BorderColor, PaletteExtended::PseudoClass_Disabled, normal);
    else if (sunken)
        return plExt->brush(PaletteExtended::PushButton_BorderColor, PaletteExtended::PseudoClass_Pressed, normal);
    else if (mouseOver)
        return plExt->brush(PaletteExtended::PushButton_BorderColor, PaletteExtended::PseudoClass_Hover, normal);
    else if (hasFocus)
        return plExt->brush(PaletteExtended::PushButton_BorderColor, PaletteExtended::PseudoClass_Focus, normal);

    return normal;
}

QBrush PushButtonHelper::getButtonBackgroundBrush(PaletteExtended *plExt, bool enabled, bool mouseOver, bool hasFocus, bool sunken)
{
    const QBrush &normal = plExt->brush(PaletteExtended::PushButton_BackgroundColor);

    if (!enabled)
        return plExt->brush(PaletteExtended::PushButton_BackgroundColor, PaletteExtended::PseudoClass_Disabled, normal);
    else if (sunken)
        return plExt->brush(PaletteExtended::PushButton_BackgroundColor, PaletteExtended::PseudoClass_Pressed, normal);
    else if (mouseOver)
            return plExt->brush(PaletteExtended::PushButton_BackgroundColor, PaletteExtended::PseudoClass_Hover, normal);
    else if (hasFocus)
        return plExt->brush(PaletteExtended::PushButton_BackgroundColor, PaletteExtended::PseudoClass_Focus, normal);

    return normal;
}

QColor PushButtonHelper::getButtonTextColor(PaletteExtended *plExt, bool flat, bool enabled, bool mouseOver, bool hasFocus, bool sunken)
{
    PaletteExtended::PseudoClassType extraType = flat ? PaletteExtended::PseudoClass_Flat : PaletteExtended::PseudoClass_Unknown;
    const QBrush &normal = plExt->brush(PaletteExtended::PushButton_TextColor, flat ? extraType : PaletteExtended::PseudoClass_Unspecified,
                                        plExt->brush(PaletteExtended::PushButton_TextColor));

    if (!enabled)
        return plExt->brush(PaletteExtended::PushButton_TextColor, PaletteExtended::PseudoClass_Disabled | extraType, normal).color();
    else if (sunken)
        return plExt->brush(PaletteExtended::PushButton_TextColor, PaletteExtended::PseudoClass_Pressed | extraType, normal).color();
    else if (mouseOver)
        return plExt->brush(PaletteExtended::PushButton_TextColor, PaletteExtended::PseudoClass_Hover | extraType, normal).color();
    else if (hasFocus)
        return plExt->brush(PaletteExtended::PushButton_TextColor, PaletteExtended::PseudoClass_Focus | extraType, normal).color();

    return normal.color();
}

}
