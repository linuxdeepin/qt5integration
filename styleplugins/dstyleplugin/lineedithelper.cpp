/*
 * SPDX-FileCopyrightText: 2017 - 2022 UnionTech Software Technology Co., Ltd.  
 * SPDX-License-Identifier: GPL-3.0-or-later
 */
#include "commonhelper.h"
#include "common.h"
#include "style.h"
#include "paletteextended.h"

#include <dtkwidget_config.h>
#ifdef DTKWIDGET_CLASS_DStyleOptionLineEdit
#include <DStyleOptionLineEdit>
#include <DLineEdit>

DWIDGET_USE_NAMESPACE
#endif
#include <QDebug>


namespace dstyle {

PaletteExtended::PseudoClassType lineEditStateToPseudoClassType(QStyle::State state)
{
    if (!state.testFlag(QStyle::State_Enabled)) {
        return PaletteExtended::PseudoClass_Disabled;
    }

    if (state.testFlag(QStyle::State_ReadOnly)) {
        return PaletteExtended::PseudoClass_ReadOnly;
    }

    if (state.testFlag(QStyle::State_HasFocus)) {
        return PaletteExtended::PseudoClass_Focus;
    }

    return PaletteExtended::PseudoClass_Unspecified;
}

bool Style::drawFrameLineEditPrimitive(const QStyleOption *option, QPainter *painter, const QWidget *widget) const
{
    Q_UNUSED(widget)

    const QRect& rect( option->rect );

    // render
    const QBrush background(m_palette->brush(PaletteExtended::LineEdit_BackgroundBrush, lineEditStateToPseudoClassType(option->state)) );
    QBrush outline(m_palette->brush(PaletteExtended::LineEdit_BorderBrush, lineEditStateToPseudoClassType(option->state)) );

#ifdef DTKWIDGET_CLASS_DLineEdit
    if (const DLineEdit *edit = qobject_cast<const DLineEdit*>(widget)) {
        DStyleOptionLineEdit edit_option;
        edit_option.init(const_cast<DLineEdit*>(edit));

        if (edit_option.features.testFlag(DStyleOptionLineEdit::Alert))
            outline = m_palette->brush(PaletteExtended::LineEdit_AlertBorderBrush, option, PaletteExtended::PseudoClass_Unknown, outline);

        CommonHelper::renderFrame( painter, rect, background, outline );

        if (edit_option.features.testFlag(DStyleOptionLineEdit::IconButton)) {
            painter->save();
            painter->setPen(QPen(outline, Metrics::Painter_PenWidth));
            painter->drawLine(QPointF(edit_option.iconButtonRect.topLeft()) - QPointF(0.5, 0), QPointF(edit_option.iconButtonRect.bottomLeft()) + QPoint(-0.5, 1));
            painter->restore();
        }
    } else
#endif
    {
        CommonHelper::renderFrame( painter, rect, background, outline );
    }

    return true;
}
}// end namespace dstyle
