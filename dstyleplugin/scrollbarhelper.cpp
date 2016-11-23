#include "scrollbarhelper.h"
#include "commonhelper.h"
#include "paletteextended.h"
#include "style.h"
#include "painterhelper.h"
#include "geometryutils.h"

#include <QPainter>
#include <QStyleOption>
#include <QScrollBar>
#include <QDebug>

bool dstyle::ScrollBarHelper::drawScrollBarSliderControl(const QStyleOption *option, QPainter *painter, const QWidget *widget)
{
    Style *style = CommonHelper::widgetStyle(widget);
    if (!style) return false;

    const bool enabled(option->state & QStyle::State_Enabled);
    const bool mouseOver(option->state & QStyle::State_MouseOver);
    const bool hasFocus((option->state & QStyle::State_HasFocus ) && !( widget && widget->focusProxy()));
    const bool sunken(option->state & (QStyle::State_On|QStyle::State_Sunken));

    painter->setRenderHint(QPainter::Antialiasing);

    qreal radius( GeometryUtils::frameRadius() );
    const QBrush &background = style->m_palette->brush(PaletteExtended::ScrollBar_HandleBrush, enabled, mouseOver, hasFocus, sunken, false, Qt::lightGray);
    const QBrush &border = style->m_palette->brush(PaletteExtended::ScrollBar_HandleBorderBrush, enabled, mouseOver, hasFocus, sunken, false, background);
    PainterHelper::drawRoundedRect(painter, option->rect, radius, radius, Qt::AbsoluteSize, background, Metrics::Painter_PenWidth, border);

    return true;
}
