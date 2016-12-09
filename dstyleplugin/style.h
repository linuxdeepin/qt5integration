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

#include <private/qcommonstyle_p.h>

#include "common.h"

namespace dstyle {

class PaletteExtended;
class StylePrivate;
class Style : public QCommonStyle
{
    Q_OBJECT
public:
    explicit Style(StyleType type);
    ~Style();

    void polish(QPalette &p) Q_DECL_OVERRIDE;
    void polish(QWidget *w) Q_DECL_OVERRIDE;
    void unpolish(QWidget *w) Q_DECL_OVERRIDE;

    int pixelMetric(PixelMetric m, const QStyleOption *opt = Q_NULLPTR, const QWidget *widget = Q_NULLPTR) const Q_DECL_OVERRIDE;

    QRect subControlRect(ComplexControl cc, const QStyleOptionComplex *opt, SubControl sc, const QWidget *w) const Q_DECL_OVERRIDE;
    QRect subElementRect(SubElement r, const QStyleOption *opt, const QWidget *widget) const Q_DECL_OVERRIDE;

    void drawControl(ControlElement element, const QStyleOption *opt, QPainter *p, const QWidget *w) const Q_DECL_OVERRIDE;
    void drawComplexControl(ComplexControl cc, const QStyleOptionComplex *opt, QPainter *p, const QWidget *w) const Q_DECL_OVERRIDE;
    void drawPrimitive(PrimitiveElement pe, const QStyleOption *opt, QPainter *p, const QWidget *w) const Q_DECL_OVERRIDE;

    int styleHint(StyleHint sh, const QStyleOption *opt = Q_NULLPTR, const QWidget *w = Q_NULLPTR,
                  QStyleHintReturn *shret = Q_NULLPTR) const Q_DECL_OVERRIDE;

    QSize sizeFromContents(ContentsType type, const QStyleOption *option,
                           const QSize &size, const QWidget *widget) const Q_DECL_OVERRIDE;

    QIcon standardIcon(StandardPixmap standardIcon, const QStyleOption *opt, const QWidget *widget) const Q_DECL_OVERRIDE;

    // some function type alias.
    //    using SubControlRectFunc = QRect(*)(const QStyleOptionComplex *opt, SubControl sc, const QWidget *w);
    using DrawComplexControlFunc = bool(Style::*)(ComplexControl, const QStyleOptionComplex *, QPainter *, const QWidget *) const;
    using DrawPrimitiveFunc = bool(*)(const QStyleOption*, QPainter*, const QWidget* );
    using DrawControlFunc = bool(Style::*)(const QStyleOption*, QPainter*, const QWidget*) const;

private:
    // push button
    bool drawPushButtonBevel(const QStyleOption*, QPainter*, const QWidget*) const;
    bool drawPushButtonLabel(const QStyleOption *, QPainter *, const QWidget *) const;
    bool drawPushButtonFrame(QPainter* painter, const QRect& rect, const QBrush &color, const QBrush &outline, const QColor& shadow) const;

    // scroll bar
    QRect scrollbarSubControlRect(const QStyleOptionComplex *opt, SubControl sc, const QWidget *widget ) const;
    bool drawScrollBarSliderControl(const QStyleOption *option, QPainter *painter, const QWidget *widget) const;

    // menu
    bool drawMenuItemControl(const QStyleOption *option, QPainter *painter, const QWidget *widget) const;

    // menu bar
    bool drawMenuBarItemControl(const QStyleOption *option, QPainter *painter, const QWidget *widget) const;

    // slider
    QRect sliderSubControlRect(const QStyleOptionComplex *option, SubControl subControl, const QWidget *widget ) const;
    bool drawSlider(ComplexControl control, const QStyleOptionComplex *opt, QPainter *p, const QWidget *w) const;
    void drawSliderGroove(QPainter*, const QRect&, const QBrush & brush) const;
    void drawSliderHandle(QPainter* painter, const QRect& rect, const QBrush &brush, const QColor& outline, const QString &type) const;
    bool drawSliderTickmarkLabels(const QStyleOption*, QPainter*, const QWidget*) const;

    // indicator
    void drawStandardIcon(QStyle::StandardPixmap sp, const QStyleOption *opt, QPainter *p, const QWidget *widget) const;
    void drawDeepinStyleIcon(const QString &name, const QStyleOption *opt, QPainter *p, const QWidget *widget) const;

    static QWindow *qt_getWindow(const QWidget *widget);
    static QColor mergedColors(const QColor &colorA, const QColor &colorB, int factor = 50);
    static QPixmap colorizedImage(const QString &fileName, const QColor &color, int rotation = 0);

    // progress bar
    bool drawProgressBarControl(const QStyleOption *option, QPainter *painter, const QWidget *widget) const;
    bool drawProgressBarContentsControl(const QStyleOption *option, QPainter *painter, const QWidget *widget) const;
    bool drawProgressBarGrooveControl(const QStyleOption *option, QPainter *painter, const QWidget *widget) const;
    bool drawProgressBarLabelControl(const QStyleOption *option, QPainter *painter, const QWidget *widget) const;
    QRect progressBarLabelRect(const QStyleOption *opt, const QWidget *widget) const;
    QRect progressBarGrooveRect(const QStyleOption *opt, const QWidget *widget) const;
    QRect progressBarContentsRect(const QStyleOption *opt, const QWidget *widget) const;

    StyleType m_type;
    PaletteExtended *m_palette;

    friend class SliderHelper;
    friend class PushButtonHelper;
    friend class LineEditHelper;
    friend class FrameHelper;
    friend class ScrollBarHelper;

    Q_PRIVATE_SLOT(d_func(), void _q_removeAnimation())

    Q_DECLARE_PRIVATE(Style)
};

class DStyleAnimation;
class StylePrivate : public QCommonStylePrivate
{
public:
    QColor outline(const QPalette &pal) const;
    QColor buttonColor(const QPalette &pal) const;
    QColor highlight(const QPalette &pal) const;
    QColor highlightedOutline(const QPalette &pal) const;
    QColor backgroundColor(const QPalette &pal, const QWidget* widget) const;

    inline QColor innerContrastLine() const {
        return QColor(255, 255, 255, 30);
    }
    inline QColor lightShade() const {
        return QColor(255, 255, 255, 90);
    }
    inline QColor darkShade() const {
        return QColor(0, 0, 0, 60);
    }

#ifndef QT_NO_ANIMATION
    DStyleAnimation *animation(const QObject *target) const;
    void startAnimation(DStyleAnimation *animation) const;
    void stopAnimation(const QObject *target) const;

    void _q_removeAnimation();
#endif

    Q_DECLARE_PUBLIC(Style)

private:
    mutable QHash<const QObject*, DStyleAnimation*> animations;
};

}

#endif // STYLE_H
