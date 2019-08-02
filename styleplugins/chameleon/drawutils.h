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

QT_BEGIN_NAMESPACE
class QPainter;
class QRect;
class QPoint;
class QColor;
class QPainterPath;
QT_END_NAMESPACE

namespace chameleon {

namespace DrawUtils {
    void drawShadow(QPainter *pa, const QRect &rect, qreal xRadius, qreal yRadius, const QColor &sc, qreal radius, const QPoint &offset);
    void drawShadow(QPainter *pa, const QPainterPath &path, const QColor &sc, int radius, const QPoint &offset);
} // namespace DrawUtils

} // namespace chameleon

#endif // DRAWUTILS_H
