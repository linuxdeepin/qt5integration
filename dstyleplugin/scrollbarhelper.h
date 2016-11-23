#ifndef SCROLLBARHELPER_H
#define SCROLLBARHELPER_H

#include <QBrush>

QT_BEGIN_NAMESPACE
class QStyleOption;
class QPainter;
class QWidget;
class QStyleOptionComplex;
QT_END_NAMESPACE

namespace dstyle {

class PaletteExtended;
class ScrollBarHelper
{
public:
    static bool drawScrollBarSliderControl(const QStyleOption *option, QPainter *painter, const QWidget *widget);
};

} // end namespace dstyle

#endif // SCROLLBARHELPER_H
