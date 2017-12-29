/*
 * Copyright (C) 2017 ~ 2017 Deepin Technology Co., Ltd.
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

#include "style.h"
#include "paletteextended.h"
#include "painterhelper.h"

#include <QStyleOptionTab>
#include <QStyleOptionTabBarBase>
#include <QPainter>

namespace dstyle {
static void tabLayout(const QStyle *proxyStyle, const QStyleOptionTab *opt, const QWidget *widget, QRect *textRect, QRect *iconRect)
{
    Q_ASSERT(textRect);
    Q_ASSERT(iconRect);
    QRect tr = opt->rect;
    bool verticalTabs = opt->shape == QTabBar::RoundedEast
                        || opt->shape == QTabBar::RoundedWest
                        || opt->shape == QTabBar::TriangularEast
                        || opt->shape == QTabBar::TriangularWest;
    if (verticalTabs)
        tr.setRect(0, 0, tr.height(), tr.width()); //0, 0 as we will have a translate transform

    int verticalShift = proxyStyle->pixelMetric(QStyle::PM_TabBarTabShiftVertical, opt, widget);
    int horizontalShift = proxyStyle->pixelMetric(QStyle::PM_TabBarTabShiftHorizontal, opt, widget);
    int hpadding = proxyStyle->pixelMetric(QStyle::PM_TabBarTabHSpace, opt, widget) / 2;
    int vpadding = proxyStyle->pixelMetric(QStyle::PM_TabBarTabVSpace, opt, widget) / 2;
    if (opt->shape == QTabBar::RoundedSouth || opt->shape == QTabBar::TriangularSouth)
        verticalShift = -verticalShift;
    tr.adjust(hpadding, verticalShift - vpadding, horizontalShift - hpadding, vpadding);
    bool selected = opt->state & QStyle::State_Selected;
    if (selected) {
        tr.setTop(tr.top() - verticalShift);
        tr.setRight(tr.right() - horizontalShift);
    }

    // left widget
    if (!opt->leftButtonSize.isEmpty()) {
        tr.setLeft(tr.left() + 4 +
            (verticalTabs ? opt->leftButtonSize.height() : opt->leftButtonSize.width()));
    }
    // right widget
    if (!opt->rightButtonSize.isEmpty()) {
        tr.setRight(tr.right() - 4 -
        (verticalTabs ? opt->rightButtonSize.height() : opt->rightButtonSize.width()));
    }

    // icon
    if (!opt->icon.isNull()) {
        QSize iconSize = opt->iconSize;
        if (!iconSize.isValid()) {
            int iconExtent = proxyStyle->pixelMetric(QStyle::PM_SmallIconSize);
            iconSize = QSize(iconExtent, iconExtent);
        }
        QSize tabIconSize = opt->icon.actualSize(iconSize,
                        (opt->state & QStyle::State_Enabled) ? QIcon::Normal : QIcon::Disabled,
                        (opt->state & QStyle::State_Selected) ? QIcon::On : QIcon::Off  );
        // High-dpi icons do not need adjustmet; make sure tabIconSize is not larger than iconSize
        tabIconSize = QSize(qMin(tabIconSize.width(), iconSize.width()), qMin(tabIconSize.height(), iconSize.height()));

        *iconRect = QRect(tr.left(), tr.center().y() - tabIconSize.height() / 2,
                    tabIconSize.width(), tabIconSize .height());
        if (!verticalTabs)
            *iconRect = proxyStyle->visualRect(opt->direction, opt->rect, *iconRect);
        tr.setLeft(tr.left() + tabIconSize.width() + 4);
    }

    if (!verticalTabs)
        tr = proxyStyle->visualRect(opt->direction, opt->rect, tr);

    *textRect = tr;
}

bool Style::drawFrameTabBarBasePrimitive( const QStyleOption* option, QPainter* painter, const QWidget* widget ) const
{
    Q_UNUSED(widget)

    auto opt = static_cast<const QStyleOptionTabBarBase*>(option);

    PainterHelper::drawRect(painter, opt->tabBarRect,
                            m_palette->brush(PaletteExtended::TabBar_BackgroundBrush, option),
                            1,
                            m_palette->brush(PaletteExtended::TabBar_BorderBrush, option));

    return true;
}

bool Style::drawTabBarTabLabelControl(const QStyleOption *opt, QPainter *p, const QWidget *widget) const
{
    auto tab = static_cast<const QStyleOptionTab*>(opt);

    QRect tr = tab->rect;
    bool verticalTabs = tab->shape == QTabBar::RoundedEast
                        || tab->shape == QTabBar::RoundedWest
                        || tab->shape == QTabBar::TriangularEast
                        || tab->shape == QTabBar::TriangularWest;

    int alignment = Qt::AlignCenter | Qt::TextShowMnemonic;
    if (!proxy()->styleHint(SH_UnderlineShortcut, opt, widget))
        alignment |= Qt::TextHideMnemonic;

    if (verticalTabs) {
        p->save();
        int newX, newY, newRot;
        if (tab->shape == QTabBar::RoundedEast || tab->shape == QTabBar::TriangularEast) {
            newX = tr.width() + tr.x();
            newY = tr.y();
            newRot = 90;
        } else {
            newX = tr.x();
            newY = tr.y() + tr.height();
            newRot = -90;
        }
        QTransform m = QTransform::fromTranslate(newX, newY);
        m.rotate(newRot);
        p->setTransform(m, true);
    }
    QRect iconRect;
    tabLayout(proxy(), tab, widget, &tr, &iconRect);
    tr = proxy()->subElementRect(SE_TabBarTabText, opt, widget); //we compute tr twice because the style may override subElementRect

    if (!tab->icon.isNull()) {
        QPixmap tabIcon = tab->icon.pixmap(qt_getWindow(widget), tab->iconSize,
                                           (tab->state & State_Enabled) ? QIcon::Normal
                                                                        : QIcon::Disabled,
                                           (tab->state & State_Selected) ? QIcon::On
                                                                         : QIcon::Off);
        p->drawPixmap(iconRect.x(), iconRect.y(), tabIcon);
    }

    p->setPen(QPen(m_palette->brush(PaletteExtended::TabBarTab_TextColor, opt), 1));
    p->drawText(tr, alignment, tab->text);

    if (verticalTabs)
        p->restore();

    if (tab->state & State_HasFocus) {
        const int OFFSET = 1 + pixelMetric(PM_DefaultFrameWidth);

        int x1, x2;
        x1 = tab->rect.left();
        x2 = tab->rect.right() - 1;

        QStyleOptionFocusRect fropt;
        fropt.QStyleOption::operator=(*tab);
        fropt.rect.setRect(x1 + 1 + OFFSET, tab->rect.y() + OFFSET,
                           x2 - x1 - 2*OFFSET, tab->rect.height() - 2*OFFSET);
        drawPrimitive(PE_FrameFocusRect, &fropt, p, widget);
    }

    return true;
}

bool Style::drawTabBarTabShapeControl(const QStyleOption *opt, QPainter *p, const QWidget *widget) const
{
    auto tab = static_cast<const QStyleOptionTab*>(opt);

    p->save();

    QRect rect(tab->rect);
    bool selected = tab->state & State_Selected;
    bool onlyOne = tab->position == QStyleOptionTab::OnlyOneTab;
    int tabOverlap = onlyOne ? 0 : proxy()->pixelMetric(PM_TabBarTabOverlap, opt, widget);

    if (!selected) {
        switch (tab->shape) {
        case QTabBar::TriangularNorth:
            rect.adjust(0, 0, 0, -tabOverlap);
            if(!selected)
                rect.adjust(1, 1, -1, 0);
            break;
        case QTabBar::TriangularSouth:
            rect.adjust(0, tabOverlap, 0, 0);
            if(!selected)
                rect.adjust(1, 0, -1, -1);
            break;
        case QTabBar::TriangularEast:
            rect.adjust(tabOverlap, 0, 0, 0);
            if(!selected)
                rect.adjust(0, 1, -1, -1);
            break;
        case QTabBar::TriangularWest:
            rect.adjust(0, 0, -tabOverlap, 0);
            if(!selected)
                rect.adjust(1, 1, 0, -1);
            break;
        default:
            break;
        }
    }

    p->setPen(QPen(m_palette->brush(PaletteExtended::TabBarTab_BorderBrush, opt), 0));
    p->setBrush(m_palette->brush(PaletteExtended::TabBarTab_BackgroundBrush, opt));
//    if (selected) {
//        p->setBrush(tab->palette.base());
//    } else {
//        if (widget && widget->parentWidget())
//            p->setBrush(widget->parentWidget()->palette().background());
//        else
//            p->setBrush(tab->palette.background());
//    }

    int y;
    int x;
    QPolygon a(10);
    switch (tab->shape) {
    case QTabBar::TriangularNorth:
    case QTabBar::TriangularSouth: {
        a.setPoint(0, 0, -1);
        a.setPoint(1, 0, 0);
        y = rect.height() - 2;
        x = y / 3;
        a.setPoint(2, x++, y - 1);
        ++x;
        a.setPoint(3, x++, y++);
        a.setPoint(4, x, y);

        int i;
        int right = rect.width() - 1;
        for (i = 0; i < 5; ++i)
            a.setPoint(9 - i, right - a.point(i).x(), a.point(i).y());
        if (tab->shape == QTabBar::TriangularNorth)
            for (i = 0; i < 10; ++i)
                a.setPoint(i, a.point(i).x(), rect.height() - 1 - a.point(i).y());

        a.translate(rect.left(), rect.top());
        p->setRenderHint(QPainter::Antialiasing);
        p->translate(0, 0.5);

        QPainterPath path;
        path.addPolygon(a);
        p->drawPath(path);
        break; }
    case QTabBar::TriangularEast:
    case QTabBar::TriangularWest: {
        a.setPoint(0, -1, 0);
        a.setPoint(1, 0, 0);
        x = rect.width() - 2;
        y = x / 3;
        a.setPoint(2, x - 1, y++);
        ++y;
        a.setPoint(3, x++, y++);
        a.setPoint(4, x, y);
        int i;
        int bottom = rect.height() - 1;
        for (i = 0; i < 5; ++i)
            a.setPoint(9 - i, a.point(i).x(), bottom - a.point(i).y());
        if (tab->shape == QTabBar::TriangularWest)
            for (i = 0; i < 10; ++i)
                a.setPoint(i, rect.width() - 1 - a.point(i).x(), a.point(i).y());
        a.translate(rect.left(), rect.top());
        p->setRenderHint(QPainter::Antialiasing);
        p->translate(0.5, 0);
        QPainterPath path;
        path.addPolygon(a);
        p->drawPath(path);
        break; }
    case QTabBar::RoundedNorth:
    case QTabBar::RoundedSouth: {
        const QRect &border_rect = tab->rect.adjusted(0, 0, 1, 1);
        p->fillRect(tab->rect, p->brush());

        if (selected) {
            p->drawLine(border_rect.topLeft(), border_rect.bottomLeft());
            p->drawLine(border_rect.topRight(), border_rect.bottomRight());

            if (tab->shape == QTabBar::RoundedSouth) {
                p->drawLine(border_rect.bottomLeft(), border_rect.bottomRight());
                p->drawLine(border_rect.topLeft(), border_rect.topLeft());
            } else {
                p->drawLine(border_rect.topLeft(), border_rect.topRight());
                p->drawLine(border_rect.bottomLeft(), border_rect.bottomLeft());
            }
        } else {
            if (tab->selectedPosition != QStyleOptionTab::NextIsSelected) {
                p->drawLine(border_rect.topRight() + QPoint(0, 4), border_rect.bottomRight() - QPoint(0, 4));
            }

            if (tab->shape == QTabBar::RoundedSouth) {
                p->drawLine(border_rect.topLeft(), border_rect.topRight());
            } else {
                p->drawLine(border_rect.bottomLeft(), border_rect.bottomRight());
            }
        }

        break;
    }
    case QTabBar::RoundedEast:
    case QTabBar::RoundedWest: {
        const QRect &border_rect = tab->rect.adjusted(0, 0, 1, 1);
        p->fillRect(tab->rect, p->brush());

        if (selected) {
            p->drawLine(border_rect.bottomLeft(), border_rect.bottomRight());
            p->drawLine(border_rect.topLeft(), border_rect.topRight());

            if (tab->shape == QTabBar::RoundedEast) {
                p->drawLine(border_rect.bottomRight(), border_rect.topRight());
                p->drawLine(border_rect.bottomLeft(), border_rect.bottomLeft());
            } else {
                p->drawLine(border_rect.bottomLeft(), border_rect.topLeft());
                p->drawLine(border_rect.bottomRight(), border_rect.bottomRight());
            }
        } else {
            if (tab->selectedPosition != QStyleOptionTab::NextIsSelected) {
                p->drawLine(border_rect.topLeft() + QPoint(4, 0), border_rect.topRight() - QPoint(4, 0));
            }

            if (tab->shape == QTabBar::RoundedEast) {
                p->drawLine(border_rect.bottomLeft(), border_rect.topLeft());
            } else {
                p->drawLine(border_rect.bottomRight(), border_rect.topRight());
            }
        }

        break;
    }
    }
    p->restore();

    return true;
}
}
