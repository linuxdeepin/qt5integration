/**
 * Copyright (C) 2016 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#ifndef DARKSTYLE_H
#define DARKSTYLE_H

#include <QCommonStyle>

class DarkStyle : public QCommonStyle
{
    Q_OBJECT
public:
    DarkStyle();
    ~DarkStyle();
};

#endif // DARKSTYLE_H
