/**
 * Copyright (C) 2016 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#ifndef COMMON_H
#define COMMON_H

namespace dstyle {

enum StyleType { StyleDark, StyleLight };

enum Metrics
{
    // painter
    Painter_PenWidth = 1,

    // frames
    Frame_FrameWidth = 2,
    Frame_FrameRadius = 4,

    // layout
    Layout_TopLevelMarginWidth = 10,
    Layout_ChildMarginWidth = 6,
    Layout_DefaultSpacing = 6,

    // line editors
    LineEdit_FrameWidth = 4,

    // combobox
    ComboBox_FrameWidth = 6,

    // spinbox
    SpinBox_FrameWidth = LineEdit_FrameWidth,
    SpinBox_ArrowButtonWidth = 20,

    // groupbox title margin
    GroupBox_TitleMarginWidth = 4,

    // buttons
    Button_MinWidth = 80,
    Button_MarginWidth = 6,
    Button_ItemSpacing = 4,

    // tool buttons
    ToolButton_MarginWidth = 6,
    ToolButton_ItemSpacing = 4,
    ToolButton_InlineIndicatorWidth = 12,

    // checkboxes and radio buttons
    CheckBox_Size = 20,
    CheckBox_FocusMarginWidth = 2,
    CheckBox_ItemSpacing = 4,

    // menu
    Menu_ItemHMargin = 0,
    Menu_ItemVMargin = -1,
    Menu_ArrowHMargin = 6,
    Menu_CheckMarkWidth = 12,
    Menu_RightBorder = 10,
    Menu_ItemFrameWidth = 2,
    Menu_ItemHTextMargin = 3,
    Menu_ItemVTextMargin = 8,
    Menu_FrameWidth = 0,
    Menu_HMargin = 0,
    Menu_VMargin = 4,
    Menu_SeparatorItemHMargin = 5,
    Menu_PanelRightPadding = 10,
    MenuItem_MarginWidth = 3,
    MenuItem_ItemSpacing = 4,
    MenuItem_AcceleratorSpace = 16,
    MenuButton_IndicatorWidth = 20,

    // menu bar
    MenuBar_ItemSpacing = 6,
    MenuBarItem_MarginWidth = 10,
    MenuBarItem_MarginHeight = 6,

    // scrollbars
    ScrollBar_Extend = 8,
    ScrollBar_SliderWidth = 15,
    ScrollBar_MinSliderHeight = 40,
    ScrollBar_NoButtonHeight = (ScrollBar_Extend-ScrollBar_SliderWidth)/2,
    ScrollBar_SingleButtonHeight = ScrollBar_Extend,
    ScrollBar_DoubleButtonHeight = 2*ScrollBar_Extend,

    // toolbars
    ToolBar_FrameWidth = 2,
    ToolBar_HandleExtent = 10,
    ToolBar_HandleWidth = 6,
    ToolBar_SeparatorWidth = 8,
    ToolBar_ExtensionWidth = 20,
    ToolBar_ItemSpacing = 0,

    // progressbars
    ProgressBar_BusyIndicatorSize = 14,
    ProgressBar_ItemSpacing = 4,

    // mdi title bar
    TitleBar_MarginWidth = 4,

    // sliders
    Slider_TickLength = 8,
    Slider_TickMarginWidth = 2,
    Slider_GrooveThickness = 2,
    Slider_ControlThickness = 18,

    // tabbar
    TabBar_TabMarginHeight = 4,
    TabBar_TabMarginWidth = 8,
    TabBar_TabMinWidth = 80,
    TabBar_TabMinHeight = 28,
    TabBar_TabItemSpacing = 8,
    TabBar_TabOverlap = 1,
    TabBar_BaseOverlap = 2,

    // tab widget
    TabWidget_MarginWidth = 4,

    // toolbox
    ToolBox_TabMinWidth = 80,
    ToolBox_TabItemSpacing = 4,
    ToolBox_TabMarginWidth = 8,

    // tooltips
    ToolTip_FrameWidth = 3,

    // list headers
    Header_MarginWidth = 6,
    Header_ItemSpacing = 4,
    Header_ArrowSize = 12,

    // tree view
    ItemView_ArrowSize = 10,
    ItemView_ItemMarginWidth = 3,
    SidePanel_ItemMarginWidth = 4,

    // splitter
    Splitter_SplitterWidth = 1,

    // shadow dimensions
    Shadow_Overlap = 2,

    // group box
    GroupBox_TopMargin = 3
};

}

#endif // COMMON_H
