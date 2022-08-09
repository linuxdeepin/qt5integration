/*
 * SPDX-FileCopyrightText: 2016 - 2022 UnionTech Software Technology Co., Ltd.  
 * SPDX-License-Identifier: GPL-3.0-or-later
 */
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
