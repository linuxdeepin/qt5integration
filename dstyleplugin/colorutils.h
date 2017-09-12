/**
 * Copyright (C) 2016 by Deepin Technology Co., Ltd.
 *
 * Use of this source code is governed by GPLv3
 * that can be found in the LICENSE file.
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
