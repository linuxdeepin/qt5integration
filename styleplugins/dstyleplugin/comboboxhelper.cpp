/*
 * SPDX-FileCopyrightText: 2016 - 2022 UnionTech Software Technology Co., Ltd.  
 * SPDX-License-Identifier: GPL-3.0-or-later
 */
#include "style.h"

#include <QPainter>
#include <QStyleOption>
#include <QComboBox>
#include <QStyleOptionComboBox>
#include <QDebug>

#include "paletteextended.h"
#include "geometryutils.h"

namespace dstyle {
extern PaletteExtended::PseudoClassType lineEditStateToPseudoClassType(QStyle::State state);

void Style::drawComboBoxPopupFramePrimitive(const QStyleOption *option, QPainter *painter) const {
    const QRect rect( option->rect );

    painter->setRenderHints(painter->renderHints() | QPainter::Antialiasing);

    painter->setPen(m_palette->brush(PaletteExtended::Menu_BorderColor).color());
    painter->setBrush(option->palette.brush(QPalette::Background));

    painter->drawRoundedRect(rect, 4, 4);
}

bool Style::drawComboBox(const QStyleOptionComplex *option, QPainter *painter, const QWidget *widget) const
{
    // cast option and check
    const QStyleOptionComboBox* comboBoxOption( qstyleoption_cast<const QStyleOptionComboBox*>( option ) );
    if( !comboBoxOption ) return true;

    // rect and palette
    const QRect& rect( option->rect );
    const QPalette& palette( option->palette );
    Q_UNUSED(palette)

    // state
    const bool editable( comboBoxOption->editable );
    bool flat( !comboBoxOption->frame );

    // frame
    if( option->subControls & SC_ComboBoxFrame )
    {

        if( editable ) {
            /*flat |= ( rect.height() <= 2*Metrics::Frame_FrameWidth + Metrics::MenuButton_IndicatorWidth );

            if ( flat ) {
                const QColor background( palette.color( QPalette::Base ) );

                painter->setBrush( background );
                painter->setPen( Qt::NoPen );
                painter->drawRect( rect );
            } else*/ {
                proxy()->drawPrimitive( PE_FrameLineEdit, option, painter, widget );
            }

        } else {
            if( flat ) {

            } else {
                // TODO(hualet): update button color from palette in case button is default
                const QColor shadow( Qt::transparent );
                const QBrush outline(m_palette->brush(PaletteExtended::PushButton_BorderBrush, option));
                const QBrush background(m_palette->brush(PaletteExtended::PushButton_BackgroundBrush, option));

                // render
                drawPushButtonFrame(painter, rect, QBrush(background), QBrush(outline), shadow);
            }
        }
    }

    if (option->subControls & SC_ComboBoxArrow) {
        QBrush brush = m_palette->brush(PaletteExtended::HeaderView_ArrowIndicatorDownIcon);

        if (brush.style() != Qt::TexturePattern)
            return false;

        // arrow rect
        QRect arrowRect( proxy()->subControlRect( CC_ComboBox, option, SC_ComboBoxArrow, widget ) );

        fillBrush(painter, arrowRect, brush);

        if (editable) {
            painter->setPen(QPen(m_palette->brush(PaletteExtended::LineEdit_BorderBrush, lineEditStateToPseudoClassType(option->state)), 1));
            painter->drawLine(arrowRect.topLeft() - QPoint(1, 1), arrowRect.bottomLeft() + QPoint(-1, 1));
        }
    }

    return option->subControls & (SC_ComboBoxFrame | SC_ComboBoxArrow);
}

QRect Style::comboBoxSubControlRect(const QStyleOptionComplex *opt, QStyle::SubControl sc, const QWidget *widget) const
{
    Q_UNUSED(widget);

    if (const QStyleOptionComboBox *cb = qstyleoption_cast<const QStyleOptionComboBox *>(opt)) {
        QRect ret;

        int x = cb->rect.x(),
                y = cb->rect.y(),
                wi = cb->rect.width(),
                he = cb->rect.height();
        int xpos = x;
        int margin = cb->frame ? 3 : 0;
        int bmarg = cb->frame ? 2 : 0;
        xpos += wi - bmarg - 16;

        switch (sc) {
        case SC_ComboBoxFrame:
            ret = cb->rect;
            break;
        case SC_ComboBoxArrow:
            ret.setRect(xpos, y + bmarg, 16, he - 2*bmarg);
            break;
        case SC_ComboBoxEditField:
            ret.setRect(x + margin, y + margin, wi - 2 * margin - 16, he - 2 * margin);
            break;
        case SC_ComboBoxListBoxPopup:
            ret = cb->rect;
            break;
        default:
            break;
        }

        return visualRect(cb->direction, cb->rect, ret);
    }

    return opt->rect;
}

bool Style::drawComboBoxLabelControl(const QStyleOption *option, QPainter *painter, const QWidget *widget) const
{
    // cast option and check
    const QStyleOptionComboBox* cb( qstyleoption_cast<const QStyleOptionComboBox*>( option ) );
    if( !cb ) return false;

    // copy rect and palette
    const QRect& rect( option->rect.adjusted(0, 0, -20, 0) );

    // state
    const QStyle::State& state( option->state );
    const bool enabled( state & QStyle::State_Enabled );
    const bool sunken( state & (QStyle::State_On | QStyle::State_Sunken) );
    const bool mouseOver(option->state & QStyle::State_MouseOver);
    const bool hasFocus(option->state & QStyle::State_HasFocus);
    const bool flat(!cb->frame);
    const bool editable(cb->editable);

    // content
    const bool hasText( !cb->currentText.isEmpty() );
    const bool hasIcon( !cb->currentIcon.isNull() );

    // contents
    QRect contentsRect( rect );
    if( sunken && !flat ) contentsRect.translate( 1, 1 );
    contentsRect.adjust(Metrics::Layout_ChildMarginWidth, 0, -Metrics::Layout_ChildMarginWidth, 0);

    // icon size
    QSize iconSize;
    if( hasIcon )
    {
        iconSize = cb->iconSize;
        if( !iconSize.isValid() )
        {
            const int metric( widget->style()->pixelMetric( QStyle::PM_SmallIconSize, option, widget ) );
            iconSize = QSize( metric, metric );
        }
    }

    // text size
    int textFlags( Qt::AlignVCenter | Qt::AlignLeft );
    const QSize textSize( option->fontMetrics.size( textFlags, cb->currentText ) );

    if (styleHint(SH_UnderlineShortcut, cb, widget))
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
        const int contentLeftPadding = flat ? (contentsRect.width() - contentsWidth )/2 : 0;
        iconRect = QRect( QPoint( contentsRect.left() + contentLeftPadding,
                                  contentsRect.top() + (contentsRect.height() - iconSize.height())/2 ), iconSize );
        textRect = QRect( QPoint( iconRect.right() + Metrics::Button_ItemSpacing + 1,
                                  contentsRect.top() + (contentsRect.height() - textSize.height())/2 ), textSize );

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

        const QPixmap pixmap = cb->currentIcon.pixmap( iconSize, iconMode, iconState );
        widget->style()->drawItemPixmap( painter, iconRect, Qt::AlignCenter, pixmap );

    }

    // render text
    if( hasText && textRect.isValid() && !editable) {
        painter->setPen(m_palette->brush(PaletteExtended::PushButton_TextColor, option).color());
        painter->drawText(textRect, textFlags, cb->currentText);
    }

    return true;
}

bool Style::widgetIsComboBoxPopupFramePrimitive(const QWidget *w)
{
    if (!w)
        return false;

    // combo box drop-list.
    // line edit completer drop-list
    return w->inherits("QComboBoxListView")
            || w->inherits("QComboBoxPrivateContainer")
            || (w->inherits("QAbstractItemView") && w->isTopLevel());
}

}
