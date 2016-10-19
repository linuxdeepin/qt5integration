/**
 * Copyright (C) 2016 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#ifndef LIGHTSTYLE_H
#define LIGHTSTYLE_H

#include <QCommonStyle>

class LightStyle : public QCommonStyle
{
    Q_OBJECT

public:
    LightStyle();
    ~LightStyle();
};

#endif // LIGHTSTYLE_H
