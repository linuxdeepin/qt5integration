/*
 * Copyright (C) 2016 ~ 2017 Deepin Technology Co., Ltd.
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

#ifndef GEOMETRYUTILS_H
#define GEOMETRYUTILS_H

#include <QRect>

#include "common.h"

namespace dstyle {

class GeometryUtils
{
public:
    static QRect centerRect(const QRect &rect, const QSize& size );
    static QRect centerRect(const QRect &rect, int width, int height);
    static QRect insideMargin( const QRect& r, int margin );
    static QRect insideMargin( const QRect& r, int marginWidth, int marginHeight );

    static qreal frameRadius( qreal bias = 0 )
    { return qMax( qreal( Metrics::Frame_FrameRadius ) - 0.5 + bias, 0.0 ); }

    static QRectF shadowRect( const QRectF& rect )
    { return rect.adjusted( 0.5, 0.5, -0.5, -0.5 ).translated( 0.5, 0.5 ); }
};

}

#endif // GEOMETRYUTILS_H
