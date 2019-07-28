/*
 * Copyright (C) 2016 ~ 2018 Deepin Technology Co., Ltd.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

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
