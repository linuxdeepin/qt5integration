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
    void drawControl(ControlElement element, const QStyleOption *opt, QPainter *p,
                     const QWidget *w = nullptr) const override;
    QRect subElementRect(SubElement r, const QStyleOption *opt, const QWidget *widget = nullptr) const override;
    void drawComplexControl(ComplexControl cc, const QStyleOptionComplex *opt, QPainter *p,
                            const QWidget *w = nullptr) const override;
    SubControl hitTestComplexControl(ComplexControl cc, const QStyleOptionComplex *opt,
                                     const QPoint &pt, const QWidget *w = nullptr) const override;
    QRect subControlRect(ComplexControl cc, const QStyleOptionComplex *opt, SubControl sc,
                         const QWidget *w = nullptr) const override;
    QSize sizeFromContents(ContentsType ct, const QStyleOption *opt,
                           const QSize &contentsSize, const QWidget *widget = nullptr) const override;

    int pixelMetric(QStyle::PixelMetric m, const QStyleOption *opt = nullptr, const QWidget *widget = nullptr) const override;

    int styleHint(StyleHint sh, const QStyleOption *opt = nullptr, const QWidget *w = nullptr,
                  QStyleHintReturn *shret = nullptr) const override;

    QIcon standardIcon(StandardPixmap standardIcon, const QStyleOption *opt = nullptr,
                       const QWidget *widget = nullptr) const override;
    QPixmap standardPixmap(StandardPixmap sp, const QStyleOption *opt = nullptr,
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

private:
    bool isDrakStyle() const;
    void drawShadow(QPainter *p, const QRect &rect, const QColor &color) const;
    void drawBorder(QPainter *p, const QRect &rect, const QBrush &brush) const;

    QColor getColor(const QStyleOption *option, QPalette::ColorRole role) const;
};

} // namespace chameleon

#endif // CHAMELEONSTYLE_H
