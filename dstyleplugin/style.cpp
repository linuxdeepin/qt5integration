/**
 * Copyright (C) 2016 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#include <QMenu>
#include <QLineEdit>
#include <QPushButton>
#include <QComboBox>
#include <QScrollBar>
#include <QDebug>

#include "style.h"
#include "common.h"
#include "geometryutils.h"
#include "paletteextended.h"
#include "sliderhelper.h"
#include "pushbuttonhelper.h"
#include "framehelper.h"
#include "lineedithelper.h"
#include "scrollbarhelper.h"

namespace dstyle {

Style::Style(StyleType style) :
    QCommonStyle(),
    m_type(style),
    m_palette(new PaletteExtended(style, this))
{

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
            || qobject_cast<QScrollBar *>(w))
        w->setAttribute(Qt::WA_Hover, true);

    if (qobject_cast<QScrollBar *>(w)) {
        w->setAttribute(Qt::WA_OpaquePaintEvent, false);
    }
}

void Style::unpolish(QWidget *w)
{
    if (qobject_cast<QPushButton *>(w)
            || qobject_cast<QComboBox *>(w)
            || qobject_cast<QScrollBar *>(w))
        w->setAttribute(Qt::WA_Hover, false);

    if (qobject_cast<QScrollBar *>(w)) {
        w->setAttribute(Qt::WA_OpaquePaintEvent, true);
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

        // layout
    case PM_LayoutLeftMargin:
    case PM_LayoutTopMargin:
    case PM_LayoutRightMargin:
    case PM_LayoutBottomMargin:
    {
        /*
             * use either Child margin or TopLevel margin,
             * depending on  widget type
             */
        if( ( option && ( option->state & QStyle::State_Window ) ) || ( widget && widget->isWindow() ) )
        {

            return Metrics::Layout_TopLevelMarginWidth;

        } else if( widget && widget->inherits( "KPageView" ) ) {

            return 0;

        } else {

            return Metrics::Layout_ChildMarginWidth;

        }

    }

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

        // menubars
    case PM_MenuBarPanelWidth: return 0;
    case PM_MenuBarHMargin: return 0;
    case PM_MenuBarVMargin: return 0;
    case PM_MenuBarItemSpacing: return 0;
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

void Style::drawControl(QStyle::ControlElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget) const
{
    DrawControlFunc fcn( nullptr );

    switch( element ) {
    case CE_PushButtonBevel: fcn = &PushButtonHelper::drawPushButtonBevel; break;
    case CE_PushButtonLabel: fcn = &PushButtonHelper::drawPushButtonLabel; break;
        //        case CE_CheckBoxLabel: fcn = &Style::drawCheckBoxLabelControl; break;
        //        case CE_RadioButtonLabel: fcn = &Style::drawCheckBoxLabelControl; break;
        //        case CE_ToolButtonLabel: fcn = &Style::drawToolButtonLabelControl; break;
        //        case CE_ComboBoxLabel: fcn = &Style::drawComboBoxLabelControl; break;
        //        case CE_MenuBarEmptyArea: fcn = &Style::emptyControl; break;
        //        case CE_MenuBarItem: fcn = &Style::drawMenuBarItemControl; break;
        //        case CE_MenuItem: fcn = &Style::drawMenuItemControl; break;
        //        case CE_ToolBar: fcn = &Style::emptyControl; break;
        //        case CE_ProgressBar: fcn = &Style::drawProgressBarControl; break;
        //        case CE_ProgressBarContents: fcn = &Style::drawProgressBarContentsControl; break;
        //        case CE_ProgressBarGroove: fcn = &Style::drawProgressBarGrooveControl; break;
        //        case CE_ProgressBarLabel: fcn = &Style::drawProgressBarLabelControl; break;
    case CE_ScrollBarSlider: fcn = &ScrollBarHelper::drawScrollBarSliderControl; break;
        //        case CE_ScrollBarAddLine: fcn = &Style::drawScrollBarAddLineControl; break;
        //        case CE_ScrollBarSubLine: fcn = &Style::drawScrollBarSubLineControl; break;
        //        case CE_ScrollBarAddPage: fcn = &Style::emptyControl; break;
        //        case CE_ScrollBarSubPage: fcn = &Style::emptyControl; break;
        //        case CE_ShapedFrame: fcn = &Style::drawShapedFrameControl; break;
        //        case CE_RubberBand: fcn = &Style::drawRubberBandControl; break;
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
    default: break;
    }

    painter->save();

    // call function if implemented
    if( !( fcn && fcn( option, painter, widget ) ) )
    { QCommonStyle::drawControl( element, option, painter, widget ); }

    painter->restore();
}

void Style::drawComplexControl(QStyle::ComplexControl cc, const QStyleOptionComplex *opt, QPainter *p, const QWidget *w) const
{
    RenderFunc fn = nullptr;

    switch (cc) {
    case QStyle::CC_Slider: fn = &SliderHelper::render; break;
    default:
        break;
    }

    if (fn && fn(opt, p, w)) {
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
//    case PE_PanelMenu: fcn = &Style::drawPanelMenuPrimitive; break;
//    case PE_PanelTipLabel: fcn = &Style::drawPanelTipLabelPrimitive; break;
//    case PE_PanelItemViewItem: fcn = &Style::drawPanelItemViewItemPrimitive; break;
//    case PE_IndicatorCheckBox: fcn = &Style::drawIndicatorCheckBoxPrimitive; break;
//    case PE_IndicatorRadioButton: fcn = &Style::drawIndicatorRadioButtonPrimitive; break;
//    case PE_IndicatorButtonDropDown: fcn = &Style::drawIndicatorButtonDropDownPrimitive; break;
//    case PE_IndicatorTabClose: fcn = &Style::drawIndicatorTabClosePrimitive; break;
//    case PE_IndicatorTabTear: fcn = &Style::drawIndicatorTabTearPrimitive; break;
//    case PE_IndicatorArrowUp: fcn = &Style::drawIndicatorArrowUpPrimitive; break;
//    case PE_IndicatorArrowDown: fcn = &Style::drawIndicatorArrowDownPrimitive; break;
//    case PE_IndicatorArrowLeft: fcn = &Style::drawIndicatorArrowLeftPrimitive; break;
//    case PE_IndicatorArrowRight: fcn = &Style::drawIndicatorArrowRightPrimitive; break;
//    case PE_IndicatorHeaderArrow: fcn = &Style::drawIndicatorHeaderArrowPrimitive; break;
//    case PE_IndicatorToolBarHandle: fcn = &Style::drawIndicatorToolBarHandlePrimitive; break;
//    case PE_IndicatorToolBarSeparator: fcn = &Style::drawIndicatorToolBarSeparatorPrimitive; break;
//    case PE_IndicatorBranch: fcn = &Style::drawIndicatorBranchPrimitive; break;
//    case PE_FrameStatusBar: fcn = &Style::emptyPrimitive; break;
//    case PE_Frame: fcn = &Style::drawFramePrimitive; break;
//    case PE_FrameMenu: fcn = &Style::drawFrameMenuPrimitive; break;
    case PE_FrameLineEdit: fcn = &LineEditHelper::drawFrameLineEditPrimitive; break;
//    case PE_FrameGroupBox: fcn = &Style::drawFrameGroupBoxPrimitive; break;
//    case PE_FrameTabWidget: fcn = &Style::drawFrameTabWidgetPrimitive; break;
//    case PE_FrameTabBarBase: fcn = &Style::drawFrameTabBarBasePrimitive; break;
//    case PE_FrameWindow: fcn = &Style::drawFrameWindowPrimitive; break;
    case PE_FrameFocusRect: fcn = &FrameHelper::drawFrameFocusRectPrimitive; break;

    // fallback
    default: break;

    }

    painter->save();

    // call function if implemented
    if( !( fcn && fcn( option, painter, widget ) ) )
    { QCommonStyle::drawPrimitive( element, option, painter, widget ); }

    painter->restore();
}

int Style::styleHint(QStyle::StyleHint sh, const QStyleOption *opt, const QWidget *w, QStyleHintReturn *shret) const
{
    switch (sh) {
    case SH_ScrollBar_Transient:
        return true;
    default:
        break;
    }

    return QCommonStyle::styleHint(sh, opt, w, shret);
}

QRect Style::sliderSubControlRect(const QStyleOptionComplex *option, QStyle::SubControl subControl, const QWidget *widget) const
{
    // cast option and check
    const QStyleOptionSlider* sliderOption( qstyleoption_cast<const QStyleOptionSlider*>( option ) );
    if( !sliderOption ) return QCommonStyle::subControlRect( CC_Slider, option, subControl, widget );

    switch( subControl )
    {
    case SC_SliderGroove:
    {

        // direction
        const bool horizontal( sliderOption->orientation == Qt::Horizontal );

        // get base class rect
        QRect grooveRect( QCommonStyle::subControlRect( CC_Slider, option, subControl, widget ) );
        grooveRect = GeometryUtils::insideMargin( grooveRect, pixelMetric( PM_DefaultFrameWidth, option, widget ) );

        // centering
        if( horizontal ) {
            grooveRect = GeometryUtils::centerRect( grooveRect, grooveRect.width(), Metrics::Slider_GrooveThickness );
        } else {
            grooveRect = GeometryUtils::centerRect( grooveRect, Metrics::Slider_GrooveThickness, grooveRect.height() );
        }

        return grooveRect;

    }

    default: return QCommonStyle::subControlRect( CC_Slider, option, subControl, widget );
    }
}

QRect Style::scrollbarSubControlRect(const QStyleOptionComplex *opt, QStyle::SubControl sc, const QWidget *widget) const
{
    QRect ret(0, 0, 0, 0);

    if (const QStyleOptionSlider *scrollbar = qstyleoption_cast<const QStyleOptionSlider *>(opt)) {
        const bool enabled(opt->state & QStyle::State_Enabled);
        const bool mouseOver(opt->state & QStyle::State_MouseOver);
        const QRect scrollBarRect = scrollbar->rect;
        int sbextent = 0;
        if (!proxy()->styleHint(SH_ScrollBar_Transient, scrollbar, widget))
            sbextent = proxy()->pixelMetric(PM_ScrollBarExtent, scrollbar, widget);
        int maxlen = ((scrollbar->orientation == Qt::Horizontal) ?
                      scrollBarRect.width() : scrollBarRect.height());
        int sliderlen;

        // calculate slider length
        if (scrollbar->maximum != scrollbar->minimum) {
            uint range = scrollbar->maximum - scrollbar->minimum;
            sliderlen = (qint64(scrollbar->pageStep) * maxlen) / (range + scrollbar->pageStep);

            int slidermin = proxy()->pixelMetric(PM_ScrollBarSliderMin, scrollbar, widget);
            if (sliderlen < slidermin || range > INT_MAX / 2)
                sliderlen = slidermin;
            if (sliderlen > maxlen)
                sliderlen = maxlen;
        } else {
            sliderlen = maxlen;
        }

        int sliderstart = sliderPositionFromValue(scrollbar->minimum,
                                                  scrollbar->maximum,
                                                  scrollbar->sliderPosition,
                                                  maxlen - sliderlen,
                                                  scrollbar->upsideDown);

        switch (sc) {
        case SC_ScrollBarSubLine:            // top/left button
            if (scrollbar->orientation == Qt::Horizontal) {
                int buttonWidth = qMin(scrollBarRect.width() / 2, sbextent);
                ret.setRect(0, 0, buttonWidth, scrollBarRect.height());
            } else {
                int buttonHeight = qMin(scrollBarRect.height() / 2, sbextent);
                ret.setRect(0, 0, scrollBarRect.width(), buttonHeight);
            }
            break;
        case SC_ScrollBarAddLine:            // bottom/right button
            if (scrollbar->orientation == Qt::Horizontal) {
                int buttonWidth = qMin(scrollBarRect.width()/2, sbextent);
                ret.setRect(scrollBarRect.width() - buttonWidth, 0, buttonWidth, scrollBarRect.height());
            } else {
                int buttonHeight = qMin(scrollBarRect.height()/2, sbextent);
                ret.setRect(0, scrollBarRect.height() - buttonHeight, scrollBarRect.width(), buttonHeight);
            }
            break;
        case SC_ScrollBarSubPage:            // between top/left button and slider
            if (scrollbar->orientation == Qt::Horizontal)
                ret.setRect(sbextent, 0, sliderstart - sbextent, scrollBarRect.height());
            else
                ret.setRect(0, sbextent, scrollBarRect.width(), sliderstart - sbextent);
            break;
        case SC_ScrollBarAddPage:            // between bottom/right button and slider
            if (scrollbar->orientation == Qt::Horizontal)
                ret.setRect(sliderstart + sliderlen, 0,
                            maxlen - sliderstart - sliderlen + sbextent, scrollBarRect.height());
            else
                ret.setRect(0, sliderstart + sliderlen, scrollBarRect.width(),
                            maxlen - sliderstart - sliderlen + sbextent);
            break;
        case SC_ScrollBarGroove:
            if (scrollbar->orientation == Qt::Horizontal)
                ret.setRect(sbextent, 0, scrollBarRect.width() - sbextent * 2,
                            scrollBarRect.height());
            else
                ret.setRect(0, sbextent, scrollBarRect.width(),
                            scrollBarRect.height() - sbextent * 2);
            break;
        case SC_ScrollBarSlider:
            if (scrollbar->orientation == Qt::Horizontal) {
                if (mouseOver && enabled)
                    ret.setRect(sliderstart, 0, sliderlen, scrollBarRect.height());
                else
                    ret.setRect(sliderstart, scrollBarRect.height() / 4, sliderlen, scrollBarRect.height() / 2);
            } else {
                if (mouseOver && enabled)
                    ret.setRect(0, sliderstart, scrollBarRect.width(), sliderlen);
                else
                    ret.setRect(scrollBarRect.width() / 4, sliderstart, scrollBarRect.width() / 2, sliderlen);
            }
            break;
        default:
            break;
        }
        ret = visualRect(scrollbar->direction, scrollBarRect, ret);
    }

    return ret;
}

}
