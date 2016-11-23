#ifndef PAINTERHELPER_H
#define PAINTERHELPER_H

#include <QBrush>

QT_BEGIN_NAMESPACE
class QPainter;
class QPainterPath;
QT_END_NAMESPACE

namespace dstyle {
namespace PainterHelper {
    void drawRect(QPainter *painter, const QRectF &rect, const QBrush &background, qreal borderWidth = 0, const QBrush &border = Qt::NoBrush);
    void drawRoundedRect(QPainter *painter, const QRectF &rect, qreal xRadius, qreal yRadius, Qt::SizeMode mode,
                         const QBrush &background, qreal borderWidth = 0, const QBrush &border = Qt::NoBrush);
    void drawEllipse(QPainter *painter, const QRectF &rect, const QBrush &background, qreal borderWidth = 0, const QBrush &border = Qt::NoBrush);
    void drawPath(QPainter *painter, const QPainterPath &path, const QBrush &background, qreal borderWidth = 0, const QBrush &border = Qt::NoBrush);
} // end namespace PainterHelper
} // end namespace dtyle

#endif // PAINTERHELPER_H
