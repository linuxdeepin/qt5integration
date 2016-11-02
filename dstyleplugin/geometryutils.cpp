/**
 * Copyright (C) 2016 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#include "geometryutils.h"

QRect GeometryUtils::centerRect(const QRect &rect, int width, int height)
{
    return QRect(rect.left() + (rect.width() - width)/2, rect.top() + (rect.height() - height)/2, width, height);
}

QRect GeometryUtils::insideMargin(const QRect &r, int margin)
{
    return insideMargin( r, margin, margin );
}

QRect GeometryUtils::insideMargin(const QRect &r, int marginWidth, int marginHeight)
{
    return r.adjusted( marginWidth, marginHeight, -marginWidth, -marginHeight );
}
