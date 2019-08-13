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
#ifndef DRAWUTILS_H
#define DRAWUTILS_H

#include <QtGlobal>
#include <qnamespace.h>

QT_BEGIN_NAMESPACE
class QPainter;
class QRect;
class QRectF;
class QPoint;
class QColor;
class QPainterPath;
class QBrush;
class QRectF;
class QWidget;
QT_END_NAMESPACE

namespace chameleon {

namespace DrawUtils {
    enum Corner {
        TopLeftCorner = 0x00001,
        TopRightCorner = 0x00002,
        BottomLeftCorner = 0x00004,
        BottomRightCorner = 0x00008
    };
    Q_DECLARE_FLAGS(Corners, Corner)

    void drawShadow(QPainter *pa, const QRect &rect, qreal xRadius, qreal yRadius, const QColor &sc, qreal radius, const QPoint &offset);
    void drawShadow(QPainter *pa, const QRect &rect, const QPainterPath &path, const QColor &sc, int radius, const QPoint &offset);
    void drawRoundedRect(QPainter *pa, const QRect &rect, qreal xRadius, qreal yRadius, Corners corners, Qt::SizeMode mode = Qt::AbsoluteSize);
    void drawFork(QPainter *pa, const QRectF &rect, const QColor &color, int width = 2);
    void drawMark(QPainter *pa, const QRectF &rect, const QColor &boxInside, const QColor &boxOutside, const int penWidth, const int outLineLeng = 2);
    void drawBorder(QPainter *pa, const QRectF &rect, const QBrush &brush, int borderWidth, int radius);

    Q_DECLARE_OPERATORS_FOR_FLAGS(Corners)
} // namespace DrawUtils

} // namespace chameleon

#endif // DRAWUTILS_H
