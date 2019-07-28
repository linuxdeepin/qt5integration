/*
 * Copyright (C) 2017 ~ 2018 Deepin Technology Co., Ltd.
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

#include "commonhelper.h"
#include "paletteextended.h"
#include "style.h"
#include "painterhelper.h"
#include "geometryutils.h"
#include "dstyleanimation.h"

#include <QPainter>
#include <QStyleOption>
#include <QScrollBar>
#include <QDebug>

namespace dstyle {
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
                    ret.setRect(sliderstart, 2, sliderlen, scrollBarRect.height() - 4);
                else
                    ret.setRect(sliderstart, scrollBarRect.height() / 3, sliderlen, scrollBarRect.height() / 3);
            } else {
                if (mouseOver && enabled)
                    ret.setRect(2, sliderstart, scrollBarRect.width() - 4, sliderlen);
                else
                    ret.setRect(scrollBarRect.width() / 3, sliderstart, scrollBarRect.width() / 3, sliderlen);
            }
            break;
        default:
            break;
        }
        ret = visualRect(scrollbar->direction, scrollBarRect, ret);
    }

    return ret;
}

bool Style::drawScrollBarSliderControl(const QStyleOption *option, QPainter *painter, const QWidget *widget) const
{
    const QStyleOptionSlider *scrollBar = qstyleoption_cast<const QStyleOptionSlider *>(option);
    QObject *styleObject = option->styleObject;

    bool wasActive = false;

    if (styleObject && styleObject && proxy()->styleHint(SH_ScrollBar_Transient, option, widget)) {
        Q_D(const Style);

        qreal opacity = 0.0;
        bool shouldExpand = false;

        int oldPos = styleObject->property("_q_stylepos").toInt();
        int oldMin = styleObject->property("_q_stylemin").toInt();
        int oldMax = styleObject->property("_q_stylemax").toInt();
        QRect oldRect = styleObject->property("_q_stylerect").toRect();
        int oldState = styleObject->property("_q_stylestate").toInt();
        uint oldActiveControls = styleObject->property("_q_stylecontrols").toUInt();

        // a scrollbar is transient when the the scrollbar itself and
        // its sibling are both inactive (ie. not pressed/hovered/moved)
        bool transient = !scrollBar->activeSubControls && !(option->state & State_On);

        if (!transient ||
                oldPos != scrollBar->sliderPosition ||
                oldMin != scrollBar->minimum ||
                oldMax != scrollBar->maximum ||
                oldRect != scrollBar->rect ||
                oldState != scrollBar->state ||
                oldActiveControls != scrollBar->activeSubControls) {

            // if the scrollbar is transient or its attributes, geometry or
            // state has changed, the opacity is reset back to 100% opaque
            opacity = 1.0;

            styleObject->setProperty("_q_stylepos", scrollBar->sliderPosition);
            styleObject->setProperty("_q_stylemin", scrollBar->minimum);
            styleObject->setProperty("_q_stylemax", scrollBar->maximum);
            styleObject->setProperty("_q_stylerect", scrollBar->rect);
            styleObject->setProperty("_q_stylestate", static_cast<int>(scrollBar->state));
            styleObject->setProperty("_q_stylecontrols", static_cast<uint>(scrollBar->activeSubControls));

#ifndef QT_NO_ANIMATION
            DScrollbarStyleAnimation *anim  = qobject_cast<DScrollbarStyleAnimation *>(d->animation(styleObject));
            if (transient) {
                if (!anim) {
                    anim = new DScrollbarStyleAnimation(DScrollbarStyleAnimation::Deactivating, styleObject);
                    d->startAnimation(anim);
                } else if (anim->mode() == DScrollbarStyleAnimation::Deactivating) {
                    // the scrollbar was already fading out while the
                    // state changed -> restart the fade out animation
                    anim->setCurrentTime(0);
                }
            } else if (anim && anim->mode() == DScrollbarStyleAnimation::Deactivating) {
                d->stopAnimation(styleObject);
            }
#endif // !QT_NO_ANIMATION
        }

#ifndef QT_NO_ANIMATION
        DScrollbarStyleAnimation *anim = qobject_cast<DScrollbarStyleAnimation *>(d->animation(styleObject));

        if (anim && anim->mode() == DScrollbarStyleAnimation::Deactivating) {
            // once a scrollbar was active (hovered/pressed), it retains
            // the active look even if it's no longer active while fading out
            if (oldActiveControls)
                anim->setActive(true);

            wasActive = anim->wasActive();
            opacity = anim->currentValue();
        }

        shouldExpand = (scrollBar->activeSubControls || wasActive);
        if (shouldExpand) {
            if (!anim && !oldActiveControls) {
                // Start expand animation only once and when entering
                anim = new DScrollbarStyleAnimation(DScrollbarStyleAnimation::Activating, styleObject);
                d->startAnimation(anim);
            }
        }
        painter->setOpacity(opacity);
#endif // !QT_NO_ANIMATION
    }

    if (qFuzzyIsNull(painter->opacity()))
        return true;

    painter->setRenderHint(QPainter::Antialiasing);

    QRectF rect = option->rect;

    const QBrush &background = m_palette->brush(PaletteExtended::ScrollBar_HandleBrush, option, 0, Qt::lightGray);
    const QBrush &border = m_palette->brush(PaletteExtended::ScrollBar_HandleBorderBrush, option, 0, Qt::transparent);
    qreal radius = scrollBar->orientation == Qt::Horizontal ? rect.height() / 2.0 : rect.width() / 2.0;
    PainterHelper::drawRoundedRect(painter, rect, radius, radius, Qt::AbsoluteSize, background, Metrics::Painter_PenWidth, border);

    return true;
}
}// end namespace dstyle
