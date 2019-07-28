/*
 * Copyright (C) 2016 ~ 2018 Deepin Technology Co., Ltd.
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

#ifndef PALETTEEXTENDED_H
#define PALETTEEXTENDED_H

#include <QObject>
#include <QSettings>
#include <QPalette>
#include <QHash>

#include "common.h"

QT_BEGIN_NAMESPACE
class QStyleOption;
QT_END_NAMESPACE

namespace QCss {
struct StyleSheet;
}

namespace dstyle {

class PaletteExtended : public QObject
{
    Q_OBJECT

public:
    static PaletteExtended *instance(StyleType type);

    ~PaletteExtended();

    enum BrushName {
        //TODO(hualet): Color Group support.
        QPalette_Base,
        QPalette_Text,
        QPalette_Window,
        QPalette_WindowText,
        QPalette_Highlight,
        QPalette_HighlightedText,
        QPalette_Button,
        QPalette_ButtonText,

        ScrollBar_HandleBrush,
        ScrollBar_HandleBorderBrush,

        Slider_GrooveColor,
        Slider_GrooveHighlightColor,
        Slider_HandleBrush,
        Slider_VernierHandleBrush,
        Slider_HandleBorderColor,
        Slider_TickmarkColor,

        ProgressBar_GrooveBackground,
        ProgressBar_GrooveBorder,
        ProgressBar_ContentBackground,
        ProgressBar_ContentBorder,

        PushButton_BackgroundBrush,
        PushButton_TextColor,
        PushButton_BorderBrush,
        PushButton_SuggestButtonColor,
        PushButton_SuggestTextColor,

        LineEdit_BorderBrush,
        LineEdit_AlertBorderBrush,
        LineEdit_BackgroundBrush,

        Menu_SeparatorColor,
        Menu_BorderColor,
        Menu_BackgroundBrush,

        CheckBox_BackgroundBrush,
        RadioButton_BackgroundBrush,

        RubberBand_BackgroundBrush,
        RubberBand_BorderBrush,

        HeaderView_ArrowIndicatorUpIcon,
        HeaderView_ArrowIndicatorDownIcon,

        SpinBox_MinusBrush,
        SpinBox_PlusBrush,
        SpinBox_DownBrush,
        SpinBox_UpBrush,
        SpinBox_BorderColor,

        TabBar_BackgroundBrush,
        TabBar_BorderBrush,
        TabBarTab_BackgroundBrush,
        TabBarTab_BorderBrush,
        TabBarTab_TextColor,
        TabBarTab_ActiveColor,
        TabBarTab_CloseIcon,
        TabBarTab_CloseIconBackground,
        TabBarScrollButton_BackgroundBrush,
        TabBarScrollButton_UpIcon,
        TabBarScrollButton_DownIcon,
        TabBarAddButton_BackgroundBrush,
        TabBarAddButton_Icon
    };
#if QT_VERSION < QT_VERSION_CHECK(5, 5, 0)
    Q_ENUMS(BrushName)
#else
    Q_ENUM(BrushName)
#endif

    enum PseudoClassType {
        PseudoClass_Unknown          = Q_UINT64_C(0x0000000000000000),
        PseudoClass_Enabled          = Q_UINT64_C(0x0000000000000001),
        PseudoClass_Disabled         = Q_UINT64_C(0x0000000000000002),
        PseudoClass_Pressed          = Q_UINT64_C(0x0000000000000004),
        PseudoClass_Focus            = Q_UINT64_C(0x0000000000000008),
        PseudoClass_Hover            = Q_UINT64_C(0x0000000000000010),
        PseudoClass_Checked          = Q_UINT64_C(0x0000000000000020),
        PseudoClass_Unchecked        = Q_UINT64_C(0x0000000000000040),
        PseudoClass_Indeterminate    = Q_UINT64_C(0x0000000000000080),
        PseudoClass_Unspecified      = Q_UINT64_C(0x0000000000000100),
        PseudoClass_Selected         = Q_UINT64_C(0x0000000000000200),
        PseudoClass_Horizontal       = Q_UINT64_C(0x0000000000000400),
        PseudoClass_Vertical         = Q_UINT64_C(0x0000000000000800),
        PseudoClass_Window           = Q_UINT64_C(0x0000000000001000),
        PseudoClass_Children         = Q_UINT64_C(0x0000000000002000),
        PseudoClass_Sibling          = Q_UINT64_C(0x0000000000004000),
        PseudoClass_Default          = Q_UINT64_C(0x0000000000008000),
        PseudoClass_First            = Q_UINT64_C(0x0000000000010000),
        PseudoClass_Last             = Q_UINT64_C(0x0000000000020000),
        PseudoClass_Middle           = Q_UINT64_C(0x0000000000040000),
        PseudoClass_OnlyOne          = Q_UINT64_C(0x0000000000080000),
        PseudoClass_PreviousSelected = Q_UINT64_C(0x0000000000100000),
        PseudoClass_NextSelected     = Q_UINT64_C(0x0000000000200000),
        PseudoClass_Flat             = Q_UINT64_C(0x0000000000400000),
        PseudoClass_Left             = Q_UINT64_C(0x0000000000800000),
        PseudoClass_Right            = Q_UINT64_C(0x0000000001000000),
        PseudoClass_Top              = Q_UINT64_C(0x0000000002000000),
        PseudoClass_Bottom           = Q_UINT64_C(0x0000000004000000),
        PseudoClass_Exclusive        = Q_UINT64_C(0x0000000008000000),
        PseudoClass_NonExclusive     = Q_UINT64_C(0x0000000010000000),
        PseudoClass_Frameless        = Q_UINT64_C(0x0000000020000000),
        PseudoClass_ReadOnly         = Q_UINT64_C(0x0000000040000000),
        PseudoClass_Active           = Q_UINT64_C(0x0000000080000000),
        PseudoClass_Closable         = Q_UINT64_C(0x0000000100000000),
        PseudoClass_Movable          = Q_UINT64_C(0x0000000200000000),
        PseudoClass_Floatable        = Q_UINT64_C(0x0000000400000000),
        PseudoClass_Minimized        = Q_UINT64_C(0x0000000800000000),
        PseudoClass_Maximized        = Q_UINT64_C(0x0000001000000000),
        PseudoClass_On               = Q_UINT64_C(0x0000002000000000),
        PseudoClass_Off              = Q_UINT64_C(0x0000004000000000),
        PseudoClass_Editable         = Q_UINT64_C(0x0000008000000000),
        PseudoClass_Item             = Q_UINT64_C(0x0000010000000000),
        PseudoClass_Closed           = Q_UINT64_C(0x0000020000000000),
        PseudoClass_Open             = Q_UINT64_C(0x0000040000000000),
        PseudoClass_EditFocus        = Q_UINT64_C(0x0000080000000000),
        PseudoClass_Alternate        = Q_UINT64_C(0x0000100000000000),
        // The Any specifier is never generated, but can be used as a wildcard in searches.
        PseudoClass_Any              = Q_UINT64_C(0x0000ffffffffffff)
    };

    QBrush brush(const QWidget *widget, BrushName name, quint64 type = PseudoClass_Unspecified, const QBrush &defaultBrush = Qt::NoBrush) const;
    QBrush brush(const QWidget *widget, BrushName name, const QStyleOption *option, quint64 extraTypes = PseudoClass_Unknown, const QBrush &defaultBrush = Qt::NoBrush) const;
    QBrush brush(BrushName name, quint64 type = PseudoClass_Unspecified, const QBrush &defaultBrush = Qt::NoBrush) const;
    QBrush brush(BrushName name, const QStyleOption *option, quint64 extraTypes = PseudoClass_Unknown, const QBrush &defaultBrush = Qt::NoBrush) const;

    void polish(QPalette &p);

private:
    void init(StyleType type);
    PaletteExtended(StyleType type, QObject *parent = 0);

    StyleType m_type;
    QCss::StyleSheet *m_brushScheme;
    mutable QHash<QPair<BrushName, quint64>, QBrush> m_brushCache;
    static QList<PaletteExtended*> styleTypeToPaletteList;
};
}

#endif // PALETTEEXTENDED_H
