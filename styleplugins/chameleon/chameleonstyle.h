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

    QIcon standardIcon(QStyle::StandardPixmap standardIcon, const QStyleOption *opt = nullptr,
                       const QWidget *widget = nullptr) const override;
    QPixmap standardPixmap(QStyle::StandardPixmap sp, const QStyleOption *opt = nullptr,
                           const QWidget *widget = nullptr) const override;

    QPixmap generatedIconPixmap(QIcon::Mode iconMode, const QPixmap &pixmap,
                                const QStyleOption *opt) const override;
    int layoutSpacing(QSizePolicy::ControlType control1, QSizePolicy::ControlType control2,
                      Qt::Orientation orientation, const QStyleOption *option = nullptr,
                      const QWidget *widget = nullptr) const override;

    QPalette standardPalette() const override;
    void polish(QPalette &pa) override;
    void polish(QApplication *app) override;
    void polish(QWidget *w) override;
    void unpolish(QWidget *w) override;
    void unpolish(QApplication *application) override;

    QBrush generatedBrush(StateFlags flags, const QBrush &base, QPalette::ColorGroup cg, QPalette::ColorRole role, const QStyleOption *option) const;
    QBrush generatedBrush(StateFlags flags, const QBrush &base, DPalette::ColorGroup cg = DPalette::Normal, DPalette::ColorType type = DPalette::ItemBackground, const QStyleOption *option = nullptr) const;

private:
    bool isDrakStyle() const;
    void drawShadow(QPainter *p, const QRect &rect, const QColor &color) const;
    void drawBorder(QPainter *p, const QRect &rect, const QBrush &brush) const;
    void drawRoundedShadow(QPainter *p, const QRect &rect, const QColor &color,int frame_radius) const;

    QColor getColor(const QStyleOption *option, QPalette::ColorRole role) const;
    QColor getColor(const QStyleOption *option, DPalette::ColorType type, const QWidget *widget) const;
    QMargins frameExtentMargins() const;

    bool drawSpinBox(const QStyleOptionSpinBox *opt, QPainter *p, const QWidget *w) const;
    void updateSpinBoxButtonState(const QStyleOptionSpinBox *opt, QStyleOptionButton& button, bool isActive, bool isEnabled) const;
    bool drawMenuBarItem(const QStyleOptionMenuItem *option, QRect &rect, QPainter *painter, const QWidget *widget) const;
    bool drawMenuItem(const QStyleOptionMenuItem *option, QPainter *painter, const QWidget *widget) const;
    bool drawTabBar(QPainter *painter ,const QStyleOptionTab *tab, const QWidget *widget) const;
    bool drawTabBarLabel(QPainter *painter ,const QStyleOptionTab *tab, const QWidget *widget) const;
    bool drawTabBarCloseButton(QPainter *painter,const QStyleOption *tab , const QWidget *widget) const;
    bool drawTabBarScrollButton(QPainter *painter, const QStyleOption *opt, const QWidget *buttonWidget) const;
};

} // namespace chameleon

#endif // CHAMELEONSTYLE_H
