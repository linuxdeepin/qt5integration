/*
 * SPDX-FileCopyrightText: 2016 - 2022 UnionTech Software Technology Co., Ltd.  
 * SPDX-License-Identifier: GPL-3.0-or-later
 */
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
