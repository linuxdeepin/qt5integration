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

class PaletteExtended;
class Style : public QCommonStyle
{
    Q_OBJECT
public:
    explicit Style(StyleType type);
    ~Style();

    void polish(QPalette &p) Q_DECL_OVERRIDE;
    void polish(QWidget *w) Q_DECL_OVERRIDE;
    void unpolish(QWidget *w) Q_DECL_OVERRIDE;

    int pixelMetric(PixelMetric m, const QStyleOption *opt, const QWidget *widget) const Q_DECL_OVERRIDE;

    QRect subControlRect(ComplexControl cc, const QStyleOptionComplex *opt, SubControl sc, const QWidget *w) const Q_DECL_OVERRIDE;

    void drawControl(ControlElement element, const QStyleOption *opt, QPainter *p, const QWidget *w) const Q_DECL_OVERRIDE;
    void drawComplexControl(ComplexControl cc, const QStyleOptionComplex *opt, QPainter *p, const QWidget *w) const Q_DECL_OVERRIDE;
    void drawPrimitive(PrimitiveElement pe, const QStyleOption *opt, QPainter *p, const QWidget *w) const Q_DECL_OVERRIDE;

    int styleHint(StyleHint sh, const QStyleOption *opt = Q_NULLPTR, const QWidget *w = Q_NULLPTR,
                  QStyleHintReturn *shret = Q_NULLPTR) const Q_DECL_OVERRIDE;

    // some function type alias.
//    using SubControlRectFunc = QRect(*)(const QStyleOptionComplex *opt, SubControl sc, const QWidget *w);
    using DrawComplexControlFunc = bool(Style::*)(const QStyleOptionComplex *, QPainter *, const QWidget *) const;
    using DrawPrimitiveFunc = bool(*)(const QStyleOption*, QPainter*, const QWidget* );
    using DrawControlFunc = bool(Style::*)(const QStyleOption*, QPainter*, const QWidget*) const;

private:
    // helper functions
    QRect sliderSubControlRect( const QStyleOptionComplex *opt, SubControl sc, const QWidget *w ) const;
    QRect scrollbarSubControlRect(const QStyleOptionComplex *opt, SubControl sc, const QWidget *widget ) const;

    // push button
    bool drawPushButtonBevel(const QStyleOption*, QPainter*, const QWidget*) const;
    bool drawPushButtonLabel(const QStyleOption *, QPainter *, const QWidget *) const;
    bool drawPushButtonFrame(QPainter* painter, const QRect& rect, const QBrush &color, const QBrush &outline, const QColor& shadow) const;

    // scroll bar
    bool drawScrollBarSliderControl(const QStyleOption *option, QPainter *painter, const QWidget *widget) const;

    // menu bar
    bool drawMenuBarItemControl(const QStyleOption *option, QPainter *painter, const QWidget *widget) const;

    // slider
    bool drawSlider(const QStyleOptionComplex *opt, QPainter *p, const QWidget *w) const;
    void drawSliderGroove(QPainter*, const QRect&, const QBrush & brush) const;
    void drawSliderHandle(QPainter* painter, const QRect& rect, const QBrush &brush, const QColor& outline) const;
    bool drawSliderTickmarkLabels(const QStyleOption*, QPainter*, const QWidget*) const;

    StyleType m_type;
    PaletteExtended *m_palette;

    friend class SliderHelper;
    friend class PushButtonHelper;
    friend class LineEditHelper;
    friend class FrameHelper;
    friend class ScrollBarHelper;
};

}

#endif // STYLE_H
