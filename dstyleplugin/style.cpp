/**
 * Copyright (C) 2016 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#include "style.h"
#include "common.h"
#include "geometryutils.h"
#include "paletteextended.h"
#include "commonhelper.h"
#include "dstyleanimation.h"

#include <QMenu>
#include <QLineEdit>
#include <QPushButton>
#include <QComboBox>
#include <QScrollBar>
#include <QDebug>
#include <QApplication>
#include <QWindow>
#include <QPixmapCache>
#include <QAbstractScrollArea>
#include <QRadioButton>
#include <QCheckBox>

#include <private/qhexstring_p.h>
#include <private/qdrawhelper_p.h>

#include <DApplication>
#include <dplatformwindowhandle.h>

DWIDGET_USE_NAMESPACE

namespace dstyle {
QColor StylePrivate::outline(const QPalette &pal) const
{
    if (pal.window().style() == Qt::TexturePattern)
        return QColor(0, 0, 0, 160);
    return pal.background().color().darker(140);
}

QColor StylePrivate::buttonColor(const QPalette &pal) const
{
    QColor buttonColor = pal.button().color();
    int val = qGray(buttonColor.rgb());
    buttonColor = buttonColor.lighter(100 + qMax(1, (180 - val)/6));
    buttonColor.setHsv(buttonColor.hue(), buttonColor.saturation() * 0.75, buttonColor.value());
    return buttonColor;
}

QColor StylePrivate::highlight(const QPalette &pal) const
{
    return pal.color(QPalette::Highlight);
}

QColor StylePrivate::highlightedOutline(const QPalette &pal) const
{
    QColor highlightedOutline = highlight(pal).darker(125);
    if (highlightedOutline.value() > 160)
        highlightedOutline.setHsl(highlightedOutline.hue(), highlightedOutline.saturation(), 160);
    return highlightedOutline;
}

QColor StylePrivate::backgroundColor(const QPalette &pal, const QWidget *widget) const
{
    if (qobject_cast<const QScrollBar *>(widget) && widget->parent() &&
            qobject_cast<const QAbstractScrollArea *>(widget->parent()->parent()))
        return widget->parentWidget()->parentWidget()->palette().color(QPalette::Base);
    return pal.color(QPalette::Base);
}

#ifndef QT_NO_ANIMATION
DStyleAnimation *StylePrivate::animation(const QObject *target) const
{
    return animations.value(target);
}

void StylePrivate::startAnimation(DStyleAnimation *animation) const
{
    Q_Q(const Style);
    stopAnimation(animation->target());
    q->connect(animation, SIGNAL(destroyed()), SLOT(_q_removeAnimation()), Qt::UniqueConnection);
    animations.insert(animation->target(), animation);
    animation->start();
}

void dstyle::StylePrivate::stopAnimation(const QObject *target) const
{
    DStyleAnimation *animation = animations.take(target);
    if (animation) {
        animation->stop();
        delete animation;
    }
}

void StylePrivate::_q_removeAnimation()
{
    Q_Q(Style);
    QObject *animation = q->sender();
    if (animation)
        animations.remove(animation->parent());
}
#endif

Style::Style(StyleType style)
    : QCommonStyle(*new StylePrivate())
    , m_type(style)
{
    m_palette = PaletteExtended::instance(style);
}

Style::~Style()
{

}

void Style::polish(QPalette &p)
{
    QCommonStyle::polish(p);

    m_palette->polish(p);
}

void Style::polish(QWidget *w)
{
    QCommonStyle::polish(w);

    if (qobject_cast<QPushButton *>(w)
            || qobject_cast<QComboBox *>(w)
            || qobject_cast<QScrollBar *>(w)
            || qobject_cast<QCheckBox*>(w)
            || qobject_cast<QRadioButton*>(w))
        w->setAttribute(Qt::WA_Hover, true);

    if (qobject_cast<QScrollBar *>(w)) {
        w->setAttribute(Qt::WA_OpaquePaintEvent, false);
    }

    if (w->inherits("dcc::widgets::SliderAnnotation")) {
        QFont font = w->font();
        font.setPointSizeF(qApp->font().pointSizeF() - 1);
        w->setFont(font);
    }

    if (DApplication::isDXcbPlatform() && qobject_cast<QMenu*>(w)
            && (!w->windowHandle() || DPlatformWindowHandle::isEnabledDXcb(w))) {
        DPlatformWindowHandle handle(w);

        const QColor &color = m_palette->brush(PaletteExtended::Menu_BorderColor).color();

        if (color.isValid())
            handle.setBorderColor(color);

        handle.setShadowOffset(QPoint(0, 4));
        handle.setShadowRadius(15);
        handle.setShadowColor(QColor(0, 0, 0, 100));
        handle.setEnableBlurWindow(true);
        handle.setTranslucentBackground(true);

        w->setAttribute(Qt::WA_TranslucentBackground);
    }

    // ###(zccrs): If the application is DApplication or derived class of it then not draw shortcut text
    w->setProperty(QT_STRINGIFY(_d_hideShortcutText), (bool)qobject_cast<DApplication*>(qApp));
    w->setProperty(QT_STRINGIFY(_d_drawMenuCheckBox), !(bool)qobject_cast<DApplication*>(qApp));

    QPalette palette = w->palette();

    if (w->testAttribute(Qt::WA_SetStyle)) {
        polish(palette);
    }

    // combo box drop-list.
    // line edit completer drop-list
    if (widgetIsComboBoxPopupFramePrimitive(w)) {
        polish(palette);
        palette.setBrush(QPalette::Background, m_palette->brush(PaletteExtended::Menu_BackgroundBrush,
                                                                PaletteExtended::PseudoClass_Unspecified,
                                                                palette.brush(QPalette::Background)));
    }

    w->setPalette(palette);
}

void Style::unpolish(QWidget *w)
{
    if (qobject_cast<QPushButton *>(w)
            || qobject_cast<QComboBox *>(w)
            || qobject_cast<QScrollBar *>(w)
            || qobject_cast<QCheckBox*>(w)
            || qobject_cast<QRadioButton*>(w))
        w->setAttribute(Qt::WA_Hover, false);

    if (qobject_cast<QScrollBar *>(w)) {
        w->setAttribute(Qt::WA_OpaquePaintEvent, true);
    }

    if (w->inherits("dcc::widgets::SliderAnnotation")) {
        QFont font = w->font();
        font.setPointSizeF(qApp->font().pointSizeF());
        w->setFont(font);
        w->setAttribute(Qt::WA_SetFont, false);
    }
}

int Style::pixelMetric(QStyle::PixelMetric metric, const QStyleOption *option, const QWidget *widget) const
{
    // handle special cases
    switch( metric )
    {

    // frame width
    case PM_DefaultFrameWidth: {
        if( qobject_cast<const QMenu*>( widget ) ) return Metrics::Menu_FrameWidth;
        if( qobject_cast<const QLineEdit*>( widget ) ) return Metrics::LineEdit_FrameWidth;

        // fallback
        return Metrics::Frame_FrameWidth;
    }

    case PM_ComboBoxFrameWidth: {
        const QStyleOptionComboBox* comboBoxOption( qstyleoption_cast< const QStyleOptionComboBox*>( option ) );
        return comboBoxOption && comboBoxOption->editable ? Metrics::LineEdit_FrameWidth : Metrics::ComboBox_FrameWidth;
    }

    case PM_SpinBoxFrameWidth: return Metrics::SpinBox_FrameWidth;
    case PM_ToolBarFrameWidth: return Metrics::ToolBar_FrameWidth;
    case PM_ToolTipLabelFrameWidth: return Metrics::ToolTip_FrameWidth;

    case PM_LayoutHorizontalSpacing: return Metrics::Layout_DefaultSpacing;
    case PM_LayoutVerticalSpacing: return Metrics::Layout_DefaultSpacing;

        // buttons
    case PM_ButtonMargin:
    {
        // needs special case for kcalc buttons, to prevent the application to set too small margins
        if( widget && widget->inherits( "KCalcButton" ) ) return Metrics::Button_MarginWidth + 4;
        else return Metrics::Button_MarginWidth;
    }

    case PM_ButtonDefaultIndicator: return 0;
    case PM_ButtonShiftHorizontal: return 0;
    case PM_ButtonShiftVertical: return 0;

        //menu
    case PM_MenuHMargin: return Menu_HMargin;
    case PM_MenuVMargin: return Menu_VMargin;

        // menubars
    case PM_MenuBarPanelWidth: return 0;
    case PM_MenuBarHMargin: return 0;
    case PM_MenuBarVMargin: return 0;
    case PM_MenuBarItemSpacing: return Metrics::MenuBar_ItemSpacing;
    case PM_MenuDesktopFrameWidth: return 0;

        // menu buttons
    case PM_MenuButtonIndicator: return Metrics::MenuButton_IndicatorWidth;

        // toolbars
    case PM_ToolBarHandleExtent: return Metrics::ToolBar_HandleExtent;
    case PM_ToolBarSeparatorExtent: return Metrics::ToolBar_SeparatorWidth;
    case PM_ToolBarExtensionExtent:
        return pixelMetric( PM_SmallIconSize, option, widget ) + 2*Metrics::ToolButton_MarginWidth;

    case PM_ToolBarItemMargin: return 0;
    case PM_ToolBarItemSpacing: return Metrics::ToolBar_ItemSpacing;

        // tabbars
    case PM_TabBarTabShiftVertical: return 0;
    case PM_TabBarTabShiftHorizontal: return 0;
    case PM_TabBarTabOverlap: return Metrics::TabBar_TabOverlap;
    case PM_TabBarBaseOverlap: return Metrics::TabBar_BaseOverlap;
    case PM_TabBarTabHSpace: return 2*Metrics::TabBar_TabMarginWidth;
    case PM_TabBarTabVSpace: return 2*Metrics::TabBar_TabMarginHeight;
    case PM_TabCloseIndicatorWidth:
    case PM_TabCloseIndicatorHeight:
        return pixelMetric( PM_SmallIconSize, option, widget );

        // scrollbars
    case PM_ScrollBarExtent: return Metrics::ScrollBar_Extend;
    case PM_ScrollBarSliderMin: return Metrics::ScrollBar_MinSliderHeight;

        // title bar
    case PM_TitleBarHeight: return 2*Metrics::TitleBar_MarginWidth + pixelMetric( PM_SmallIconSize, option, widget );

        // sliders
    case PM_SliderThickness: return Metrics::Slider_ControlThickness;
    case PM_SliderControlThickness: return Metrics::Slider_ControlThickness;
    case PM_SliderLength: return Metrics::Slider_ControlThickness;

        // checkboxes and radio buttons
    case PM_IndicatorWidth: return Metrics::CheckBox_Size;
    case PM_IndicatorHeight: return Metrics::CheckBox_Size;
    case PM_ExclusiveIndicatorWidth: return Metrics::CheckBox_Size;
    case PM_ExclusiveIndicatorHeight: return Metrics::CheckBox_Size;

        // list heaaders
    case PM_HeaderMarkSize: return Metrics::Header_ArrowSize;
    case PM_HeaderMargin: return Metrics::Header_MarginWidth;

        // dock widget
        // return 0 here, since frame is handled directly in polish
    case PM_DockWidgetFrameWidth: return 0;
    case PM_DockWidgetTitleMargin: return Metrics::Frame_FrameWidth;
    case PM_DockWidgetTitleBarButtonMargin: return Metrics::ToolButton_MarginWidth;

    case PM_SplitterWidth: return Metrics::Splitter_SplitterWidth;
    case PM_DockWidgetSeparatorExtent: return Metrics::Splitter_SplitterWidth;

        // fallback
    default: return QCommonStyle::pixelMetric( metric, option, widget );

    }
}

QRect Style::subControlRect(QStyle::ComplexControl cc, const QStyleOptionComplex *opt, QStyle::SubControl sc, const QWidget *w) const
{
    //    SubControlRectFunc fn = nullptr;

    switch (cc) {
    case QStyle::CC_Slider: return sliderSubControlRect(opt, sc, w);
    case QStyle::CC_ScrollBar: return scrollbarSubControlRect(opt, sc, w);
#ifndef QT_NO_COMBOBOX
    case QStyle::CC_ComboBox: return comboBoxSubControlRect(opt, sc, w);
#endif
    default:
        break;
    }

    //    if (fn) {
    //        QRect rect = fn(opt, sc, w);
    //        if (!rect.isEmpty()) {
    //            return rect;
    //        }
    //    }

    return QCommonStyle::subControlRect(cc, opt, sc, w);
}

QRect Style::subElementRect(QStyle::SubElement r, const QStyleOption *opt, const QWidget *widget) const
{
    switch (r) {
    case SE_ProgressBarGroove: return progressBarGrooveRect( opt, widget );
    case SE_ProgressBarContents: return progressBarContentsRect( opt, widget );
    case SE_ProgressBarLabel: return progressBarLabelRect( opt, widget );
    case SE_HeaderArrow: return headerArrowRect( opt, widget );
    default:
        break;
    }

    return QCommonStyle::subElementRect(r, opt, widget);
}

void Style::drawControl(QStyle::ControlElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget) const
{
    DrawControlFunc fcn(Q_NULLPTR);

    switch( element ) {
    case CE_PushButtonBevel: fcn = &Style::drawPushButtonBevel; break;
    case CE_PushButtonLabel: fcn = &Style::drawPushButtonLabel; break;
        //        case CE_CheckBoxLabel: fcn = &Style::drawCheckBoxLabelControl; break;
        //        case CE_RadioButtonLabel: fcn = &Style::drawCheckBoxLabelControl; break;
        //        case CE_ToolButtonLabel: fcn = &Style::drawToolButtonLabelControl; break;
    case CE_ComboBoxLabel: fcn = &Style::drawComboBoxLabelControl; break;
    case CE_MenuBarEmptyArea: return;
    case CE_MenuBarItem: fcn = &Style::drawMenuBarItemControl; break;
    case CE_MenuItem: fcn = &Style::drawMenuItemControl; break;
        //        case CE_ToolBar: fcn = &Style::emptyControl; break;
    case CE_ProgressBar: fcn = &Style::drawProgressBarControl; break;
    case CE_ProgressBarContents: fcn = &Style::drawProgressBarContentsControl; break;
    case CE_ProgressBarGroove: fcn = &Style::drawProgressBarGrooveControl; break;
    case CE_ProgressBarLabel: fcn = &Style::drawProgressBarLabelControl; break;
    case CE_ScrollBarSlider: fcn = &Style::drawScrollBarSliderControl; break;
        //        case CE_ScrollBarAddLine: fcn = &Style::drawScrollBarAddLineControl; break;
        //        case CE_ScrollBarSubLine: fcn = &Style::drawScrollBarSubLineControl; break;
        //        case CE_ScrollBarAddPage: fcn = &Style::emptyControl; break;
        //        case CE_ScrollBarSubPage: fcn = &Style::emptyControl; break;
    case CE_ShapedFrame: fcn = &Style::drawShapedFrameControl; break;
    case CE_RubberBand: fcn = &Style::drawRubberBandControl; break;
        //        case CE_SizeGrip: fcn = &Style::emptyControl; break;
        //        case CE_HeaderSection: fcn = &Style::drawHeaderSectionControl; break;
        //        case CE_HeaderEmptyArea: fcn = &Style::drawHeaderEmptyAreaControl; break;
        //        case CE_TabBarTabLabel: fcn = &Style::drawTabBarTabLabelControl; break;
        //        case CE_TabBarTabShape: fcn = &Style::drawTabBarTabShapeControl; break;
        //        case CE_ToolBoxTabLabel: fcn = &Style::drawToolBoxTabLabelControl; break;
        //        case CE_ToolBoxTabShape: fcn = &Style::drawToolBoxTabShapeControl; break;
        //        case CE_DockWidgetTitle: fcn = &Style::drawDockWidgetTitleControl; break;
        //        case CE_CapacityBar: fcn = &Style::drawProgressBarControl; break;

        // fallback
    default: {
        // TODO: move this bare number comparison to some more human friendly form.
        if ((unsigned int)(element) == QStyle::CE_CustomBase + 1024) {
            fcn = &Style::drawSliderTickmarkLabels;
        }
    }
    }

    painter->save();

    // call function if implemented
    if( !( fcn && (this->*fcn)( option, painter, widget ) ) )
    { QCommonStyle::drawControl( element, option, painter, widget ); }

    painter->restore();
}

void Style::drawComplexControl(QStyle::ComplexControl cc, const QStyleOptionComplex *opt, QPainter *p, const QWidget *w) const
{
    DrawComplexControlFunc fn = nullptr;

    switch (cc) {
    case QStyle::CC_Slider: fn = &Style::drawSlider; break;
#ifndef QT_NO_COMBOBOX
    case QStyle::CC_ComboBox: fn = &Style::drawComboBox; break;
#endif
    default:
        break;
    }

    if (fn && (this->*fn)(cc, opt, p, w)) {
        return;
    }

    return QCommonStyle::drawComplexControl(cc, opt, p, w);
}

void Style::drawPrimitive(QStyle::PrimitiveElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget) const
{
    DrawPrimitiveFunc fcn( nullptr );
    switch( element )
    {
    //    case PE_PanelButtonCommand: fcn = &Style::drawPanelButtonCommandPrimitive; break;
    //    case PE_PanelButtonTool: fcn = &Style::drawPanelButtonToolPrimitive; break;
    //    case PE_PanelScrollAreaCorner: fcn = &Style::drawPanelScrollAreaCornerPrimitive; break;
    case PE_PanelMenu: {
        QBrush menu_background_brush = m_palette->brush(PaletteExtended::Menu_BackgroundBrush,
                                                        PaletteExtended::PseudoClass_Unspecified,
                                                        option->palette.brush(QPalette::Background));
        QColor menu_background_color = menu_background_brush.color();

        if (DPlatformWindowHandle::isEnabledDXcb(const_cast<QWidget*>(widget)) && menu_background_color.isValid()) {
            DPlatformWindowHandle handle(const_cast<QWidget*>(widget));

            if (handle.enableBlurWindow()) {
                menu_background_color.setAlphaF(DPlatformWindowHandle::hasBlurWindow() ? 0.4 : menu_background_color.alphaF());
            }

            menu_background_brush.setColor(menu_background_color);
        }

        return painter->fillRect(option->rect, menu_background_brush);
    }
        //    case PE_PanelTipLabel: fcn = &Style::drawPanelTipLabelPrimitive; break;
        //    case PE_PanelItemViewItem: fcn = &Style::drawPanelItemViewItemPrimitive; break;
    case PE_IndicatorCheckBox: fcn = &Style::drawIndicatorCheckBoxPrimitive; break;
    case PE_IndicatorRadioButton:
        fillBrush(painter, option->rect, m_palette->brush(PaletteExtended::RadioButton_BackgroundBrush, option));
        return;
        //    case PE_IndicatorButtonDropDown: fcn = &Style::drawIndicatorButtonDropDownPrimitive; break;
        //    case PE_IndicatorTabClose: fcn = &Style::drawIndicatorTabClosePrimitive; break;
        //    case PE_IndicatorTabTear: fcn = &Style::drawIndicatorTabTearPrimitive; break;
    case PE_IndicatorArrowDown:
        return drawStandardIcon(QStyle::SP_ArrowDown, option, painter, widget);
    case PE_IndicatorArrowLeft:
        return drawStandardIcon(QStyle::SP_ArrowLeft, option, painter, widget);
    case PE_IndicatorArrowRight:
        return drawStandardIcon(QStyle::SP_ArrowRight, option, painter, widget);
    case PE_IndicatorArrowUp:
        return drawStandardIcon(QStyle::SP_ArrowUp, option, painter, widget);
    case PE_IndicatorHeaderArrow: fcn = &Style::drawIndicatorHeaderArrowPrimitive; break;
        //    case PE_IndicatorToolBarHandle: fcn = &Style::drawIndicatorToolBarHandlePrimitive; break;
        //    case PE_IndicatorToolBarSeparator: fcn = &Style::drawIndicatorToolBarSeparatorPrimitive; break;
        //    case PE_IndicatorBranch: fcn = &Style::drawIndicatorBranchPrimitive; break;
        //    case PE_FrameStatusBar: fcn = &Style::emptyPrimitive; break;
    case PE_Frame: fcn = &Style::drawFramePrimitive; break;
        //    case PE_FrameMenu: fcn = &Style::drawFrameMenuPrimitive; break;
    case PE_FrameLineEdit: fcn = &Style::drawFrameLineEditPrimitive; break;
        //    case PE_FrameGroupBox: fcn = &Style::drawFrameGroupBoxPrimitive; break;
        //    case PE_FrameTabWidget: fcn = &Style::drawFrameTabWidgetPrimitive; break;
        //    case PE_FrameTabBarBase: fcn = &Style::drawFrameTabBarBasePrimitive; break;
        //    case PE_FrameWindow: fcn = &Style::drawFrameWindowPrimitive; break;
    case PE_FrameFocusRect: fcn = &Style::drawFrameFocusRectPrimitive; break;

        // fallback
    default: break;

    }

    painter->save();

    // call function if implemented
    if( !( fcn && (this->*fcn)( option, painter, widget ) ) )
    { QCommonStyle::drawPrimitive( element, option, painter, widget ); }

    painter->restore();
}

int Style::styleHint(QStyle::StyleHint sh, const QStyleOption *opt, const QWidget *w, QStyleHintReturn *shret) const
{
    switch (sh) {
    case SH_ScrollBar_Transient: return true;
    case SH_ComboBox_ListMouseTracking: return true;
    case SH_MenuBar_MouseTracking: return true;
    case SH_Menu_MouseTracking: return true;
    case SH_Menu_SubMenuPopupDelay: return 100;
    case QStyle::SH_Menu_Scrollable: return true;
    case SH_Menu_SloppySubMenus: return true;
#if QT_VERSION >= QT_VERSION_CHECK(5, 5, 0)
    case SH_Menu_SubMenuUniDirection: return 1000;
#endif
    case SH_Slider_AbsoluteSetButtons: return Qt::LeftButton;
    case SH_Slider_PageSetButtons: return Qt::MidButton;
#if QT_VERSION >= QT_VERSION_CHECK(5, 2, 0)
    case SH_Widget_Animate: return true;
#endif
    case QStyle::SH_ItemView_ShowDecorationSelected: return true;
    case SH_RubberBand_Mask:
        return 0;
    case SH_ComboBox_Popup:
        return true;
    default:
        break;
    }

    return QCommonStyle::styleHint(sh, opt, w, shret);
}

QSize Style::sizeFromContents(QStyle::ContentsType type, const QStyleOption *option, const QSize &size, const QWidget *widget) const
{
    QSize newSize = QCommonStyle::sizeFromContents(type, option, size, widget);
    switch (type) {
    case CT_PushButton:
        if (const QStyleOptionButton *btn = qstyleoption_cast<const QStyleOptionButton *>(option)) {
            const bool flat = btn->features & QStyleOptionButton::Flat;
            const bool semiTransparent = m_type == StyleSemiDark || m_type == StyleSemiLight;
            if (!btn->text.isEmpty() && !flat) {
                if (newSize.width() < 80)
                    newSize.setWidth(80);
                // QPushButton should be set a minimum height 36 in d-semi-transparent style.
                if (semiTransparent && newSize.height() < 36)
                    newSize.setHeight(36);
            }
            if (!btn->icon.isNull() && btn->iconSize.height() > 16)
                newSize -= QSize(0, 2);
        }
        break;
    case CT_GroupBox:
        if (option) {
            int topMargin = qMax(pixelMetric(PM_ExclusiveIndicatorHeight), option->fontMetrics.height()) + GroupBox_TopMargin;
            newSize += QSize(10, topMargin); // Add some space below the groupbox
        }
        break;
    case CT_RadioButton:
    case CT_CheckBox:
        newSize += QSize(0, 1);
        break;
    case CT_ToolButton:
        newSize += QSize(2, 2);
        break;
    case CT_SpinBox:
        newSize += QSize(0, -3);
        break;
    case CT_ComboBox:
//        newSize += QSize(2, 4);
        break;
    case CT_LineEdit:
        newSize += QSize(0, 4);
        break;
    case CT_MenuBarItem:
        newSize += QSize(8, 5);
        break;
    case CT_MenuItem:
        if (const QStyleOptionMenuItem *menuItem = qstyleoption_cast<const QStyleOptionMenuItem *>(option)) {
            int w = newSize.width();
            bool hideShortcutText = widget->property(QT_STRINGIFY(_d_hideShortcutText)).toBool();

            if (hideShortcutText) {
                w -= menuItem->tabWidth;

                int t = menuItem->text.indexOf(QLatin1Char('\t'));
                if (t != -1) {
                    int textWidth = option->fontMetrics.width(menuItem->text.mid(t + 1));
                    w -= textWidth;

                    if (menuItem->tabWidth == 0)
                        w -= textWidth;
                }
            }

            int maxpmw = menuItem->maxIconWidth;
            int tabSpacing = 20;
            if (menuItem->text.contains(QLatin1Char('\t'))) {
                if (!hideShortcutText)
                    w += tabSpacing;
            } else {
                if (menuItem->menuItemType == QStyleOptionMenuItem::SubMenu) {
                    w += 2 * Menu_ArrowHMargin;
                } else if (menuItem->menuItemType == QStyleOptionMenuItem::DefaultItem) {
                    QFontMetrics fm(menuItem->font);
                    QFont fontBold = menuItem->font;
                    fontBold.setBold(true);
                    QFontMetrics fmBold(fontBold);
                    w += fmBold.width(menuItem->text) - fm.width(menuItem->text);
                }
            }
            int checkcol = qMax<int>(maxpmw, Menu_CheckMarkWidth); // Windows always shows a check column
            w += checkcol;
            w += Menu_RightBorder + 10;
            newSize.setWidth(w);
            if (menuItem->menuItemType == QStyleOptionMenuItem::Separator) {
                if (!menuItem->text.isEmpty()) {
                    newSize.setHeight(menuItem->fontMetrics.height());
                } else {
                    newSize.setHeight(newSize.height() + 4);
                }
            }
            else if (!menuItem->icon.isNull()) {
                if (const QComboBox *combo = qobject_cast<const QComboBox*>(widget)) {
                    newSize.setHeight(qMax(combo->iconSize().height() + 2, newSize.height()));
                }
            }
            newSize.setWidth(newSize.width() + 12);
            newSize.setWidth(qMax(newSize.width(), 80));
        }

        newSize.setWidth(newSize.width() + Menu_ItemHMargin * 2);
        newSize.setHeight(newSize.height() + Menu_ItemVMargin * 2 + 1);
        break;
    case CT_SizeGrip:
        newSize += QSize(4, 4);
        break;
    case CT_MdiControls:
        if (const QStyleOptionComplex *styleOpt = qstyleoption_cast<const QStyleOptionComplex *>(option)) {
            int width = 0;
            if (styleOpt->subControls & SC_MdiMinButton)
                width += 19 + 1;
            if (styleOpt->subControls & SC_MdiNormalButton)
                width += 19 + 1;
            if (styleOpt->subControls & SC_MdiCloseButton)
                width += 19 + 1;
            newSize = QSize(width, 19);
        } else {
            newSize = QSize(60, 19);
        }
        break;
    default:
        break;
    }
    return newSize;
}

QIcon Style::standardIcon(QStyle::StandardPixmap standardIcon, const QStyleOption *opt, const QWidget *widget) const
{
    QIcon icon;
    const bool rtl = (opt && opt->direction == Qt::RightToLeft) || (!opt && QApplication::isRightToLeft());
    QString themeType = m_type == StyleLight ? "light" : "dark";

    if (QApplication::desktopSettingsAware() && !QIcon::themeName().isEmpty() && false) {
        switch (standardIcon) {
        case SP_DirHomeIcon:
            icon = QIcon::fromTheme(QLatin1String("user-home"));
            break;
        case SP_MessageBoxInformation:
            icon = QIcon::fromTheme(QLatin1String("dialog-information"));
            break;
        case SP_MessageBoxWarning:
            icon = QIcon::fromTheme(QLatin1String("dialog-warning"));
            break;
        case SP_MessageBoxCritical:
            icon = QIcon::fromTheme(QLatin1String("dialog-error"));
            break;
        case SP_MessageBoxQuestion:
            icon = QIcon::fromTheme(QLatin1String("dialog-question"));
            break;
        case SP_DialogOpenButton:
        case SP_DirOpenIcon:
            icon = QIcon::fromTheme(QLatin1String("folder-open"));
            break;
        case SP_DialogSaveButton:
            icon = QIcon::fromTheme(QLatin1String("document-save"));
            break;
        case SP_DialogApplyButton:
            icon = QIcon::fromTheme(QLatin1String("dialog-ok-apply"));
            break;
        case SP_DialogYesButton:
        case SP_DialogOkButton:
            icon = QIcon::fromTheme(QLatin1String("dialog-ok"));
            break;
        case SP_DialogDiscardButton:
            icon = QIcon::fromTheme(QLatin1String("edit-delete"));
            break;
        case SP_DialogResetButton:
            icon = QIcon::fromTheme(QLatin1String("edit-clear"));
            break;
        case SP_DialogHelpButton:
            icon = QIcon::fromTheme(QLatin1String("help-contents"));
            break;
        case SP_FileIcon:
            icon = QIcon::fromTheme(QLatin1String("text-x-generic"));
            break;
        case SP_DirClosedIcon:
        case SP_DirIcon:
            icon = QIcon::fromTheme(QLatin1String("folder"));
            break;
        case SP_DriveFDIcon:
            icon = QIcon::fromTheme(QLatin1String("floppy_unmount"));
            break;
        case SP_ComputerIcon:
            icon = QIcon::fromTheme(QLatin1String("computer"),
                                    QIcon::fromTheme(QLatin1String("system")));
            break;
        case SP_DesktopIcon:
            icon = QIcon::fromTheme(QLatin1String("user-desktop"));
            break;
        case SP_TrashIcon:
            icon = QIcon::fromTheme(QLatin1String("user-trash"));
            break;
        case SP_DriveCDIcon:
        case SP_DriveDVDIcon:
            icon = QIcon::fromTheme(QLatin1String("media-optical"));
            break;
        case SP_DriveHDIcon:
            icon = QIcon::fromTheme(QLatin1String("drive-harddisk"));
            break;
        case SP_FileDialogToParent:
            icon = QIcon::fromTheme(QLatin1String("go-up"));
            break;
        case SP_FileDialogNewFolder:
            icon = QIcon::fromTheme(QLatin1String("folder-new"));
            break;
        case SP_ArrowUp:
            icon = QIcon::fromTheme(QLatin1String("go-up"));
            break;
        case SP_ArrowDown:
            icon = QIcon::fromTheme(QLatin1String("go-down"));
            break;
        case SP_ArrowRight:
            icon = QIcon::fromTheme(QLatin1String("go-next"));
            break;
        case SP_ArrowLeft:
            icon = QIcon::fromTheme(QLatin1String("go-previous"));
            break;
        case SP_DialogCancelButton:
            icon = QIcon::fromTheme(QLatin1String("dialog-cancel"),
                                    QIcon::fromTheme(QLatin1String("process-stop")));
            break;
        case SP_DialogCloseButton:
            icon = QIcon::fromTheme(QLatin1String("window-close"));
            break;
        case SP_FileDialogDetailedView:
            icon = QIcon::fromTheme(QLatin1String("view-list-details"));
            break;
        case SP_FileDialogListView:
            icon = QIcon::fromTheme(QLatin1String("view-list-icons"));
            break;
        case SP_BrowserReload:
            icon = QIcon::fromTheme(QLatin1String("view-refresh"));
            break;
        case SP_BrowserStop:
            icon = QIcon::fromTheme(QLatin1String("process-stop"));
            break;
        case SP_MediaPlay:
            icon = QIcon::fromTheme(QLatin1String("media-playback-start"));
            break;
        case SP_MediaPause:
            icon = QIcon::fromTheme(QLatin1String("media-playback-pause"));
            break;
        case SP_MediaStop:
            icon = QIcon::fromTheme(QLatin1String("media-playback-stop"));
            break;
        case SP_MediaSeekForward:
            icon = QIcon::fromTheme(QLatin1String("media-seek-forward"));
            break;
        case SP_MediaSeekBackward:
            icon = QIcon::fromTheme(QLatin1String("media-seek-backward"));
            break;
        case SP_MediaSkipForward:
            icon = QIcon::fromTheme(QLatin1String("media-skip-forward"));
            break;
        case SP_MediaSkipBackward:
            icon = QIcon::fromTheme(QLatin1String("media-skip-backward"));
            break;
        case SP_MediaVolume:
            icon = QIcon::fromTheme(QLatin1String("audio-volume-medium"));
            break;
        case SP_MediaVolumeMuted:
            icon = QIcon::fromTheme(QLatin1String("audio-volume-muted"));
            break;
        case SP_ArrowForward:
            if (rtl)
                return Style::standardIcon(SP_ArrowLeft, opt, widget);
            return Style::standardIcon(SP_ArrowRight, opt, widget);
        case SP_ArrowBack:
            if (rtl)
                return Style::standardIcon(SP_ArrowRight, opt, widget);
            return Style::standardIcon(SP_ArrowLeft, opt, widget);
        case SP_FileLinkIcon:
        {
            QIcon linkIcon = QIcon::fromTheme(QLatin1String("emblem-symbolic-link"));
            if (!linkIcon.isNull()) {
                QIcon baseIcon = Style::standardIcon(SP_FileIcon, opt, widget);
                const QList<QSize> sizes = baseIcon.availableSizes(QIcon::Normal, QIcon::Off);
                for (int i = 0 ; i < sizes.size() ; ++i) {
                    int size = sizes[i].width();
                    QPixmap basePixmap = baseIcon.pixmap(qt_getWindow(widget), QSize(size, size));
                    QPixmap linkPixmap = linkIcon.pixmap(qt_getWindow(widget), QSize(size / 2, size / 2));
                    QPainter painter(&basePixmap);
                    painter.drawPixmap(size/2, size/2, linkPixmap);
                    icon.addPixmap(basePixmap);
                }
            }
        }
            break;
        case SP_DirLinkIcon:
        {
            QIcon linkIcon = QIcon::fromTheme(QLatin1String("emblem-symbolic-link"));
            if (!linkIcon.isNull()) {
                QIcon baseIcon = Style::standardIcon(SP_DirIcon, opt, widget);
                const QList<QSize> sizes = baseIcon.availableSizes(QIcon::Normal, QIcon::Off);
                for (int i = 0 ; i < sizes.size() ; ++i) {
                    int size = sizes[i].width();
                    QPixmap basePixmap = baseIcon.pixmap(qt_getWindow(widget), QSize(size, size));
                    QPixmap linkPixmap = linkIcon.pixmap(qt_getWindow(widget), QSize(size / 2, size / 2));
                    QPainter painter(&basePixmap);
                    painter.drawPixmap(size/2, size/2, linkPixmap);
                    icon.addPixmap(basePixmap);
                }
            }
        }
            break;
        default:
            break;
        }
    } // if (QApplication::desktopSettingsAware() && !QIcon::themeName().isEmpty())

    if (!icon.isNull())
        return icon;

    switch (standardIcon) {
    case SP_FileDialogNewFolder:
        break;
    case SP_FileDialogBack:
        return Style::standardIcon(SP_ArrowBack, opt, widget);
    case SP_FileDialogToParent:
        return Style::standardIcon(SP_ArrowUp, opt, widget);
    case SP_FileDialogDetailedView:
        break;
    case SP_FileDialogInfoView:
        break;
    case SP_FileDialogContentsView:
        break;
    case SP_FileDialogListView:
        break;
    case SP_DialogOkButton:
    case SP_DialogYesButton:
        icon.addFile(QLatin1String(":/assets/icons/dialog-apply.png"));
        break;
    case SP_DialogCancelButton:
    case SP_DialogNoButton:
        icon.addFile(QLatin1String(":/assets/icons/button-cancel.png"));
        break;
    case SP_DialogHelpButton:
        icon.addFile(QLatin1String(":/assets/icons/help-faq.png"));
        break;
    case SP_DialogOpenButton:
        break;
    case SP_DialogSaveButton:
        break;
    case SP_DialogCloseButton:
        return Style::standardIcon(SP_TitleBarCloseButton, opt, widget);
    case SP_DialogApplyButton:
        icon.addFile(QLatin1String(":/assets/icons/dialog-apply.png"));
        break;
    case SP_DialogResetButton:
        break;
    case SP_DialogDiscardButton:
        break;
    case SP_ArrowForward:
        if (rtl)
            return Style::standardIcon(SP_ArrowLeft, opt, widget);
        return Style::standardIcon(SP_ArrowRight, opt, widget);
    case SP_ArrowBack:
        if (rtl)
            return Style::standardIcon(SP_ArrowRight, opt, widget);
        return Style::standardIcon(SP_ArrowLeft, opt, widget);
    case SP_ArrowLeft:
        icon.addFile(QLatin1String(":/assets/icons/arrow-left.png"));
        break;
    case SP_ArrowRight:
        icon.addFile(QLatin1String(":/assets/icons/arrow-right.png"));
        break;
    case SP_ArrowUp:
        icon.addFile(QLatin1String(":/assets/icons/arrow-up.png"));
        break;
    case SP_ArrowDown:
        icon.addFile(QLatin1String(":/assets/icons/arrow-down.png"));
        break;
    case SP_DirHomeIcon:
        icon.addFile(QLatin1String(":/assets/icons/user-home.svg"));
        break;
    case SP_DirIcon:
        icon.addFile(QLatin1String(":/assets/icons/folder.svg"));
        break;
    case SP_DirOpenIcon:
        icon.addFile(QLatin1String(":/assets/icons/folder-open.svg"));
        break;
    case SP_DirLinkIcon: {
        QIcon linkIcon(":/assets/icons/emblem-symbolic-link.png");
        QIcon baseIcon = Style::standardIcon(SP_DirIcon, opt, widget);
        QList<QSize> sizes;
        sizes << QSize(128, 128);
        for (int i = 0 ; i < sizes.size() ; ++i) {
            int size = sizes[i].width();
            QPixmap basePixmap = baseIcon.pixmap(qt_getWindow(widget), QSize(size, size));
            QPixmap linkPixmap = linkIcon.pixmap(qt_getWindow(widget), QSize(size / 2, size / 2));
            QPainter painter(&basePixmap);
            painter.drawPixmap(size/1.5, size/1.5, linkPixmap);
            icon.addPixmap(basePixmap);
        }
        break;
    }
    case SP_DirLinkOpenIcon: {
        QIcon linkIcon(":/assets/icons/emblem-symbolic-link.png");
        QIcon baseIcon = Style::standardIcon(SP_DirOpenIcon, opt, widget);
        QList<QSize> sizes;
        sizes << QSize(128, 128);
        for (int i = 0 ; i < sizes.size() ; ++i) {
            int size = sizes[i].width();
            QPixmap basePixmap = baseIcon.pixmap(qt_getWindow(widget), QSize(size, size));
            QPixmap linkPixmap = linkIcon.pixmap(qt_getWindow(widget), QSize(size / 2, size / 2));
            QPainter painter(&basePixmap);
            painter.drawPixmap(size/1.5, size/1.5, linkPixmap);
            icon.addPixmap(basePixmap);
        }
        break;
    }
    case SP_DriveCDIcon:
    case SP_DriveDVDIcon:
        icon.addFile(QLatin1String(":/assets/icons/media-optical.png"));
        break;
    case SP_DriveHDIcon:
        icon.addFile(QLatin1String(":/assets/icons/drive-harddisk.png"));
        break;
    case SP_FileIcon:
        icon.addFile(QLatin1String(":/assets/icons/text-x-generic.svg"));
        break;
    case SP_FileLinkIcon: {
        QIcon linkIcon(":/assets/icons/emblem-symbolic-link.png");
        QIcon baseIcon = Style::standardIcon(SP_FileIcon, opt, widget);
        QList<QSize> sizes;
        sizes << QSize(128, 128);
        for (int i = 0 ; i < sizes.size() ; ++i) {
            int size = sizes[i].width();
            QPixmap basePixmap = baseIcon.pixmap(qt_getWindow(widget), QSize(size, size));
            QPixmap linkPixmap = linkIcon.pixmap(qt_getWindow(widget), QSize(size / 2, size / 2));
            QPainter painter(&basePixmap);
            painter.drawPixmap(size/1.5, size/1.5, linkPixmap);
            icon.addPixmap(basePixmap);
        }
        break;
    }
    case SP_TrashIcon:
        icon.addFile(QLatin1String(":/assets/icons/user-trash.svg"));
        break;
    case SP_BrowserReload:
        icon.addFile(QLatin1String(":/assets/icons/view-refresh.png"));
        break;
    case SP_BrowserStop:
        icon.addFile(QLatin1String(":/assets/icons/process-stop.png"));
        break;
    case SP_MediaPlay:
        break;
    case SP_MediaPause:
        break;
    case SP_MediaStop:
        break;
    case SP_MediaSeekForward:
        break;
    case SP_MediaSeekBackward:
        break;
    case SP_MediaSkipForward:
        break;
    case SP_MediaSkipBackward:
        break;
    case SP_MediaVolume:
        break;
    case SP_MediaVolumeMuted:
        break;
    case SP_DockWidgetCloseButton:
    case SP_TitleBarCloseButton:
        icon.addFile(QString(":/assets/icons/window_close_normal_%1.png").arg(themeType));
        icon.addFile(QString(":/assets/icons/window_close_hover_%1.png").arg(themeType), QSize(), QIcon::Active);
        icon.addFile(QString(":/assets/icons/window_close_press_%1.png").arg(themeType), QSize(), QIcon::Active, QIcon::On);
        break;
    case SP_TitleBarMaxButton:
        icon.addFile(QString(":/assets/icons/window_max_normal_%1.png").arg(themeType));
        icon.addFile(QString(":/assets/icons/window_max_hover_%1.png").arg(themeType), QSize(), QIcon::Active);
        icon.addFile(QString(":/assets/icons/window_max_press_%1.png").arg(themeType), QSize(), QIcon::Active, QIcon::On);
        break;
    case SP_TitleBarNormalButton:
        icon.addFile(QString(":/assets/icons/window_unmax_normal_%1.png").arg(themeType));
        icon.addFile(QString(":/assets/icons/window_unmax_hover_%1.png").arg(themeType), QSize(), QIcon::Active);
        icon.addFile(QString(":/assets/icons/window_unmax_press_%1.png").arg(themeType), QSize(), QIcon::Active, QIcon::On);
        break;
    case SP_TitleBarMinButton:
        icon.addFile(QString(":/assets/icons/window_min_normal_%1.png").arg(themeType));
        icon.addFile(QString(":/assets/icons/window_min_hover_%1.png").arg(themeType), QSize(), QIcon::Active);
        icon.addFile(QString(":/assets/icons/window_min_press_%1.png").arg(themeType), QSize(), QIcon::Active, QIcon::On);
        break;
    case SP_TitleBarMenuButton:
        icon.addFile(QString(":/assets/icons/window_option_normal_%1.png").arg(themeType));
        icon.addFile(QString(":/assets/icons/window_option_hover_%1.png").arg(themeType), QSize(), QIcon::Active);
        icon.addFile(QString(":/assets/icons/window_option_press_%1.png").arg(themeType), QSize(), QIcon::Active, QIcon::On);
        break;
    case SP_LineEditClearButton:
        icon.addFile(QLatin1String(":/assets/icons/input_clear_normal.svg"));
        icon.addFile(QLatin1String(":/assets/icons/input_clear_hover.svg"), QSize(), QIcon::Active);
        icon.addFile(QLatin1String(":/assets/icons/input_clear_press.svg"), QSize(), QIcon::Active, QIcon::On);
        break;
    default:
        break;
    }

    if (!icon.isNull())
        return icon;

    return QCommonStyle::standardIcon(standardIcon, opt, widget);
}

void Style::drawStandardIcon(QStyle::StandardPixmap sp, const QStyleOption *opt, QPainter *p, const QWidget *widget) const
{
    if (opt->rect.width() <= 1 || opt->rect.height() <= 1)
        return;
    QRect r = opt->rect;
    int size = qMin(r.height(), r.width());
    const bool enabled(opt->state & QStyle::State_Enabled);
    const bool mouseOver(opt->state & QStyle::State_MouseOver);
    const bool hasFocus((opt->state & QStyle::State_HasFocus ) && !( widget && widget->focusProxy()));
    const bool sunken((opt->state | QStyle::State_Sunken) == opt->state);
    QIcon::Mode mode = QIcon::Normal;

    if (!enabled)
        mode = QIcon::Disabled;
    else if (mouseOver)
        mode = QIcon::Active;
    else if (hasFocus)
        mode = QIcon::Selected;

    QPixmap pixmap = standardIcon(sp, opt, widget).pixmap(size, size, mode, sunken ? QIcon::On : QIcon::Off);

    int xOffset = r.x() + (r.width() - size)/2;
    int yOffset = r.y() + (r.height() - size)/2;
    p->drawPixmap(xOffset, yOffset, pixmap);
}

void Style::drawDeepinStyleIcon(const QString &name, const QStyleOption *opt, QPainter *p, const QWidget *widget) const
{
    if (opt->rect.width() <= 1 || opt->rect.height() <= 1)
        return;
    QRect r = opt->rect;
    int size = qMin(r.height(), r.width());
    const bool enabled(opt->state & QStyle::State_Enabled);
    const bool mouseOver(opt->state & QStyle::State_MouseOver);
    const bool hasFocus((opt->state & QStyle::State_HasFocus ) && !( widget && widget->focusProxy()));
    const bool selected(opt->state & QStyle::State_Selected);

    QString icon_state = "_normal";

    if (!enabled)
        icon_state = "_disabled";
    else if (mouseOver)
        icon_state = "_hover";
    else if (selected)
        icon_state = "_selected";
    else if (hasFocus)
        icon_state = "_active";

    QString style_name = "light";

    if (m_type == StyleDark)
        style_name = "dark";
    else if (m_type == StyleSemiDark)
        style_name = "semidark";
    else if (m_type == StyleSemiLight)
        style_name = "semilight";

    const QStringList formatList = QStringList() << "png" << "svg";
    QPixmap pixmap;

    foreach (const QString &format, formatList) {
        pixmap = QPixmap(":/assets/" + style_name + "/" + name + icon_state + "." + format);

        if (!pixmap.isNull())
            break;

        pixmap = QPixmap(":/assets/" + style_name + "/" + name + "_normal." + format);

        if (!pixmap.isNull())
            break;
    }

    if (pixmap.isNull())
        return;

    int xOffset = r.x() + (r.width() - size)/2;
    int yOffset = r.y() + (r.height() - size)/2;
    p->drawPixmap(xOffset, yOffset, pixmap);
}

QWindow *Style::qt_getWindow(const QWidget *widget)
{
    return widget ? widget->window()->windowHandle() : 0;
}

QColor Style::mergedColors(const QColor &colorA, const QColor &colorB, int factor)
{
    const int maxFactor = 100;
    QColor tmp = colorA;
    tmp.setRed((tmp.red() * factor) / maxFactor + (colorB.red() * (maxFactor - factor)) / maxFactor);
    tmp.setGreen((tmp.green() * factor) / maxFactor + (colorB.green() * (maxFactor - factor)) / maxFactor);
    tmp.setBlue((tmp.blue() * factor) / maxFactor + (colorB.blue() * (maxFactor - factor)) / maxFactor);
    return tmp;
}

QPixmap Style::colorizedImage(const QString &fileName, const QColor &color, int rotation)
{
    QString pixmapName = QLatin1String("$qt_ia-") % fileName % HexString<uint>(color.rgba()) % QString::number(rotation);
    QPixmap pixmap;
    if (!QPixmapCache::find(pixmapName, pixmap)) {
        QImage image(fileName);

        if (image.format() != QImage::Format_ARGB32_Premultiplied)
            image = image.convertToFormat( QImage::Format_ARGB32_Premultiplied);

        int width = image.width();
        int height = image.height();
        int source = color.rgba();

        unsigned char sourceRed = qRed(source);
        unsigned char sourceGreen = qGreen(source);
        unsigned char sourceBlue = qBlue(source);

        for (int y = 0; y < height; ++y)
        {
            QRgb *data = (QRgb*) image.scanLine(y);
            for (int x = 0 ; x < width ; x++) {
                QRgb col = data[x];
                unsigned int colorDiff = (qBlue(col) - qRed(col));
                unsigned char gray = qGreen(col);
                unsigned char red = gray + qt_div_255(sourceRed * colorDiff);
                unsigned char green = gray + qt_div_255(sourceGreen * colorDiff);
                unsigned char blue = gray + qt_div_255(sourceBlue * colorDiff);
                unsigned char alpha = qt_div_255(qAlpha(col) * qAlpha(source));
                data[x] = qRgba(std::min(alpha, red),
                                std::min(alpha, green),
                                std::min(alpha, blue),
                                alpha);
            }
        }
        if (rotation != 0) {
            QTransform transform;
            transform.translate(-image.width()/2, -image.height()/2);
            transform.rotate(rotation);
            transform.translate(image.width()/2, image.height()/2);
            image = image.transformed(transform);
        }

        pixmap = QPixmap::fromImage(image);
        QPixmapCache::insert(pixmapName, pixmap);
    }
    return pixmap;
}

void Style::fillBrush(QPainter *p, const QRect &rect, const QBrush &brush)
{
    if (brush.style() == Qt::TexturePattern) {
        const QPixmap &pixmap = brush.texture();
        QRect r = rect;

        r.setSize(pixmap.size());
        r.moveCenter(rect.center());

        p->drawPixmap(r, pixmap);
    } else {
        p->fillRect(rect, brush);
    }
}

#include "moc_style.cpp"

}
