/**
 * Copyright (C) 2016 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#ifndef COLORUTILS_H
#define COLORUTILS_H

#include <QColor>

namespace dstyle {

class ColorUtils
{
public:
    //* add alpha channel multiplier to color
    static QColor alphaColor( QColor color, qreal alpha );
};

}

#endif // COLORUTILS_H
