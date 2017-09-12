/**
 * Copyright (C) 2017 by Deepin Technology Co., Ltd.
 *
 * Use of this source code is governed by GPLv3
 * that can be found in the LICENSE file.
 **/

#include "style.h"
#include "paletteextended.h"

#include <QPainter>
#include <QApplication>
#include <QDebug>

namespace dstyle {
bool Style::drawSpinBoxComplexControl( const QStyleOptionComplex* option, QPainter* painter, const QWidget* widget ) const
{
    const QStyleOptionSpinBox *sb( qstyleoption_cast<const QStyleOptionSpinBox*>( option ) );
    if( !sb ) return true;

    // store palette and rect
    const QPalette& palette( option->palette );
    const QRect& rect( option->rect );


    if( option->subControls & SC_SpinBoxFrame )
    {

        // detect flat spinboxes
        bool flat( !sb->frame );
        flat |= ( rect.height() < 2*Metrics::Frame_FrameWidth + Metrics::SpinBox_ArrowButtonWidth );
        if( flat )
        {

            const QColor background( palette.color( QPalette::Base ) );

            painter->setBrush( background );
            painter->setPen( Qt::NoPen );
            painter->drawRect( rect );

        } else {

            drawPrimitive( PE_FrameLineEdit, option, painter, widget );

        }

    }

    QStyleOptionSpinBox copy = *sb;
    PrimitiveElement pe;
//    int fw = -Metrics::Painter_PenWidth;

    if (sb->subControls & SC_SpinBoxUp) {
        copy.subControls = SC_SpinBoxUp;
        if (!(sb->stepEnabled & QAbstractSpinBox::StepUpEnabled)) {
            copy.state &= ~State_Enabled;
        }

        if (sb->activeSubControls == SC_SpinBoxUp && (sb->state & State_Sunken)) {
            copy.state |= State_On;
            copy.state |= State_Sunken;
        } else {
            copy.state |= State_Raised;
            copy.state &= ~State_Sunken;
        }

        pe = (sb->buttonSymbols == QAbstractSpinBox::PlusMinus ? PE_IndicatorSpinPlus
              : PE_IndicatorSpinUp);

        copy.rect = proxy()->subControlRect(CC_SpinBox, sb, SC_SpinBoxUp, widget);
//        copy.rect.adjust(0, -fw, 0, fw);

//        fillBrush(painter, copy.rect, m_palette->brush(PaletteExtended::SpinBox_PlusBrush, &copy));
//        painter->setPen(QPen(m_palette->brush(PaletteExtended::SpinBox_BorderColor, option), Metrics::Painter_PenWidth));
//        painter->drawLine(copy.rect.topLeft(), copy.rect.bottomLeft());

        proxy()->drawPrimitive(pe, &copy, painter, widget);
    }

    if (sb->subControls & SC_SpinBoxDown) {
        copy.subControls = SC_SpinBoxDown;
        copy.state = sb->state;
        QPalette pal2 = sb->palette;
        if (!(sb->stepEnabled & QAbstractSpinBox::StepDownEnabled)) {
            pal2.setCurrentColorGroup(QPalette::Disabled);
            copy.state &= ~State_Enabled;
        }
        copy.palette = pal2;

        if (sb->activeSubControls == SC_SpinBoxDown && (sb->state & State_Sunken)) {
            copy.state |= State_On;
            copy.state |= State_Sunken;
        } else {
            copy.state |= State_Raised;
            copy.state &= ~State_Sunken;
        }
        pe = (sb->buttonSymbols == QAbstractSpinBox::PlusMinus ? PE_IndicatorSpinMinus
              : PE_IndicatorSpinDown);

        copy.rect = proxy()->subControlRect(CC_SpinBox, sb, SC_SpinBoxDown, widget);
//        copy.rect.adjust(0, -fw, 0, fw);

//        fillBrush(painter, copy.rect, m_palette->brush(PaletteExtended::SpinBox_MinusBrush, &copy));
//        painter->setPen(QPen(m_palette->brush(PaletteExtended::SpinBox_BorderColor, option), Metrics::Painter_PenWidth));
//        painter->drawLine(copy.rect.topLeft(), copy.rect.bottomLeft());

        proxy()->drawPrimitive(pe, &copy, painter, widget);
    }

    painter->setPen(QPen(m_palette->brush(PaletteExtended::SpinBox_BorderColor, option), Metrics::Painter_PenWidth));
    QRect lineEditRect = proxy()->subControlRect(CC_SpinBox, sb, SC_SpinBoxEditField, widget).adjusted(0, -1, 0, 1);
    painter->drawLine(lineEditRect.topRight(), lineEditRect.bottomRight());

    return true;
}

QRect Style::spinboxSubControlRect(const QStyleOptionComplex *opt, QStyle::SubControl sc, const QWidget *widget) const
{
    Q_UNUSED(widget)

    QRect ret;

    if (const QStyleOptionSpinBox *spinbox = qstyleoption_cast<const QStyleOptionSpinBox *>(opt)) {
        QSize bs;
        int fw = 0;
        bs.setHeight(spinbox->rect.height() - 2 * fw);
        // 1.6 -approximate golden mean
        bs.setWidth(bs.height());
        bs = bs.expandedTo(QApplication::globalStrut());
        int y = fw;
        int x, lx, rx;
        x = spinbox->rect.right() - fw - bs.width();
        lx = fw;
        rx = x - fw - bs.width();
        switch (sc) {
        case SC_SpinBoxUp:
            if (spinbox->buttonSymbols == QAbstractSpinBox::NoButtons)
                return QRect();
            ret = QRect(x, y, bs.width(), bs.height());
            break;
        case SC_SpinBoxDown:
            if (spinbox->buttonSymbols == QAbstractSpinBox::NoButtons)
                return QRect();

            ret = QRect(x - bs.width(), y, bs.width(), bs.height());
            break;
        case SC_SpinBoxEditField:
            if (spinbox->buttonSymbols == QAbstractSpinBox::NoButtons) {
                ret = QRect(lx, fw, spinbox->rect.width() - 2*fw, spinbox->rect.height() - 2*fw);
            } else {
                ret = QRect(lx, fw, rx, spinbox->rect.height() - 2*fw);
            }
            break;
        case SC_SpinBoxFrame:
            ret = spinbox->rect;
        default:
            break;
        }
        ret = visualRect(spinbox->direction, spinbox->rect, ret);
    }

    return ret;
}
}
