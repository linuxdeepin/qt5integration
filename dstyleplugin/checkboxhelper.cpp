/**
 * Copyright (C) 2017 by Deepin Technology Co., Ltd.
 *
 * Use of this source code is governed by GPLv3
 * that can be found in the LICENSE file.
 **/

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
