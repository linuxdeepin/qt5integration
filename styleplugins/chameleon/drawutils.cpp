/*
 * Copyright (C) 2017 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     zccrs <zccrs@live.com>
 *
 * Maintainer: zccrs <zhangjide@deepin.com>
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
#include "drawutils.h"

#include <QPainter>
#include <QPixmapCache>
#include <QDebug>
#include <QWidget>
#include <QPaintEngine>

#include <math.h>

QT_BEGIN_NAMESPACE
//extern Q_WIDGETS_EXPORT void qt_blurImage(QImage &blurImage, qreal radius, bool quality, int transposed = 0);
extern Q_WIDGETS_EXPORT void qt_blurImage(QPainter *p, QImage &blurImage, qreal radius, bool quality, bool alphaOnly, int transposed = 0);
QT_END_NAMESPACE

namespace chameleon {

namespace DrawUtils {
static QImage dropShadow(const QPixmap &px, qreal radius, const QColor &color)
{
    if (px.isNull())
        return QImage();

    QImage tmp(px.size() + QSize(radius * 2, radius * 2), QImage::Format_ARGB32_Premultiplied);
    tmp.fill(0);
    QPainter tmpPainter(&tmp);
    tmpPainter.setCompositionMode(QPainter::CompositionMode_Source);
    tmpPainter.drawPixmap(QPoint(radius, radius), px);
    tmpPainter.end();

    // blur the alpha channel
    QImage blurred(tmp.size(), QImage::Format_ARGB32_Premultiplied);
    blurred.fill(0);
    QPainter blurPainter(&blurred);
    qt_blurImage(&blurPainter, tmp, radius, false, true);
    blurPainter.end();

    if (color == QColor(Qt::black))
        return blurred;

    tmp = blurred;

    // blacken the image...
    tmpPainter.begin(&tmp);
    tmpPainter.setCompositionMode(QPainter::CompositionMode_SourceIn);
    tmpPainter.fillRect(tmp.rect(), color);
    tmpPainter.end();

    return tmp;
}

static QList<QRect> sudokuByRect(const QRect &rect, QMargins borders)
{
    QList<QRect> list;

//    qreal border_width = borders.left() + borders.right();

//    if ( border_width > rect.width()) {
//        borders.setLeft(borders.left() / border_width * rect.width());
//        borders.setRight(rect.width() - borders.left());
//    }

//    qreal border_height = borders.top() + borders.bottom();

//    if (border_height > rect.height()) {
//        borders.setTop(borders.top()/ border_height * rect.height());
//        borders.setBottom(rect.height() - borders.top());
//    }

    const QRect &contentsRect = rect - borders;

    list << QRect(0, 0, borders.left(), borders.top());
    list << QRect(list.at(0).topRight(), QSize(contentsRect.width(), borders.top())).translated(1, 0);
    list << QRect(list.at(1).topRight(), QSize(borders.right(), borders.top())).translated(1, 0);
    list << QRect(list.at(0).bottomLeft(), QSize(borders.left(), contentsRect.height())).translated(0, 1);
    list << contentsRect;
    list << QRect(contentsRect.topRight(), QSize(borders.right(), contentsRect.height())).translated(1, 0);
    list << QRect(list.at(3).bottomLeft(), QSize(borders.left(), borders.bottom())).translated(0, 1);
    list << QRect(contentsRect.bottomLeft(), QSize(contentsRect.width(), borders.bottom())).translated(0, 1);
    list << QRect(contentsRect.bottomRight(), QSize(borders.left(), borders.bottom())).translated(1, 1);

    return list;
}

static QImage borderImage(const QPixmap &px, const QMargins &borders, const QSize &size, QImage::Format format)
{
    QImage image(size, format);
    QPainter pa(&image);

    const QList<QRect> sudoku_src = sudokuByRect(px.rect(), borders);
    const QList<QRect> sudoku_tar = sudokuByRect(QRect(QPoint(0, 0), size), borders);

    pa.setCompositionMode(QPainter::CompositionMode_Source);  //设置组合模式

    for (int i = 0; i < 9; ++i) {
        pa.drawPixmap(sudoku_tar[i], px, sudoku_src[i]);
    }

    pa.end();

    return image;
}

void drawShadow(QPainter *pa, const QRect &rect, qreal xRadius, qreal yRadius, const QColor &sc, qreal radius, const QPoint &offset)
{
    QPixmap shadow;
    qreal scale = pa->paintEngine()->paintDevice()->devicePixelRatioF();
    QRect shadow_rect = rect;

    shadow_rect.setTopLeft(shadow_rect.topLeft() + offset);

    xRadius *= scale;
    yRadius *= scale;
    radius *= scale;

    const QString &key = QString("dstyle-chameleon-shadow-%1x%2-%3-%4").arg(xRadius).arg(yRadius).arg(sc.name()).arg(radius);

    if (!QPixmapCache::find(key, shadow)) {
        QImage shadow_base(QSize(xRadius * 3, yRadius * 3), QImage::Format_ARGB32_Premultiplied);
        shadow_base.fill(0);
        QPainter pa(&shadow_base);

        pa.setBrush(sc);
        pa.setPen(Qt::NoPen);
        pa.drawRoundedRect(shadow_base.rect(), xRadius, yRadius);
        pa.end();

        shadow_base = dropShadow(QPixmap::fromImage(shadow_base), radius, sc);
        shadow = QPixmap::fromImage(shadow_base);
        QPixmapCache::insert(key, shadow);
    }

    const QMargins margins(xRadius + radius, yRadius + radius, xRadius + radius, yRadius + radius);
    QImage new_shadow = borderImage(shadow, margins, shadow_rect.size() * scale, QImage::Format_ARGB32_Premultiplied);
//    QPainter pa_shadow(&new_shadow);
//    pa_shadow.setCompositionMode(QPainter::CompositionMode_Clear);
//    pa_shadow.setPen(Qt::NoPen);
//    pa_shadow.setBrush(Qt::transparent);
//    pa_shadow.setRenderHint(QPainter::Antialiasing);
//    pa_shadow.drawRoundedRect((new_shadow.rect() - QMargins(radius, radius, radius, radius)).translated(-offset), xRadius, yRadius);
//    pa_shadow.end();
    new_shadow.setDevicePixelRatio(scale);
    pa->drawImage(shadow_rect.topLeft(), new_shadow);
}

void drawShadow(QPainter *pa, const QRect &rect, const QPainterPath &path, const QColor &sc, int radius, const QPoint &offset)
{
    QPixmap shadow;
    qreal scale = pa->paintEngine()->paintDevice()->devicePixelRatioF();
    QRect shadow_rect = rect;

    shadow_rect.setTopLeft(rect.topLeft() + offset);
    radius *= scale;

    QImage shadow_base(shadow_rect.size() * scale, QImage::Format_ARGB32_Premultiplied);
    shadow_base.fill(0);
    shadow_base.setDevicePixelRatio(scale);

    QPainter paTmp(&shadow_base);
    paTmp.setBrush(sc);
    paTmp.setPen(Qt::NoPen);
    paTmp.drawPath(path);
    paTmp.end();
    shadow_base = dropShadow(QPixmap::fromImage(shadow_base), radius, sc);
    shadow = QPixmap::fromImage(shadow_base);
    shadow.setDevicePixelRatio(scale);

    pa->drawPixmap(shadow_rect, shadow);
}

void drawFork(QPainter *pa, const QRectF &rect, const QColor &color, int width)
{
    QPen pen;
    pen.setWidth(width);
    pen.setColor(color);

    pa->setRenderHint(QPainter::Antialiasing, true);
    pa->setPen(pen);
    pa->setBrush(Qt::NoBrush);
    pa->drawLine(rect.topLeft(), rect.bottomRight());
    pa->drawLine(rect.bottomLeft(), rect.topRight());
}

void drawRoundedRect(QPainter *pa, const QRect &rect, qreal xRadius, qreal yRadius, Corners corners, Qt::SizeMode mode)
{
    QRectF r = rect.normalized();

    if (r.isNull())
        return;

    if (mode == Qt::AbsoluteSize) {
        qreal w = r.width() / 2;
        qreal h = r.height() / 2;

        if (w == 0) {
            xRadius = 0;
        } else {
            xRadius = 100 * qMin(xRadius, w) / w;
        }
        if (h == 0) {
            yRadius = 0;
        } else {
            yRadius = 100 * qMin(yRadius, h) / h;
        }
    } else {
        if (xRadius > 100)                          // fix ranges
            xRadius = 100;

        if (yRadius > 100)
            yRadius = 100;
    }

    if (xRadius <= 0 || yRadius <= 0) {             // add normal rectangle
        pa->drawRect(r);
        return;
    }

    QPainterPath path;
    qreal x = r.x();
    qreal y = r.y();
    qreal w = r.width();
    qreal h = r.height();
    qreal rxx2 = w * xRadius / 100;
    qreal ryy2 = h * yRadius / 100;

    path.arcMoveTo(x, y, rxx2, ryy2, 180);

    if (corners & TopLeftCorner) {
        path.arcTo(x, y, rxx2, ryy2, 180, -90);
    } else {
        path.lineTo(r.topLeft());
    }

    if (corners & TopRightCorner) {
        path.arcTo(x + w - rxx2, y, rxx2, ryy2, 90, -90);
    } else {
        path.lineTo(r.topRight());
    }

    if (corners & BottomRightCorner) {
        path.arcTo(x + w - rxx2, y + h - ryy2, rxx2, ryy2, 0, -90);
    } else {
        path.lineTo(r.bottomRight());
    }

    if (corners & BottomLeftCorner) {
        path.arcTo(x, y + h - ryy2, rxx2, ryy2, 270, -90);
    } else {
        path.lineTo(r.bottomLeft());
    }

    path.closeSubpath();
    pa->drawPath(path);
}

void drawMark(QPainter *pa, const QRectF &rect, const QColor &boxInside, const QColor &boxOutside, const int penWidth, const int outLineLeng)
{
    QPen pen(boxInside);
    pen.setWidth(penWidth);
    pa->setPen(pen);
    pen.setJoinStyle(Qt::RoundJoin);
    pa->setRenderHint(QPainter::Antialiasing, true);

    pa->drawLine(rect.x(), rect.center().y(), rect.center().x(), rect.bottom());
    pa->drawLine(rect.center().x(), rect.bottom(), rect.right(), rect.y());

    int xWide = qRound64(rect.bottom() / 2);
    int yHigh = qRound64(rect.right());
    int length = qRound64(sqrt(pow(xWide, 2) + pow(yHigh, 2)));

    if (outLineLeng == 0)
        return;

    int y = qRound64((yHigh * outLineLeng) / sqrt(length) - rect.y());
    int Result = qRound64(rect.right() - rect.height() - y);
    int x = qRound64((xWide * outLineLeng) / sqrt(length) + xWide + rect.x() + 1);

    pen.setColor(boxOutside);
    pa->setPen(pen);
    pa->drawLine(rect.right(), rect.y(), x, Result);
}

void drawBorder(QPainter *pa, const QRectF &rect, const QBrush &brush, int borderWidth, int radius)
{
    pa->setPen(QPen(brush, borderWidth, Qt::SolidLine));
    pa->setBrush(Qt::NoBrush);
    pa->setRenderHint(QPainter::Antialiasing);
    pa->drawRoundedRect(rect.adjusted(1, 1, -1, -1), radius, radius) ;
}

} // namespace DrawUtils

}
