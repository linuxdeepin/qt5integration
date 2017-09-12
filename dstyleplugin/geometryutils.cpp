/**
 * Copyright (C) 2016 by Deepin Technology Co., Ltd.
 *
 * Use of this source code is governed by GPLv3
 * that can be found in the LICENSE file.
 **/

#include "geometryutils.h"

namespace dstyle {

QRect GeometryUtils::centerRect(const QRect &rect, const QSize &size)
{
    return centerRect( rect, size.width(), size.height() );
}

QRect GeometryUtils::centerRect(const QRect &rect, int width, int height)
{
    QRect newRect = rect;

    newRect.setWidth(width);
    newRect.setHeight(height);
    newRect.moveCenter(rect.center());

    return newRect;
}

QRect GeometryUtils::insideMargin(const QRect &r, int margin)
{
    return insideMargin( r, margin, margin );
}

QRect GeometryUtils::insideMargin(const QRect &r, int marginWidth, int marginHeight)
{
    return r.adjusted( marginWidth, marginHeight, -marginWidth, -marginHeight );
}

}
