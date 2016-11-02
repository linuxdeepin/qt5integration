/**
 * Copyright (C) 2016 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#ifndef GEOMETRYUTILS_H
#define GEOMETRYUTILS_H

#include <QRect>

class GeometryUtils
{
public:
    static QRect centerRect(const QRect &rect, int width, int height);
    static QRect insideMargin( const QRect& r, int margin );
    static QRect insideMargin( const QRect& r, int marginWidth, int marginHeight );
};

#endif // GEOMETRYUTILS_H
