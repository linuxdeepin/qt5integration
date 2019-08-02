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

    pa.setCompositionMode(QPainter::CompositionMode_Source);

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
    const QRect shadow_rect = rect.adjusted(offset.x(), offset.y(), 0, 0);
    QImage new_shadow = borderImage(shadow, margins, shadow_rect.size() * scale, QImage::Format_ARGB32_Premultiplied);
//    QPainter pa_shadow(&new_shadow);
//    pa_shadow.setCompositionMode(QPainter::CompositionMode_Clear);
//    pa_shadow.setPen(Qt::NoPen);
//    pa_shadow.setBrush(Qt::transparent);
//    pa_shadow.setRenderHint(QPainter::Antialiasing);
//    pa_shadow.drawRoundedRect((new_shadow.rect() - QMargins(radius, radius, radius, radius)).translated(-offset), xRadius, yRadius);
//    pa_shadow.end();
    new_shadow.setDevicePixelRatio(scale);
    pa->drawImage(rect.topLeft() + offset, new_shadow);
}

void drawShadow(QPainter *pa, const QPainterPath &path, const QColor &sc, int radius, const QPoint &offset)
{

}

} // namespace DrawUtils

}
