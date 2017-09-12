/**
 * Copyright (C) 2016 by Deepin Technology Co., Ltd.
 *
 * Use of this source code is governed by GPLv3
 * that can be found in the LICENSE file.
 **/

#ifndef COMMONHELPER_H
#define COMMONHELPER_H

#include <QPainter>

namespace dstyle {

class Style;

class CommonHelper
{
public:
    static void renderFrame(QPainter* painter, const QRect& rect, const QBrush &brush, const QBrush &outline);

    static Style *widgetStyle(const QWidget *widget);
};

}

#endif // COMMONHELPER_H
