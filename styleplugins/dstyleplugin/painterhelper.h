/*
 * Copyright (C) 2017 ~ 2018 Deepin Technology Co., Ltd.
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

#ifndef PAINTERHELPER_H
#define PAINTERHELPER_H

#include <QBrush>

#include "common.h"

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
    QPainterPath roundedPath( const QRectF& rect, Corners corners, qreal radius );
    QColor colorBlend(const QColor &color1, const QColor &color2);
} // end namespace PainterHelper
} // end namespace dtyle

#endif // PAINTERHELPER_H
