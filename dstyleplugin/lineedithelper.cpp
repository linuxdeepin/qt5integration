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


namespace dstyle {

bool Style::drawFrameLineEditPrimitive(const QStyleOption *option, QPainter *painter, const QWidget *widget) const
{
    Q_UNUSED(widget)

    const QRect& rect( option->rect );

    // render
    const QBrush background(m_palette->brush(PaletteExtended::LineEdit_BackgroundBrush, option) );
    QBrush outline(m_palette->brush(PaletteExtended::LineEdit_BorderBrush, option) );

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
