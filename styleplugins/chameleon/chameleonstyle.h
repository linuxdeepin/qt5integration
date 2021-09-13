/*
 * Copyright (C) 2017 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     zccrs <zccrs@live.com>
 *
 * Maintainer: zccrs <zhangjide@deepin.com>
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
#ifndef CHAMELEONSTYLE_H
#define CHAMELEONSTYLE_H

#include <DStyle>

DWIDGET_USE_NAMESPACE

namespace dstyle {
class DStyleAnimation;
}

DWIDGET_BEGIN_NAMESPACE
class DStyleOptionButtonBoxButton;
DWIDGET_END_NAMESPACE

namespace chameleon {

class ChameleonStyle : public DStyle
{
public:
    explicit ChameleonStyle();

    void drawPrimitive(QStyle::PrimitiveElement pe, const QStyleOption *opt, QPainter *p,
                       const QWidget *w = nullptr) const override;
    void drawControl(QStyle::ControlElement element, const QStyleOption *opt, QPainter *p,
                     const QWidget *w = nullptr) const override;
    QRect subElementRect(QStyle::SubElement r, const QStyleOption *opt, const QWidget *widget = nullptr) const override;
    void drawComplexControl(ComplexControl cc, const QStyleOptionComplex *opt, QPainter *p,
                            const QWidget *w = nullptr) const override;
    SubControl hitTestComplexControl(ComplexControl cc, const QStyleOptionComplex *opt,
                                     const QPoint &pt, const QWidget *w = nullptr) const override;
    QRect subControlRect(ComplexControl cc, const QStyleOptionComplex *opt, SubControl sc,
                         const QWidget *w = nullptr) const override;
    QSize sizeFromContents(QStyle::ContentsType ct, const QStyleOption *opt,
                           const QSize &contentsSize, const QWidget *widget = nullptr) const override;

    int pixelMetric(QStyle::PixelMetric m, const QStyleOption *opt = nullptr, const QWidget *widget = nullptr) const override;

    int styleHint(StyleHint sh, const QStyleOption *opt = nullptr, const QWidget *w = nullptr,
                  QStyleHintReturn *shret = nullptr) const override;

    void polish(QWidget *w) override;
    void unpolish(QWidget *w) override;
    void unpolish(QApplication *application) override;

    QBrush generatedBrush(StateFlags flags, const QBrush &base, QPalette::ColorGroup cg, QPalette::ColorRole role, const QStyleOption *option) const;
    QBrush generatedBrush(StateFlags flags, const QBrush &base, DPalette::ColorGroup cg = DPalette::Normal, DPalette::ColorType type = DPalette::ItemBackground, const QStyleOption *option = nullptr) const;

private:
    void drawShadow(QPainter *p, const QRect &rect, const QColor &color) const;
    void drawBorder(QPainter *p, const QStyleOption *opt, const QWidget *w) const;
    void drawCalenderEllipse(QPainter *p, const QRect &rect, int offset) const;
    void drawButtonBoxButton(const DStyleOptionButtonBoxButton *btnopt, QPainter *p, const QRect &rect, int radius) const;
    void drawRoundedShadow(QPainter *p, const QRect &rect, const QColor &color,int frame_radius) const;
    bool isNoticks(const QStyleOptionSlider *slider, QPainter *p, const QWidget *w) const;

    QColor getColor(const QStyleOption *option, QPalette::ColorRole role) const;
    QColor getColor(const QStyleOption *option, DPalette::ColorType type, const QWidget *widget) const;
    QBrush getBrush(const QStyleOption *option, DPalette::ColorRole type) const;
    QMargins frameExtentMargins() const;
    QRect drawButtonDownArrow(const QStyleOption *opt, QPainter *p, const QWidget *w) const;

    bool drawSpinBox(const QStyleOptionSpinBox *opt, QPainter *p, const QWidget *w) const;
    void updateSpinBoxButtonState(const QStyleOptionSpinBox *opt, QStyleOptionButton& button, bool isActive, bool isEnabled) const;
    bool drawMenuBarItem(const QStyleOptionMenuItem *option, QRect &rect, QPainter *painter, const QWidget *widget) const;
    void drawMenuItemBackground(const QStyleOption *option, QPainter *painter, QStyleOptionMenuItem::MenuItemType type) const;
    bool drawMenuItem(const QStyleOptionMenuItem *option, QPainter *painter, const QWidget *widget) const;
    bool drawTabBar(QPainter *painter ,const QStyleOptionTab *tab, const QWidget *widget) const;
    bool drawTabBarLabel(QPainter *painter ,const QStyleOptionTab *tab, const QWidget *widget) const;
    bool drawTabBarCloseButton(QPainter *painter,const QStyleOption *tab , const QWidget *widget) const;
    bool drawTabBarScrollButton(QPainter *painter, const QStyleOption *opt, const QWidget *buttonWidget) const;
    bool drawComboBox(QPainter *painter, const QStyleOptionComboBox *comboBox, const QWidget *widget) const;
    bool drawComboBoxLabel(QPainter *painter, const QStyleOptionComboBox *comboBox, const QWidget *widget) const;
    void drawSliderHandle(const QStyleOptionComplex *opt, QRectF& rectHandle, QPainter *p, const QWidget *w) const;
    void drawSliderHandleFocus(const QStyleOptionComplex *opt, QRectF& rectHandle, QPainter *p, const QWidget *w) const;
    void drawIcon(const QStyleOption *opt, QPainter *p, QRect &rect, const QIcon &icon, bool checked = false) const;
    void tabLayout(const QStyleOptionTab *opt, const QWidget *widget, QRect *textRect, QRect *iconRect) const;
    bool drawTableViewItem(QStyle::PrimitiveElement pe, const QStyleOption *opt, QPainter *p,
                       const QWidget *w = nullptr) const;
    bool hideScrollBarByAnimation(const QStyleOptionSlider *scrollBar, QPainter *p) const;

public:
#ifndef QT_NO_ANIMATION
    dstyle::DStyleAnimation *animation(const QObject *target) const;
    void startAnimation(dstyle::DStyleAnimation *animation, int delay = 0) const;
    void _q_removeAnimation();
#endif

protected:
    void drawMenuItemRedPoint(const QStyleOptionMenuItem *option, QPainter *painter, const QWidget *widget) const;
    void transScrollbarMouseEvents(QObject *obj, bool on = true) const;

private:
    mutable QHash<const QObject*, dstyle::DStyleAnimation*> animations;

    bool eventFilter(QObject *watched, QEvent *event) override;
};
} // namespace chameleon

#endif // CHAMELEONSTYLE_H
