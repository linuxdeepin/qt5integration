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
#include "chameleonstyle.h"

#include <DNativeSettings>
#include <DStyleOption>

#include <QVariant>
#include <QDebug>
#include <QApplication>

DGUI_USE_NAMESPACE
DWIDGET_USE_NAMESPACE

static QColor light[QPalette::NColorRoles] {
    QColor("#414d68"),                  //WindowText
    QColor("#e5e5e5"),                  //Button
    QColor("#e6e6e6"),                  //Light
    QColor("#e5e5e5"),                  //Midlight
    QColor("#e3e3e3"),                  //Dark
    QColor("#e4e4e4"),                  //Mid
    QColor("#414d68"),                  //Text
    QColor("#3768ff"),                  //BrightText
    QColor("#414d68"),                  //ButtonText
    Qt::white,                          //Base
    QColor("#f8f8f8"),                  //Window
    QColor(0, 0, 0, 0.05 * 255),        //Shadow
    QColor("#0081ff"),                  //Highlight
    Qt::white,                          //HighlightedText
    QColor("#0082fa"),                  //Link
    QColor("#ad4579"),                  //LinkVisited
    QColor(0, 0, 0, 0.03 * 255),        //AlternateBase
    Qt::white,                          //NoRole
    QColor(255, 255, 255, 0.8 * 255),   //ToolTipBase
    Qt::black                           //ToolTipText
};

static QColor dark[QPalette::NColorRoles] {
    QColor("#c0c6d4"),                  //WindowText
    QColor("#444444"),                  //Button
    QColor("#484848"),                  //Light
    QColor("#474747"),                  //Midlight
    QColor("#414141"),                  //Dark
    QColor("#434343"),                  //Mid
    QColor("#c0c6d4"),                  //Text
    QColor("#3768ff"),                  //BrightText
    QColor("#c0c6d4"),                  //ButtonText
    QColor("#454545"),                  //Base
    QColor("#252525"),                  //Window
    QColor(0, 0, 0, 0.05 * 255),        //Shadow
    QColor("#0081ff"),                  //Highlight
    QColor("#b8d3ff"),                  //HighlightedText
    QColor("#0082fa"),                  //Link
    QColor("#ad4579"),                  //LinkVisited
    QColor(0, 0, 0, 0.05 * 255),        //AlternateBase
    Qt::black,                          //NoRole
    QColor(45, 45, 45, 0.8 * 255),      //ToolTipBase
    QColor("#c0c6d4")                   //ToolTipText
};

ChameleonStyle::ChameleonStyle()
    : QCommonStyle()
{

}

static void initDisablePalette(QPalette &pa)
{
    for (int i = 0; i < QPalette::NColorRoles; ++i) {
        QPalette::ColorRole role = static_cast<QPalette::ColorRole>(i);

        if (role == QPalette::Window) {
            continue;
        }

        QColor color = pa.color(QPalette::Normal, role);

        color.setAlpha(color.alpha() * 0.6);
        pa.setColor(QPalette::Disabled, role, color);
    }
}

static void initInactivePalette(QPalette &pa)
{
    for (int i = 0; i < QPalette::NColorRoles; ++i) {
        QPalette::ColorRole role = static_cast<QPalette::ColorRole>(i);

        if (role == QPalette::Window) {
            continue;
        }

        QColor color = pa.color(QPalette::Normal, role);

        color.setAlpha(color.alpha() * 0.4);
        pa.setColor(QPalette::Inactive, role, color);
    }
}

QPalette ChameleonStyle::standardPalette() const
{
    QPalette pa;
    const QColor *color_list = isDrakStyle() ? dark : light;

    for (int i = 0; i < QPalette::NColorRoles; ++i) {
        QPalette::ColorRole role = static_cast<QPalette::ColorRole>(i);

        pa.setColor(DPalette::Active, role, color_list[i]);
    }

    initDisablePalette(pa);
    initInactivePalette(pa);

    return pa;
}

static void initDisablePalette(DPalette &pa)
{
    for (int i = 0; i < DPalette::NColorTypes; ++i) {
        DPalette::ColorType role = static_cast<DPalette::ColorType>(i);
        QColor color = pa.color(DPalette::Normal, role);

        color.setAlpha(color.alpha() * 0.6);
        pa.setColor(QPalette::Disabled, role, color);
    }
}

static void initInactivePalette(DPalette &pa)
{
    for (int i = 0; i < DPalette::NColorTypes; ++i) {
        DPalette::ColorType role = static_cast<DPalette::ColorType>(i);
        QColor color = pa.color(DPalette::Normal, role);

        color.setAlpha(color.alpha() * 0.4);
        pa.setColor(QPalette::Inactive, role, color);
    }
}

static QColor light_dpalette[DPalette::NColorTypes] {
    QColor(0, 0, 0, 255 * 0.03),    //ItemBackground
    QColor("#001A2E"),              //TextTitle
    QColor("#526A7F"),              //TextTips
    QColor("#FF5736"),              //TextWarning
    QColor("#0082FA")               //TextLively
};

static QColor dark_dpalette[DPalette::NColorTypes] {
    QColor(255, 255, 255, 255 * 0.05),  //ItemBackground
    QColor("#C0C6D4"),                  //TextTitle
    QColor("#6D7C88"),                  //TextTips
    QColor("#FF5736"),                  //TextWarning
    QColor("#0082FA")                   //TextLively
};

void ChameleonStyle::polish(QApplication *app)
{
    DPalette pa = proxy()->standardPalette();
    const QColor *color_list = isDrakStyle() ? dark_dpalette : light_dpalette;

    for (int i = 0; i < DPalette::NColorTypes; ++i) {
        DPalette::ColorType type = static_cast<DPalette::ColorType>(i);

        pa.setColor(DPalette::Active, type, color_list[i]);
    }

    initDisablePalette(pa);
    initInactivePalette(pa);

    DPalette::setGeneric(pa);

    QCommonStyle::polish(app);
}

bool ChameleonStyle::isDrakStyle() const
{
    DNativeSettings theme_settings(0);

    return theme_settings.isValid() && theme_settings.getSetting("Net/ThemeName").toByteArray().contains("dark");
}
