/*
 * SPDX-FileCopyrightText: 2017 - 2022 UnionTech Software Technology Co., Ltd.  
 * SPDX-License-Identifier: LGPL-3.0-or-later
 */
#include "style.h"
#include "paletteextended.h"

#include <QPainter>
#include <QDebug>

namespace dstyle {
//* centering
QRect centerRect(const QRect &rect, int width, int height)
{ return QRect(rect.left() + (rect.width() - width)/2, rect.top() + (rect.height() - height)/2, width, height); }

QRect centerRect(const QRect &rect, const QSize& size )
{ return centerRect( rect, size.width(), size.height() ); }

bool Style::drawToolButtonLabelControl( const QStyleOption* option, QPainter* painter, const QWidget* widget ) const
{
    if (isTabBarToolButton(widget)) {
        return drawScrollButtonLabelControl(option, painter, widget);
    }

    // cast option and check
    const QStyleOptionToolButton* toolButtonOption( qstyleoption_cast<const QStyleOptionToolButton*>(option) );

    // copy rect and palette
    const QRect& rect = option->rect;
    const QPalette& palette = option->palette;

    // state
    const State& state( option->state );
    const bool enabled( state & State_Enabled );
    const bool sunken( state & (State_On | State_Sunken) );
    const bool mouseOver( enabled && (option->state & State_MouseOver) );
    const bool flat( state & State_AutoRaise );

    // focus flag is set to match the background color in either renderButtonFrame or renderToolButtonFrame
    bool hasFocus( false );
    if( flat ) hasFocus = enabled && !mouseOver && (option->state & State_HasFocus);
    else hasFocus = enabled && !mouseOver && (option->state & (State_HasFocus|State_Sunken) );

    const bool hasArrow( toolButtonOption->features & QStyleOptionToolButton::Arrow );
    const bool hasIcon( !( hasArrow || toolButtonOption->icon.isNull() ) );
    const bool hasText( !toolButtonOption->text.isEmpty() );

    // contents
    QRect contentsRect( rect );
    if( sunken && !flat ) contentsRect.translate( 1, 1 );

    // icon size
    const QSize iconSize( toolButtonOption->iconSize );

    // text size
    int textFlags( Qt::TextHideMnemonic );
    const QSize textSize( option->fontMetrics.size( textFlags, toolButtonOption->text ) );

    // adjust text and icon rect based on options
    QRect iconRect;
    QRect textRect;

    if( hasText && ( !(hasArrow||hasIcon) || toolButtonOption->toolButtonStyle == Qt::ToolButtonTextOnly ) )
    {

        // text only
        textRect = contentsRect;
        textFlags |= Qt::AlignCenter;

    } else if( (hasArrow||hasIcon) && (!hasText || toolButtonOption->toolButtonStyle == Qt::ToolButtonIconOnly ) ) {

        // icon only
        iconRect = contentsRect;

    } else if( toolButtonOption->toolButtonStyle == Qt::ToolButtonTextUnderIcon ) {

        const int contentsHeight( iconSize.height() + textSize.height() + Metrics::ToolButton_ItemSpacing );
        iconRect = QRect( QPoint( contentsRect.left() + (contentsRect.width() - iconSize.width())/2, contentsRect.top() + (contentsRect.height() - contentsHeight)/2 ), iconSize );
        textRect = QRect( QPoint( contentsRect.left() + (contentsRect.width() - textSize.width())/2, iconRect.bottom() + Metrics::ToolButton_ItemSpacing + 1 ), textSize );
        textFlags |= Qt::AlignCenter;

    } else {
        const int contentsWidth( iconSize.width() + textSize.width() + Metrics::ToolButton_ItemSpacing );
        iconRect = QRect( QPoint( contentsRect.left() + (contentsRect.width() - contentsWidth )/2, contentsRect.top() + (contentsRect.height() - iconSize.height())/2 ), iconSize );

        textRect = QRect( QPoint( iconRect.right() + Metrics::ToolButton_ItemSpacing + 1, contentsRect.top() + (contentsRect.height() - textSize.height())/2 ), textSize );

        // handle right to left layouts
        iconRect = visualRect( option->direction, option->rect, iconRect );
        textRect = visualRect( option->direction, option->rect, textRect );

        textFlags |= Qt::AlignLeft | Qt::AlignVCenter;

    }

    // make sure there is enough room for icon
    if( iconRect.isValid() ) iconRect = centerRect( iconRect, iconSize );

    // render arrow or icon
    if( hasArrow && iconRect.isValid() )
    {

        QStyleOptionToolButton copy( *toolButtonOption );
        copy.rect = iconRect;
        switch( toolButtonOption->arrowType )
        {
        case Qt::LeftArrow: drawPrimitive( PE_IndicatorArrowLeft, &copy, painter, widget ); break;
        case Qt::RightArrow: drawPrimitive( PE_IndicatorArrowRight, &copy, painter, widget ); break;
        case Qt::UpArrow: drawPrimitive( PE_IndicatorArrowUp, &copy, painter, widget ); break;
        case Qt::DownArrow: drawPrimitive( PE_IndicatorArrowDown, &copy, painter, widget ); break;
        default: break;
        }

    } else if( hasIcon && iconRect.isValid() ) {

        // icon state and mode
        const QIcon::State iconState( sunken ? QIcon::On : QIcon::Off );
        QIcon::Mode iconMode;
        if( !enabled ) iconMode = QIcon::Disabled;
        else if( !flat && hasFocus ) iconMode = QIcon::Selected;
        else if( mouseOver && flat ) iconMode = QIcon::Active;
        else iconMode = QIcon::Normal;

        const QPixmap pixmap = toolButtonOption->icon.pixmap( iconSize, iconMode, iconState );
        drawItemPixmap( painter, iconRect, Qt::AlignCenter, pixmap );

    }

    // render text
    if( hasText && textRect.isValid() )
    {

        QPalette::ColorRole textRole( QPalette::ButtonText );
        if( flat ) textRole = (hasFocus&&sunken&&!mouseOver) ? QPalette::HighlightedText: QPalette::WindowText;
        else if( mouseOver ) textRole = QPalette::HighlightedText;

        painter->setFont(toolButtonOption->font);
        drawItemText( painter, textRect, textFlags, palette, enabled, toolButtonOption->text, textRole );
    }

    return true;
}

bool Style::drawPanelButtonToolPrimitive(const QStyleOption *option, QPainter *painter, const QWidget *widget) const
{
    if (isTabBarToolButton(widget)) {
        return drawScrollButtonPrimitive(option, painter, widget);
    }

    // rect and palette
    const QRect& rect( option->rect );

    // update animation state
    // mouse over takes precedence over focus
    // TODO(hualet): update button color from palette in case button is default
    const QColor shadow( Qt::transparent );
    const QBrush outline(m_palette->brush(PaletteExtended::PushButton_BorderBrush, option));
    const QBrush background(m_palette->brush(PaletteExtended::PushButton_BackgroundBrush, option));

    // render
    drawPushButtonFrame(painter, rect, background, outline, shadow, widget);

//    if (buttonOption->features & QStyleOptionButton::HasMenu) {
//        int mbi = proxy()->pixelMetric(PM_MenuButtonIndicator, buttonOption, widget);
//        QRect ir = buttonOption->rect;
//        QStyleOption newBtn = *option;
//        newBtn.rect = QRect(ir.right() - mbi + 2, ir.height()/2 - mbi/2 + 3, mbi - 6, mbi - 6);
//        proxy()->drawPrimitive(PE_IndicatorArrowDown, &newBtn, painter, widget);
//    }

    return true;
}
}
