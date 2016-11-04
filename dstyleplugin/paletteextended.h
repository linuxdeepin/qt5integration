/**
 * Copyright (C) 2016 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#ifndef PALETTEEXTENDED_H
#define PALETTEEXTENDED_H

#include <QObject>
#include <QSettings>

#include "common.h"

namespace dstyle {

class PaletteExtended : public QObject
{
    Q_OBJECT
public:
    static PaletteExtended *instance();

    enum ColorName {
        QPalette_Background,
        QPalette_WindowText,
        QPalette_Highlight,
        QPalette_HighlightText,

        Slider_GrooveColor,
        Slider_GrooveHighlightColor,
        Slider_HandleColor,
        Slider_TickmarkColor,

        PushButton_BackgroundDisabledColor,
        PushButton_BackgroundNormalColor,
        PushButton_BackgroundHoverColor,
        PushButton_BackgroundPressedColor,
        PushButton_TextDisabledColor,
        PushButton_TextNormalColor,
        PushButton_TextHoverColor,
        PushButton_TextPressedColor,
        PushButton_BorderDisabledColor,
        PushButton_BorderNormalColor,
        PushButton_BorderHoverColor,
        PushButton_BorderPressedColor,
    };
    Q_ENUM(ColorName)

    QColor color(ColorName name) const;

    void setType(StyleType type);


private:
    PaletteExtended();
    QColor parseColor(const QStringList &value) const;

    QSettings *m_colorScheme;
};

}

#endif // PALETTEEXTENDED_H
