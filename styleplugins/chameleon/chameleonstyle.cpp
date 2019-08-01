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
#include <DApplication>
#include <DPlatformWindowHandle>

#include <QVariant>
#include <QDebug>
#include <QApplication>
#include <QPushButton>
#include <QComboBox>
#include <QScrollBar>
#include <QCheckBox>
#include <QRadioButton>
#include <QToolButton>
#include <QLineEdit>
#include <QAction>
#include <QMenu>

#include <qpa/qplatformwindow.h>

DGUI_USE_NAMESPACE
DWIDGET_USE_NAMESPACE

static QColor light_qpalette[QPalette::NColorRoles] {
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

static QColor dark_qpalette[QPalette::NColorRoles] {
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

ChameleonStyle::ChameleonStyle()
    : QCommonStyle()
{

}

void ChameleonStyle::drawPrimitive(QStyle::PrimitiveElement pe, const QStyleOption *opt,
                                   QPainter *p, const QWidget *w) const
{
    QCommonStyle::drawPrimitive(pe, opt, p, w);
}

void ChameleonStyle::drawControl(QStyle::ControlElement element, const QStyleOption *opt,
                                 QPainter *p, const QWidget *w) const
{
    QCommonStyle::drawControl(element, opt, p, w);
}

QRect ChameleonStyle::subElementRect(QStyle::SubElement r, const QStyleOption *opt,
                                     const QWidget *widget) const
{
    return QCommonStyle::subElementRect(r, opt, widget);
}

void ChameleonStyle::drawComplexControl(QStyle::ComplexControl cc, const QStyleOptionComplex *opt,
                                        QPainter *p, const QWidget *w) const
{
    QCommonStyle::drawComplexControl(cc, opt, p, w);
}

QStyle::SubControl ChameleonStyle::hitTestComplexControl(QStyle::ComplexControl cc, const QStyleOptionComplex *opt,
                                                         const QPoint &pt, const QWidget *w) const
{
    return QCommonStyle::hitTestComplexControl(cc, opt, pt, w);
}

QRect ChameleonStyle::subControlRect(QStyle::ComplexControl cc, const QStyleOptionComplex *opt,
                                     QStyle::SubControl sc, const QWidget *w) const
{
    return QCommonStyle::subControlRect(cc, opt, sc, w);
}

QSize ChameleonStyle::sizeFromContents(QStyle::ContentsType ct, const QStyleOption *opt,
                                       const QSize &contentsSize, const QWidget *widget) const
{
    return QCommonStyle::sizeFromContents(ct, opt, contentsSize, widget);
}

int ChameleonStyle::pixelMetric(QStyle::PixelMetric m, const QStyleOption *opt,
                                const QWidget *widget) const
{
    return QCommonStyle::pixelMetric(m, opt, widget);
}

int ChameleonStyle::styleHint(QStyle::StyleHint sh, const QStyleOption *opt,
                              const QWidget *w, QStyleHintReturn *shret) const
{
    return QCommonStyle::styleHint(sh, opt, w, shret);
}

QIcon ChameleonStyle::standardIcon(QStyle::StandardPixmap standardIcon, const QStyleOption *opt,
                                   const QWidget *widget) const
{
    return QCommonStyle::standardIcon(standardIcon, opt, widget);
}

QPixmap ChameleonStyle::standardPixmap(QStyle::StandardPixmap sp, const QStyleOption *opt,
                                       const QWidget *widget) const
{
    return QCommonStyle::standardPixmap(sp, opt, widget);
}

QPixmap ChameleonStyle::generatedIconPixmap(QIcon::Mode iconMode, const QPixmap &pixmap,
                                            const QStyleOption *opt) const
{
    return QCommonStyle::generatedIconPixmap(iconMode, pixmap, opt);
}

int ChameleonStyle::layoutSpacing(QSizePolicy::ControlType control1, QSizePolicy::ControlType control2,
                                  Qt::Orientation orientation, const QStyleOption *option, const QWidget *widget) const
{
    return QCommonStyle::layoutSpacing(control1, control2, orientation, option, widget);
}

QPalette ChameleonStyle::standardPalette() const
{
    QPalette pa;
    const QColor *color_list = isDrakStyle() ? dark_qpalette : light_qpalette;

    for (int i = 0; i < QPalette::NColorRoles; ++i) {
        QPalette::ColorRole role = static_cast<QPalette::ColorRole>(i);

        pa.setColor(DPalette::Active, role, color_list[i]);
    }

    initDisablePalette(pa);
    initInactivePalette(pa);

    return pa;
}

void ChameleonStyle::polish(QPalette &pa)
{
    QCommonStyle::polish(pa);
}

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

void ChameleonStyle::polish(QWidget *w)
{
    QCommonStyle::polish(w);

    if (qobject_cast<QPushButton *>(w)
            || qobject_cast<QComboBox *>(w)
            || qobject_cast<QScrollBar *>(w)
            || qobject_cast<QCheckBox*>(w)
            || qobject_cast<QRadioButton*>(w)
            || qobject_cast<QToolButton*>(w)
            || qobject_cast<QAbstractSpinBox*>(w)
            || qobject_cast<QTabBar*>(w)) {
        w->setAttribute(Qt::WA_Hover, true);
    }

    if (DApplication::isDXcbPlatform()) {
        bool is_menu = qobject_cast<QMenu*>(w);
        bool is_tip = w->inherits("QTipLabel");

        // 当窗口已经创建对应的native窗口，要判断当前是否已经设置了窗口背景透明
        // Bug: https://github.com/linuxdeepin/internal-discussion/issues/323
        if (is_menu && w->windowHandle()) {
            if (const QPlatformWindow *handle = w->windowHandle()->handle()) {
                if (!w->testAttribute(Qt::WA_TranslucentBackground) && !handle->isExposed()) {
                    // 销毁现有的native窗口，否则设置Qt::WA_TranslucentBackground不会生效
                    class DQWidget : public QWidget {public: using QWidget::destroy;};
                    reinterpret_cast<DQWidget*>(w)->destroy(true, false);
                }
            }
        }

        if (is_menu) {
            DPlatformWindowHandle handle(w);

            if (DPlatformWindowHandle::isEnabledDXcb(w)) {
                handle.setEnableBlurWindow(true);
                w->setAttribute(Qt::WA_TranslucentBackground);
            }
        } else if (is_tip) {
            DPlatformWindowHandle::enableDXcbForWindow(w);
        }
    }
}

void ChameleonStyle::unpolish(QWidget *w)
{
    QCommonStyle::unpolish(w);

    if (qobject_cast<QPushButton *>(w)
            || qobject_cast<QComboBox *>(w)
            || qobject_cast<QScrollBar *>(w)
            || qobject_cast<QCheckBox*>(w)
            || qobject_cast<QRadioButton*>(w)
            || qobject_cast<QToolButton*>(w)
            || qobject_cast<QAbstractSpinBox*>(w)
            || qobject_cast<QTabBar*>(w)) {
        w->setAttribute(Qt::WA_Hover, false);
    }
}

void ChameleonStyle::unpolish(QApplication *application)
{
    QCommonStyle::unpolish(application);
}

bool ChameleonStyle::isDrakStyle() const
{
    DNativeSettings theme_settings(0);

    return theme_settings.isValid() && theme_settings.getSetting("Net/ThemeName").toByteArray().contains("dark");
}
