/*
 * SPDX-FileCopyrightText: 2017 - 2022 UnionTech Software Technology Co., Ltd.  
 * SPDX-License-Identifier: LGPL-3.0-or-later
 */
#include "style.h"
#include "paletteextended.h"

#include <QPainter>
#include <qdrawutil.h>
#include <QDebug>

namespace dstyle {
bool Style::drawIndicatorCheckBoxPrimitive(const QStyleOption *opt, QPainter *p, const QWidget *widget) const
{
    Q_UNUSED(widget)

    fillBrush(p, opt->rect, m_palette->brush(PaletteExtended::CheckBox_BackgroundBrush, opt));

    return true;
}
}
