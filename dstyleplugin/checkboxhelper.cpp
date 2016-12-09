#include "style.h"
#include "paletteextended.h"

#include <QPainter>
#include <qdrawutil.h>
#include <QDebug>

namespace dstyle {
bool Style::drawIndicatorCheckBoxPrimitive(QStyle::PrimitiveElement element, const QStyleOption *opt, QPainter *p, const QWidget *widget) const
{
    Q_UNUSED(element)
    Q_UNUSED(widget)

    fillBrush(p, opt->rect, m_palette->brush(PaletteExtended::CheckBox_BackgroundBrush, opt));

    return true;
}
}
