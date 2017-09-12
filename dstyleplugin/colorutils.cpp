/**
 * Copyright (C) 2016 by Deepin Technology Co., Ltd.
 *
 * Use of this source code is governed by GPLv3
 * that can be found in the LICENSE file.
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
