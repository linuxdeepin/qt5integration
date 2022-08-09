/*
 * SPDX-FileCopyrightText: 2016-2022 UnionTech Software Technology Co., Ltd.  
 * SPDX-License-Identifier: GPL-3.0-or-later
 */
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
