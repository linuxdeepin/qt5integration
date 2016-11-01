/**
 * Copyright (C) 2016 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#include "style.h"

Style::Style(dstyle::StyleType style) :
    QCommonStyle(),
    m_type(style)
{

}

Style::~Style()
{

}

void Style::polish(QPalette &p)
{
    Q_UNUSED(p)
}

void Style::polish(QWidget *w)
{
    Q_UNUSED(w)
}
