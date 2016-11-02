/**
 * Copyright (C) 2016 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#include "colorutils.h"

namespace dstyle {

QColor ColorUtils::alphaColor(QColor color, qreal alpha)
{
    if( alpha >= 0 && alpha < 1.0 ) {
        color.setAlphaF( alpha*color.alphaF() );
    }

    return color;
}

} // end namespace dstyle.
