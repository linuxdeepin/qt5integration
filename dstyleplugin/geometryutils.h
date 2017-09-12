/**
 * Copyright (C) 2016 by Deepin Technology Co., Ltd.
 *
 * Use of this source code is governed by GPLv3
 * that can be found in the LICENSE file.
 **/

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
