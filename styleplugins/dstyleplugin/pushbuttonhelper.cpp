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

#include "common.h"
#include "geometryutils.h"
#include "style.h"
#include "commonhelper.h"
#include "painterhelper.h"
#include "paletteextended.h"

#include <dtkwidget_global.h>
#ifdef DTKWIDGET_CLASS_DSuggestButton
#include <DStyleOption>
#include <DSuggestButton>
#endif

#include <QDebug>
#include <QStyleOptionButton>
#include <QPainterPath>

DWIDGET_USE_NAMESPACE

namespace dstyle {

bool Style::drawPushButtonBevel(const QStyleOption *option, QPainter *painter, const QWidget *widget) const
{
    // cast option and check
    const QStyleOptionButton* buttonOption( qstyleoption_cast< const QStyleOptionButton* >( option ) );
    if( !buttonOption ) return true;

    // rect and palette
    const QRect& rect( option->rect );

    // button state
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
        const QBrush outline(
#ifdef DTKWIDGET_CLASS_DSuggestButton
                    (buttonOption->features & DStyleOptionButton::SuggestButton) ?
                        m_palette->brush(PaletteExtended::PushButton_SuggestButtonColor, option) :
                        m_palette->brush(PaletteExtended::PushButton_BorderBrush, option)
#else
                    m_palette->brush(PaletteExtended::PushButton_BorderBrush, option)
#endif
                    );
        const QBrush background(m_palette->brush(PaletteExtended::PushButton_BackgroundBrush, option));

        // render
        drawPushButtonFrame(painter, rect, background, outline, shadow, widget);
    }

    if (buttonOption->features & QStyleOptionButton::HasMenu) {
        int mbi = proxy()->pixelMetric(PM_MenuButtonIndicator, buttonOption, widget);
        QRect ir = buttonOption->rect;
        QStyleOptionButton newBtn = *buttonOption;
        newBtn.rect = QRect(ir.right() - mbi + 2, ir.height()/2 - mbi/2 + 3, mbi - 6, mbi - 6);
        proxy()->drawPrimitive(PE_IndicatorArrowDown, &newBtn, painter, widget);
    }

    return true;
}

bool Style::drawPushButtonLabel(const QStyleOption *option, QPainter *painter, const QWidget *widget) const
{
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
    const bool suggest(
#ifdef DTKWIDGET_CLASS_DSuggestButton
                buttonOption->features & DStyleOptionButton::SuggestButton
#else
                false
#endif
                );

    // content
    const bool hasText( !buttonOption->text.isEmpty() );
    const bool hasIcon( ( flat || !hasText ) && !buttonOption->icon.isNull() );

    // contents
    QRect contentsRect( rect );

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
            const int metric( proxy()->pixelMetric( QStyle::PM_SmallIconSize, option, widget ) );
            iconSize = QSize( metric, metric );
        }
    }

    // text size
    int textFlags( Qt::AlignCenter );
    const QSize textSize( option->fontMetrics.size( textFlags, buttonOption->text ) );

    if (styleHint(SH_UnderlineShortcut, buttonOption, widget))
        textFlags |= Qt::TextShowMnemonic;
    else
        textFlags |= Qt::TextHideMnemonic;

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
    if( iconRect.isValid() ) iconRect = proxy()->visualRect( option->direction, option->rect, iconRect );
    if( textRect.isValid() ) textRect = proxy()->visualRect( option->direction, option->rect, textRect );

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

        buttonOption->icon.paint(painter, iconRect, Qt::AlignCenter, iconMode, iconState);
    }

    // render text
    if( hasText && textRect.isValid() ) {
        if (suggest) {
            painter->setPen(m_palette->brush(PaletteExtended::PushButton_SuggestTextColor, option).color());
        } else {
            painter->setPen(m_palette->brush(PaletteExtended::PushButton_TextColor, option).color());
        }

        if (buttonOption->features & QStyleOptionButton::HasMenu)
            textRect = textRect.adjusted(0, 0, -proxy()->pixelMetric(PM_MenuButtonIndicator, buttonOption, widget), 0);
        painter->drawText(textRect, textFlags, buttonOption->text);
    }

    return true;
}

bool Style::drawPushButtonFrame(QPainter* painter, const QRect& rect, const QBrush& brush, const QBrush& outline, const QColor& shadow, const QWidget *widget) const
{
    Q_UNUSED(shadow)

    // setup painter
    painter->setRenderHint( QPainter::Antialiasing, true );

    const qreal radius( GeometryUtils::frameRadius() );
    const QMarginsF margins(0.5, 0.5, 0.5, 0.5);

    QPainterPath path;
    path.addRoundedRect(QRectF(rect).marginsRemoved(margins), radius, radius);

    // ButtonTuples are QPushButton groups that needs to be taken care of,
    // the right corners of the LeftButton are not rounded, and the left
    // corners of the RightButton are not rounded either.
    if (widget) {
        const QString className = widget->metaObject()->className();
        const QRectF rf = QRectF(rect);
        if (className == "dcc::widgets::LeftButton") {
            QPainterPath rightHalf;
            rightHalf.addRect(rf.right() - radius, rf.y() + margins.top(), radius, rf.height() - margins.top() - margins.bottom());
            path = path.united(rightHalf);
        } else if (className == "dcc::widgets::RightButton") {
            QPainterPath leftHalf;
            leftHalf.addRect(rf.left(), rf.y() + margins.top(), radius, rf.height() - margins.top() - margins.bottom());
            path = path.united(leftHalf);
        }
    }

    PainterHelper::drawPath(painter, path, brush, Metrics::Painter_PenWidth, outline);

    return true;
}

}// end namespace dstyle
