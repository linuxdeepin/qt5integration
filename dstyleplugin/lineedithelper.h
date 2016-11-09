/**
 * Copyright (C) 2016 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#ifndef LINEEDITHELPER_H
#define LINEEDITHELPER_H

#include <QStyleOptionComplex>

namespace dstyle {

class LineEditHelper
{
public:
    static bool drawFrameLineEditPrimitive( const QStyleOption* option, QPainter* painter, const QWidget* widget );

    static QColor getBorderColor( bool enabled, bool hasFocus );
    static QColor getBackgroundColor( bool enabled, bool hasFocus );
};

}

#endif // LINEEDITHELPER_H
