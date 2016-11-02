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

#include "style.h"
#include "sliderhelper.h"
#include "common.h"
#include "geometryutils.h"

namespace dstyle {

Style::Style(StyleType style) :
    QCommonStyle(),
    m_type(style)
{

}

Style::~Style()
{

}

void Style::polish(QPalette &p)
{
    Q_UNUSED(p)
    QCommonStyle::polish(p);

    p.setCurrentColorGroup(QPalette::Active);
    p.setColor(QPalette::WindowText, Qt::white);
    p.setColor(QPalette::Highlight, QColor("#2ca7f8"));
}

void Style::polish(QWidget *w)
{
    Q_UNUSED(w)
    QCommonStyle::polish(w);
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

}
