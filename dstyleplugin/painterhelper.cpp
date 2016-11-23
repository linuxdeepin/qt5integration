#include "painterhelper.h"

#include <QPainter>
#include <QDebug>

namespace dstyle {
namespace PainterHelper {
void drawRect(QPainter *painter, const QBrush &background, const QRectF &rect, qreal borderWidth, const QBrush &border)
{
    QPainterPath path;

    path.addRect(rect);
    drawPath(painter, path, background, borderWidth, border);
}

void drawRoundedRect(QPainter *painter, const QRectF &rect, qreal xRadius, qreal yRadius, Qt::SizeMode mode, const QBrush &background, qreal borderWidth, const QBrush &border)
{
    QPainterPath path;

    path.addRoundedRect(rect, xRadius, yRadius, mode);
    drawPath(painter, path, background, borderWidth, border);
}

void drawEllipse(QPainter *painter, const QRectF &rect, const QBrush &background, qreal borderWidth, const QBrush &border)
{
    QPainterPath path;

    path.addEllipse(rect);
    drawPath(painter, path, background, borderWidth, border);
}

void drawPath(QPainter *painter, const QPainterPath &path, const QBrush &background, qreal borderWidth, const QBrush &border)
{
    painter->fillPath(path, background);

    if (qFuzzyIsNull(borderWidth) || background == border)
        return;

    painter->save();
    painter->setBrush(background);
    painter->setPen(QPen(border, borderWidth));
    painter->drawPath(path);
    painter->restore();
}

} // end namespace PainterHelper
} // end namespace dtyle

