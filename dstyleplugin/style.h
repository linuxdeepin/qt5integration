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

namespace dstyle {

class Style : public QCommonStyle
{
    Q_OBJECT
public:
    explicit Style(StyleType type);
    ~Style();

    void polish(QPalette &p) Q_DECL_OVERRIDE;
    void polish(QWidget *w) Q_DECL_OVERRIDE;

    int pixelMetric(PixelMetric m, const QStyleOption *opt, const QWidget *widget) const Q_DECL_OVERRIDE;

    QRect subControlRect(ComplexControl cc, const QStyleOptionComplex *opt, SubControl sc, const QWidget *w) const Q_DECL_OVERRIDE;

    void drawComplexControl(ComplexControl cc, const QStyleOptionComplex *opt, QPainter *p, const QWidget *w) const Q_DECL_OVERRIDE;

    // some function type alias.
//    using SubControlRectFunc = QRect(*)(const QStyleOptionComplex *opt, SubControl sc, const QWidget *w);
    using RenderFunc = bool(*)(const QStyleOptionComplex *opt, QPainter *p, const QWidget *w);

    // helper functions
    QRect sliderSubControlRect( const QStyleOptionComplex *opt, SubControl sc, const QWidget *w ) const;

private:
    dstyle::StyleType m_type;
};


}

#endif // STYLE_H
