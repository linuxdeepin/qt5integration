/**
 * Copyright (C) 2016 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#ifndef STYLE_H
#define STYLE_H

#include <QCommonStyle>

#include "common.h"

class Style : public QCommonStyle
{
    Q_OBJECT
public:
    explicit Style(dstyle::StyleType type);
    ~Style();

    void polish(QPalette &p) Q_DECL_OVERRIDE;
    void polish(QWidget *w) Q_DECL_OVERRIDE;

private:
    dstyle::StyleType m_type;
};

#endif // STYLE_H
