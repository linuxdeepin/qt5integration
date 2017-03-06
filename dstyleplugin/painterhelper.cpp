#include "painterhelper.h"

#include <QPainter>
#include <QDebug>

namespace dstyle {
namespace PainterHelper {
void drawRect(QPainter *painter, const QRectF &rect, const QBrush &background, qreal borderWidth, const QBrush &border)
{
    painter->fillRect(rect, background);

    if (qFuzzyIsNull(borderWidth) || background == border)
        return;

    painter->save();
    painter->setBrush(background);
    painter->setPen(QPen(border, borderWidth));
    painter->drawRect(rect.adjusted(0, 0, -1, -1));
    painter->restore();
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

QPainterPath roundedPath(const QRectF &rect, Corners corners, qreal radius)
{
    QPainterPath path;

    // simple cases
    if( corners == 0 )
    {

        path.addRect( rect );
        return path;

    }

    if( corners == AllCorners ) {

        path.addRoundedRect( rect, radius, radius );
        return path;

    }

    const QSizeF cornerSize( 2*radius, 2*radius );

    // rotate counterclockwise
    // top left corner
    if( corners & CornerTopLeft )
    {

        path.moveTo( rect.topLeft() + QPointF( radius, 0 ) );
        path.arcTo( QRectF( rect.topLeft(), cornerSize ), 90, 90 );

    } else path.moveTo( rect.topLeft() );

    // bottom left corner
    if( corners & CornerBottomLeft )
    {

        path.lineTo( rect.bottomLeft() - QPointF( 0, radius ) );
        path.arcTo( QRectF( rect.bottomLeft() - QPointF( 0, 2*radius ), cornerSize ), 180, 90 );

    } else path.lineTo( rect.bottomLeft() );

    // bottom right corner
    if( corners & CornerBottomRight )
    {

        path.lineTo( rect.bottomRight() - QPointF( radius, 0 ) );
        path.arcTo( QRectF( rect.bottomRight() - QPointF( 2*radius, 2*radius ), cornerSize ), 270, 90 );

    } else path.lineTo( rect.bottomRight() );

    // top right corner
    if( corners & CornerTopRight )
    {

        path.lineTo( rect.topRight() + QPointF( 0, radius ) );
        path.arcTo( QRectF( rect.topRight() - QPointF( 2*radius, 0 ), cornerSize ), 0, 90 );

    } else path.lineTo( rect.topRight() );

    path.closeSubpath();

    return path;
}

QColor colorBlend(const QColor &color1, const QColor &color2)
{
    QColor c2 = color2.toRgb();

    if (c2.alpha() >= 255)
        return c2;

    QColor c1 = color1.toRgb();
    qreal c1_weight = 1 - c2.alphaF();

    int r = c1_weight * c1.red() + c2.alphaF() * c2.red();
    int g = c1_weight * c1.green() + c2.alphaF() * c2.green();
    int b = c1_weight * c1.blue() + c2.alphaF() * c2.blue();

    return QColor(r, g, b);
}

} // end namespace PainterHelper
} // end namespace dtyle

