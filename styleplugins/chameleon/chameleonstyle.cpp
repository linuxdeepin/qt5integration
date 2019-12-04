/*
 * Copyright (C) 2017 ~ 2019 Deepin Technology Co., Ltd.
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
#include "chameleonstyle.h"
#include "common.h"

#include <DNativeSettings>
#include <DStyleOption>
#include <DApplication>
#include <DPlatformWindowHandle>
#include <DApplicationHelper>
#include <DWindowManagerHelper>
#include <DSlider>
#include <DTabBar>

#include <QLabel>
#include <QCalendarWidget>
#include <QLayout>
#include <QVariant>
#include <QDebug>
#include <QApplication>
#include <QPushButton>
#include <QComboBox>
#include <QScrollBar>
#include <QCheckBox>
#include <QRadioButton>
#include <QToolButton>
#include <QLineEdit>
#include <QAction>
#include <QMenu>
#include <QPainter>
#include <QPaintEngine>
#include <QAbstractItemView>
#include <QBitmap>
#include <QSpinBox>
#include <QTableView>
#include <private/qcombobox_p.h>

#include <qdrawutil.h>
#include <qpa/qplatformwindow.h>

#include "../dstyleplugin/dstyleanimation.h"

DGUI_USE_NAMESPACE
DWIDGET_USE_NAMESPACE

namespace chameleon {

ChameleonStyle::ChameleonStyle()
    : DStyle()
{

}

void ChameleonStyle::drawPrimitive(QStyle::PrimitiveElement pe, const QStyleOption *opt,
                                   QPainter *p, const QWidget *w) const
{
    switch (static_cast<int>(pe)) {
    case PE_PanelButtonCommand: {
//        qDebug() << "### pushbutton state " << (int)opt->state;
        const QMargins &margins = frameExtentMargins();

        // checked
        if (opt->state & State_On) {
            p->setBrush(getColor(opt, QPalette::Highlight));
        } else {
            drawShadow(p, opt->rect - margins, getColor(opt, QPalette::Shadow));
            // 初始化button的渐变背景色
            QLinearGradient lg(QPointF(0, opt->rect.top()),
                               QPointF(0, opt->rect.bottom()));
            lg.setColorAt(0, getColor(opt, QPalette::Light));
            lg.setColorAt(1, getColor(opt, QPalette::Dark));

            p->setBrush(lg);
        }

        p->setPen(QPen(getColor(opt, DPalette::FrameBorder, w), Metrics::Painter_PenWidth));
        p->setRenderHint(QPainter::Antialiasing);

        int frame_radius = DStyle::pixelMetric(PM_FrameRadius, opt, w);

        p->drawRoundedRect(opt->rect - margins, frame_radius, frame_radius);

        return;
    }
    case PE_FrameFocusRect: {
        if (w && w->property("_d_dtk_noFocusRect").toBool())
            return;

        drawBorder(p, opt, w);
        return;
    }
    case PE_PanelItemViewItem: {
        if (const QStyleOptionViewItem *vopt = qstyleoption_cast<const QStyleOptionViewItem *>(opt)) {
            int frame_radius = DStyle::pixelMetric(PM_FrameRadius, opt, w);

            if (vopt->state & QStyle::State_Selected) {
                QRect select_rect = opt->rect;

                if (!vopt->showDecorationSelected) {
                    select_rect = proxy()->subElementRect(QStyle::SE_ItemViewItemText,  opt, w);
                } else {
                    select_rect -= frameExtentMargins();
                }

                p->setPen(Qt::NoPen);
                p->setBrush(getColor(opt, QPalette::Highlight));
                p->setRenderHint(QPainter::Antialiasing);
                p->drawRoundedRect(select_rect, frame_radius, frame_radius);
            }

            return;
        }
        break;
    }
    case PE_PanelLineEdit: {
        if (auto fopt = qstyleoption_cast<const QStyleOptionFrame*>(opt)) {
            // Flat时不绘制输入框的背景
            if (fopt->features == QStyleOptionFrame::Flat) {
                if (opt->state.testFlag(QStyle::State_HasFocus)) {
                    proxy()->drawPrimitive(PE_FrameFocusRect, opt, p, w);
                }
                return;
            }

            if (fopt->lineWidth > 0) {
                proxy()->drawPrimitive(PE_FrameLineEdit, fopt, p, w);
            }
        }

        p->setBrush(opt->palette.button());
        p->setPen(Qt::NoPen);
        p->setRenderHints(QPainter::Antialiasing);
        int frame_radius = DStyle::pixelMetric(PM_FrameRadius, opt, w);
        p->drawRoundedRect(opt->rect - frameExtentMargins(), frame_radius, frame_radius);

        if (w && w->parent() && (
                    qobject_cast<const QComboBox *>(w->parent())
                )) {
            //禁用一些控件绘制子lineEdit时产生的ForceRect
            return;
        }

        if (opt->state.testFlag(QStyle::State_HasFocus)) {
            proxy()->drawPrimitive(PE_FrameFocusRect, opt, p, w);
        }

        return;
    }
    case PE_FrameLineEdit: {
        // lineedit no frame
        return;
    }
    case PE_IndicatorRadioButton: {
        QRect standard = opt->rect;

        p->setRenderHint(QPainter::Antialiasing, true);

        if (opt->state & State_On) {  //Qt::Checked
            double padding = standard.width() / 2.0 / 2.0;
            QPainterPath path;

            path.addEllipse(standard);
            path.addEllipse(standard.adjusted(padding, padding, -padding, -padding));

            p->fillPath(path, getColor(opt, DPalette::Highlight));
        } else if (opt->state & State_Off) {
            p->setPen(QPen(getColor(opt, DPalette::WindowText), 1));
            p->drawEllipse(standard.adjusted(1, 1, -1, -1));
        }

        return;
    }
    case PE_IndicatorCheckBox: {
        QRectF standard = opt->rect;

        if (opt->state & State_NoChange) {  //Qt::PartiallyChecked
            DDrawUtils::drawBorder(p, standard, getColor(opt, DPalette::WindowText), 1, 2);
            QRectF lineRect(0, 0, standard.width() / 2.0, 2);
            lineRect.moveCenter(standard.center());
            p->fillRect(lineRect, getColor(opt, DPalette::TextTitle, w));
        } else if (opt->state & State_On) {  //Qt::Checked
            p->setPen(getColor(opt, DPalette::Highlight));
            p->setBrush(Qt::NoBrush);

            QIcon icon = QIcon::fromTheme("checked");
            icon.paint(p, standard.toRect());
        } else {
            DDrawUtils::drawBorder(p, standard, getColor(opt, DPalette::WindowText), 1, 2);
        }

        return;
    }
    case PE_IndicatorTabClose: {
        QIcon icon = DStyle::standardIcon(SP_CloseButton, opt, w);
        icon.paint(p, opt->rect);
        return;
    }
    case PE_FrameTabWidget: {
        p->setPen(QPen(getColor(opt, QPalette::Dark), proxy()->pixelMetric(PM_DefaultFrameWidth, opt, w)));
        p->setBrush(getColor(opt, QPalette::Window));
        p->drawRect(opt->rect);
        return;
    }
    case PE_IndicatorItemViewItemCheck: {
        QRectF standard = opt->rect;
        p->setRenderHint(QPainter::Antialiasing, true);

        if (opt->state & State_NoChange) {  //Qt::PartiallyChecked
            DStyle::standardIcon(SP_IndicatorChecked, opt, w).paint(p, standard.toRect());
        } else if (opt->state & State_On) {  //Qt::Checked
            p->setPen(getColor(opt, DPalette::Highlight));
            DStyle::standardIcon(SP_IndicatorChecked, opt, w).paint(p, standard.toRect());
        }
        return;
    }
    case PE_PanelMenu: {
        if (opt->palette.window().color().isValid()
                && DWindowManagerHelper::instance()->hasBlurWindow()) {
            QColor color = opt->palette.window().color();
            color.setAlphaF(0.3);
            p->fillRect(opt->rect, color);
        } else {
            p->fillRect(opt->rect, opt->palette.window());
        }

        break;
    }
    case PE_Frame: {
        if (const QStyleOptionFrame *f = qstyleoption_cast<const QStyleOptionFrame *>(opt)) {
            if (f->features & QStyleOptionFrame::Rounded) {
                p->setRenderHint(QPainter::Antialiasing);
                p->setBrush(p->background());

                if (f->lineWidth <= 0) {
                    p->setPen(Qt::NoPen);
                }

                int radius = DStyle::pixelMetric(PM_FrameRadius, opt, w);
                QRectF rect(opt->rect);
                rect.adjust(f->lineWidth / 2.0, f->lineWidth / 2.0, -f->lineWidth / 2.0, -f->lineWidth / 2.0);
                p->drawRoundedRect(rect, radius, radius);
                return;
            }
        }
        break;
    }
    case PE_PanelTipLabel: {
        return;
    }
    case PE_FrameGroupBox: {
        DStyleOptionBackgroundGroup option;
        option.init(w);
        option.position = DStyleOptionBackgroundGroup::OnlyOne;
        static_cast<QStyleOption*>(&option)->operator =(*opt);
        DStyle::drawPrimitive(PE_ItemBackground, &option, p, w);
        return;
    }
    case PE_IndicatorArrowUp: {
        QIcon icon = DStyle::standardIcon(SP_ArrowUp, opt, w);
        icon.paint(p, opt->rect);
        return;
    }
    case PE_IndicatorArrowDown: {
        QIcon icon = DStyle::standardIcon(SP_ArrowDown, opt, w);
        icon.paint(p, opt->rect);
        return;
    }
    case PE_IndicatorArrowRight: {
        QIcon icon = DStyle::standardIcon(SP_ArrowRight, opt, w);
        icon.paint(p, opt->rect);
        return;
    }
    case PE_IndicatorArrowLeft: {
        QIcon icon = DStyle::standardIcon(SP_ArrowLeft, opt, w);
        icon.paint(p, opt->rect);
        return;
    }
    default:
        break;
    }

    DStyle::drawPrimitive(pe, opt, p, w);
}

void ChameleonStyle::drawControl(QStyle::ControlElement element, const QStyleOption *opt,
                                 QPainter *p, const QWidget *w) const
{
    switch (element) {
    case CE_RadioButton:
    case CE_CheckBox:
        if (const QStyleOptionButton *btn = qstyleoption_cast<const QStyleOptionButton *>(opt)) {
            bool isRadio = (element == CE_RadioButton);
            QStyleOptionButton subopt = *btn;
            subopt.rect = subElementRect(isRadio ? SE_RadioButtonIndicator : SE_CheckBoxIndicator, btn, w);
            proxy()->drawPrimitive(isRadio ? PE_IndicatorRadioButton : PE_IndicatorCheckBox, &subopt, p, w);

            subopt.rect = subElementRect(SE_CheckBoxContents, btn, w);
            proxy()->drawControl(CE_CheckBoxLabel, &subopt, p, w);

            if (btn->state & State_HasFocus) {
                QRect rect(subElementRect(isRadio ? SE_RadioButtonFocusRect : SE_CheckBoxFocusRect, btn, w));

                if (isRadio) {
                    p->setPen(QPen(getColor(opt, DPalette::Highlight), DStyle::pixelMetric(PM_FocusBorderWidth)));
                    p->drawEllipse(rect.adjusted(1, 1, -1, -1));
                } else {
                    DDrawUtils::drawBorder(p, rect, getColor(opt, DPalette::Highlight),
                                           DStyle::pixelMetric(PM_FocusBorderWidth), DStyle::pixelMetric(PM_FocusBorderSpacing) + 2);
                }
            }

        }
        return;
    case CE_ScrollBarSlider: {
        if (const QStyleOptionSlider* scrollBar = qstyleoption_cast<const QStyleOptionSlider *>(opt)) {
            p->setBrush(getColor(opt, QPalette::Highlight));
            p->setPen(Qt::NoPen);
            p->setRenderHint(QPainter::Antialiasing);
            QRectF rect = opt->rect;
            QRectF rectHand = rect;
            int realRadius = 0;

            if (opt->state & QStyle::State_Horizontal) {
                rectHand.setHeight(0);
                if (opt->state & QStyle::State_MouseOver)
                    rectHand.setHeight(rect.height());

                static bool bLeaveHor = false;  //动画效果
                if (w) {
                    bool ok = false;
                    int prevValue = w->property("_d_dtk_slider_value").toInt(&ok);
                    const_cast<QWidget*>(w)->setProperty("_d_dtk_slider_value", scrollBar->sliderValue);
                    bool transient = !scrollBar->activeSubControls && !(scrollBar->state & State_On);

                    if (ok && prevValue != scrollBar->sliderValue)
                        rectHand.setHeight(rect.height() / 2);

                    if (!transient) {
                        bLeaveHor = true;
                        rectHand.setHeight(rect.height());
                    }

                    if (transient && bLeaveHor) {
                        bLeaveHor = false;
                        dstyle::DScrollbarStyleAnimation* anim = new dstyle::DScrollbarStyleAnimation(dstyle::DScrollbarStyleAnimation::Deactivating, scrollBar->styleObject);
                        if (anim) {
                            startAnimation(anim);
                            rectHand.setHeight(rect.height() / 2);
                        }
                    }
                }

                realRadius = rectHand.height() / 2.0;
            } else {
                rectHand.setWidth(0);
                if (opt->state & QStyle::State_MouseOver)
                    rectHand.setWidth(rect.width());

                static bool bLeaveVer = false;  //------动画效果开始制作------//是否 箭头从scrollBar上面离开
                if (w) {
                    bool ok = false;
                    int prevValue = w->property("_d_dtk_slider_value").toInt(&ok);
                    const_cast<QWidget*>(w)->setProperty("_d_dtk_slider_value", scrollBar->sliderValue);
                    bool transient = !scrollBar->activeSubControls && !(scrollBar->state & State_On);  //没有被激活

                    if (ok && prevValue != scrollBar->sliderValue) //中键在滚动，但是没有在scrollBar上面
                        rectHand.setWidth(rect.width() / 2);

                    if (!transient) { //处于激活态
                        bLeaveVer = true;
                        rectHand.setWidth(rect.width());
                    }

                    if (transient && bLeaveVer) { //处于离开状态
                        bLeaveVer = false;
                        dstyle::DScrollbarStyleAnimation* anim = new dstyle::DScrollbarStyleAnimation(dstyle::DScrollbarStyleAnimation::Deactivating, scrollBar->styleObject);
                        if (anim) {
                            startAnimation(anim);
                            rectHand.setWidth(rect.width() / 2);
                        }
                    }
                }

                realRadius = rectHand.width() / 2.0;
            }

            rectHand.moveCenter(QRectF(rect).center());
            p->setBrush(getColor(opt, QPalette::Button));
            p->drawRoundedRect(rectHand, realRadius, realRadius);
        }
        break;
    }
    case CE_MenuBarItem: {
        if (const QStyleOptionMenuItem *mbi = qstyleoption_cast<const QStyleOptionMenuItem *>(opt)) {
            p->save();
            QRect rect = mbi->rect;
            drawMenuBarItem(mbi, rect, p, w);
            p->restore();
            return;
        }
    }
    break;
    case CE_MenuBarEmptyArea: {
        p->save();
        QRect menubarRect = opt->rect;
        p->setPen(Qt::NoPen);
        p->setBrush(getColor(opt, QPalette::Window));
        p->drawRect(menubarRect);
        p->restore();
        return;
    }
    case CE_MenuItem: {
        if (const QStyleOptionMenuItem *menuItem = qstyleoption_cast<const QStyleOptionMenuItem *>(opt)) {
            p->save();
            drawMenuItem(menuItem, p, w);
            p->restore();
            return;
        }
        break;
    }
    case CE_MenuEmptyArea: {
        drawMenuItemBackground(opt, p);
        return;
    }
    case CE_MenuScroller: {
        QStyleOption arrowOpt = *opt;
        arrowOpt.state |= State_Enabled;
        int minSize = qMin(arrowOpt.rect.width(), arrowOpt.rect.height());
        arrowOpt.rect.setWidth(minSize);
        arrowOpt.rect.setHeight(minSize);
        arrowOpt.rect.moveCenter(opt->rect.center());
        proxy()->drawPrimitive(((opt->state & State_DownArrow) ? PE_IndicatorArrowDown : PE_IndicatorArrowUp),
                               &arrowOpt, p, w);
        return;
    }
    case CE_PushButton: {
        if (const QStyleOptionButton *btn = qstyleoption_cast<const QStyleOptionButton *>(opt)) {
            proxy()->drawControl(CE_PushButtonBevel, btn, p, w);
            QStyleOptionButton subopt = *btn;
            subopt.rect -= frameExtentMargins();
            subopt.rect = proxy()->subElementRect(SE_PushButtonContents, &subopt, w);
            proxy()->drawControl(CE_PushButtonLabel, &subopt, p, w);

            if (btn->state & State_HasFocus) {
                QStyleOptionFocusRect fropt;
                fropt.QStyleOption::operator=(*btn);
                fropt.rect = proxy()->subElementRect(SE_PushButtonFocusRect, btn, w);
                proxy()->drawPrimitive(PE_FrameFocusRect, &fropt, p, w);
            }

            return;
        }
        break;
    }
    case CE_PushButtonBevel: {
            if (const QStyleOptionButton *btn = qstyleoption_cast<const QStyleOptionButton *>(opt)) {
                QRect br = btn->rect;
                int dbi = proxy()->pixelMetric(PM_ButtonDefaultIndicator, btn, w);
                if (btn->features & QStyleOptionButton::DefaultButton)
                    proxy()->drawPrimitive(PE_FrameDefaultButton, opt, p, w);
                if (btn->features & QStyleOptionButton::AutoDefaultButton)
                    br.setCoords(br.left() + dbi, br.top() + dbi, br.right() - dbi, br.bottom() - dbi);
                if (!(btn->features & (QStyleOptionButton::Flat | QStyleOptionButton::CommandLinkButton))
                    || btn->state & (State_Sunken | State_On)
                    || (btn->features & QStyleOptionButton::CommandLinkButton && btn->state & State_MouseOver)) {
                    QStyleOptionButton tmpBtn = *btn;
                    tmpBtn.rect = br;
                    proxy()->drawPrimitive(PE_PanelButtonCommand, &tmpBtn, p, w);
                }
            }
            return;
        }
    case CE_TabBarTabShape: {
        if (const QStyleOptionTab *tab = qstyleoption_cast<const QStyleOptionTab *>(opt)) {
            if (drawTabBar(p, tab, w))
                return;
        }
    }
    break;
    case CE_TabBarTabLabel: {
        if (const QStyleOptionTab *tab = qstyleoption_cast<const QStyleOptionTab *>(opt)) {
            if (drawTabBarLabel(p, tab, w))
                return;
        }
    }
    break;
    case CE_TabBarTab: {
        if (const QStyleOptionTab *tab = qstyleoption_cast<const QStyleOptionTab*>(opt)) {
            QStyleOptionButton btn;
            int margin = DStyle::pixelMetric(PM_FocusBorderWidth) + DStyle::pixelMetric(PM_FocusBorderSpacing);
            btn.rect = tab->rect.adjusted(margin, margin, -margin, -margin);
            btn.state = tab->state;

            if (tab->state & QStyle::State_Selected) {
                btn.state |= QStyle::State_On;
            }

            p->setRenderHint(QPainter::Antialiasing);
            DStyle::drawControl(CE_PushButtonBevel, &btn, p, w);
            proxy()->drawControl(CE_TabBarTabLabel, opt, p, w);
            return;
        }
        break;
    }
    case CE_RubberBand: {
        if (qstyleoption_cast<const QStyleOptionRubberBand *>(opt)) {
            QColor color = opt->palette.highlight().color();
            color.setAlphaF(0.1);
            p->setBrush(color);
            color.setAlphaF(0.2);
            p->setPen(QPen(color, 1));
            p->drawRect(opt->rect.adjusted(0, 0, -1, -1));
            return;
        }
        break;
    }
    case CE_Header:
        if (const QStyleOptionHeader *header = qstyleoption_cast<const QStyleOptionHeader *>(opt)) {
            QRegion clipRegion = p->clipRegion();
            p->setClipRect(opt->rect);
            proxy()->drawControl(CE_HeaderSection, header, p, w);
            QStyleOptionHeader subopt = *header;
            subopt.rect = subElementRect(SE_HeaderLabel, header, w);
            if (subopt.rect.isValid())
                proxy()->drawControl(CE_HeaderLabel, &subopt, p, w);
            if (header->sortIndicator != QStyleOptionHeader::None) {
                subopt.rect = subElementRect(SE_HeaderArrow, opt, w);
                proxy()->drawPrimitive(PE_IndicatorHeaderArrow, &subopt, p, w);
            }
            p->setClipRegion(clipRegion);
        }
        return;
    case CE_ShapedFrame: {
        if (const QStyleOptionFrame *f = qstyleoption_cast<const QStyleOptionFrame *>(opt)) {
            int frameShape  = f->frameShape;
            int frameShadow = QFrame::Plain;
            if (f->state & QStyle::State_Sunken) {
                frameShadow = QFrame::Sunken;
            } else if (f->state & QStyle::State_Raised) {
                frameShadow = QFrame::Raised;
            }

            int lw = f->lineWidth;
            int mlw = f->midLineWidth;
            QPalette::ColorRole foregroundRole = QPalette::WindowText;
            if (w)
                foregroundRole = w->foregroundRole();

            switch (frameShape) {
            case QFrame::Box:
                if (frameShadow == QFrame::Plain) {
                    qDrawPlainRect(p, f->rect, f->palette.color(foregroundRole), lw);
                } else {
                    qDrawShadeRect(p, f->rect, f->palette, frameShadow == QFrame::Sunken, lw, mlw);
                }
                break;
            case QFrame::StyledPanel:
                //keep the compatibility with Qt 4.4 if there is a proxy style.
                //be sure to call drawPrimitive(QStyle::PE_Frame) on the proxy style
                if (w) {
                    w->style()->drawPrimitive(QStyle::PE_Frame, opt, p, w);
                } else {
                    proxy()->drawPrimitive(QStyle::PE_Frame, opt, p, w);
                }
                break;
            case QFrame::Panel:
                if (frameShadow == QFrame::Plain) {
                    qDrawPlainRect(p, f->rect, f->palette.color(foregroundRole), lw);
                } else {
                    qDrawShadePanel(p, f->rect, f->palette, frameShadow == QFrame::Sunken, lw);
                }
                break;
            case QFrame::WinPanel:
                if (frameShadow == QFrame::Plain) {
                    qDrawPlainRect(p, f->rect, f->palette.color(foregroundRole), lw);
                } else {
                    qDrawWinPanel(p, f->rect, f->palette, frameShadow == QFrame::Sunken);
                }
                break;
            case QFrame::HLine:
            case QFrame::VLine: {
                QPoint p1, p2;
                if (frameShape == QFrame::HLine) {
                    p1 = QPoint(opt->rect.x(), opt->rect.y() + opt->rect.height() / 2);
                    p2 = QPoint(opt->rect.x() + opt->rect.width(), p1.y());
                } else {
                    p1 = QPoint(opt->rect.x() + opt->rect.width() / 2, opt->rect.y());
                    p2 = QPoint(p1.x(), p1.y() + opt->rect.height());
                }
                if (frameShadow == QFrame::Plain) {
                    QPen oldPen = p->pen();
                    QColor color = opt->palette.color(foregroundRole);
                    color.setAlphaF(0.1);
                    p->setPen(QPen(color, lw));
                    p->drawLine(p1, p2);
                    p->setPen(oldPen);
                } else {
                    qDrawShadeLine(p, p1, p2, f->palette, frameShadow == QFrame::Sunken, lw, mlw);
                }
                break;
            }
            }
            return;
        }
        break;
    }
    case CE_ComboBoxLabel: {
        if (const QStyleOptionComboBox *cb = qstyleoption_cast<const QStyleOptionComboBox *>(opt)) {
            if (drawComboBoxLabel(p, cb, w))
                return;
        }
        break;
    }
    case CE_PushButtonLabel:
        if (const QStyleOptionButton *button = qstyleoption_cast<const QStyleOptionButton *>(opt)) {
            QRect textRect = button->rect;
            uint tf = Qt::AlignVCenter | Qt::TextShowMnemonic;
            if (!proxy()->styleHint(SH_UnderlineShortcut, button, w))
                tf |= Qt::TextHideMnemonic;

            const QPalette::ColorRole &text_color_role = opt->state & State_On ? QPalette::HighlightedText : QPalette::ButtonText;

            QPalette pa = button->palette;

            if (button->features & DStyleOptionButton::WarningButton) {
                pa.setBrush(QPalette::ButtonText, getColor(opt, DPalette::TextWarning, w));
            } else if (button->features & DStyleOptionButton::SuggestButton) {
                pa.setBrush(QPalette::ButtonText, getColor(opt, QPalette::HighlightedText));
            } else {
                pa.setBrush(QPalette::ButtonText, getColor(opt, text_color_role));
            }

            // 设置文字和图标的绘制颜色
            p->setPen(QPen(pa.buttonText(), 1));

            if (!button->icon.isNull()) {
                //Center both icon and text
                QRect iconRect;
                QIcon::Mode mode = button->state & State_Enabled ? QIcon::Normal : QIcon::Disabled;
                if (mode == QIcon::Normal && button->state & State_HasFocus)
                    mode = QIcon::Active;
                QIcon::State state = QIcon::Off;
                if (button->state & State_On)
                    state = QIcon::On;

                int pixmapWidth = button->iconSize.width();
                int pixmapHeight = button->iconSize.height();
                int labelWidth = pixmapWidth;
                int labelHeight = pixmapHeight;
                int iconSpacing = 4;//### 4 is currently hardcoded in QPushButton::sizeHint()
                int textWidth = button->fontMetrics.boundingRect(opt->rect, tf, button->text).width();
                if (!button->text.isEmpty())
                    labelWidth += (textWidth + iconSpacing);

                iconRect = QRect(textRect.x() + (textRect.width() - labelWidth) / 2,
                                 textRect.y() + (textRect.height() - labelHeight) / 2,
                                 pixmapWidth, pixmapHeight);

                iconRect = visualRect(button->direction, textRect, iconRect);

                tf |= Qt::AlignLeft; //left align, we adjust the text-rect instead

                if (button->direction == Qt::RightToLeft)
                    textRect.setRight(iconRect.left() - iconSpacing);
                else
                    textRect.setLeft(iconRect.left() + iconRect.width() + iconSpacing);

                if (button->state & (State_On | State_Sunken))
                    iconRect.translate(proxy()->pixelMetric(PM_ButtonShiftHorizontal, opt, w),
                                       proxy()->pixelMetric(PM_ButtonShiftVertical, opt, w));

                button->icon.paint(p, iconRect, Qt::AlignCenter, mode, state);
            } else {
                tf |= Qt::AlignHCenter;
            }
            if (button->state & (State_On | State_Sunken))
                textRect.translate(proxy()->pixelMetric(PM_ButtonShiftHorizontal, opt, w),
                                   proxy()->pixelMetric(PM_ButtonShiftVertical, opt, w));

            if (button->features & QStyleOptionButton::HasMenu) {
                QRect rectArrowAndLine = drawButtonDownArrow(opt, nullptr, w);
                int frameRadius = DStyle::pixelMetric(PM_FrameRadius);
                drawButtonDownArrow(button, p, w);

                if (button->direction == Qt::LeftToRight) {
                    textRect.setRight(rectArrowAndLine.left() - frameRadius);
                } else {
                    textRect.setLeft(rectArrowAndLine.right() + frameRadius);
                }
            }

            proxy()->drawItemText(p, textRect, tf, pa, (button->state & State_Enabled),
                                  button->text, QPalette::ButtonText);
            return;
        }
        break;
    case CE_ProgressBar: {  //显示进度区域
        if (const QStyleOptionProgressBar *progBar =  qstyleoption_cast<const QStyleOptionProgressBar *>(opt)) {
            p->setRenderHint(QPainter::Antialiasing);
            p->setPen(Qt::NoPen);
            p->drawRect(opt->rect);

            QStyleOptionProgressBar progGroove = *progBar;
            proxy()->drawControl(CE_ProgressBarGroove, &progGroove, p, w);

            QRect rect = progBar->rect;   //滑块区域矩形
            int min = progBar->minimum;
            int max = progBar->maximum;
            int val = progBar->progress;
            int drawWidth = 0;

            if (progBar->orientation == Qt::Horizontal) {
                drawWidth = (val * 1.0 / (max - min)) * rect.width();
                rect = QRect(rect.left(), rect.top(), drawWidth, rect.height());
            } else {
                drawWidth = (val * 1.0 / (max - min)) * rect.height();
                rect = QRect(rect.left(), rect.bottom() - drawWidth, rect.width(), drawWidth);
            }

            p->setPen(Qt::NoPen);
            QPointF pointStart(rect.left(), rect.center().y());
            QPointF pointEnd(rect.right(), rect.center().y());
            QLinearGradient linear(pointStart, pointEnd);
            linear.setColorAt(0, getColor(opt, DPalette::DarkLively, w));
            linear.setColorAt(1, getColor(opt, DPalette::LightLively, w));
            linear.setSpread(QGradient::PadSpread);

            QStyleOptionProgressBar subopt = *progBar;
            subopt.rect = proxy()->subElementRect(SE_ProgressBarContents, progBar, w);
            subopt.palette.setBrush(QPalette::ColorRole::Highlight, QBrush(linear));
            proxy()->drawControl(CE_ProgressBarContents, &subopt, p, w);

            if (progBar->textVisible && progBar->orientation == Qt::Horizontal) {
                subopt.rect = proxy()->subElementRect(SE_ProgressBarLabel, progBar, w);
                proxy()->drawControl(CE_ProgressBarLabel, &subopt, p, w);
            }
        }
        return;
    }
    case CE_ProgressBarGroove: {  //滑槽显示
        int frameRadius = DStyle::pixelMetric(PM_FrameRadius, opt, w);
        int height = qstyleoption_cast<const QStyleOptionProgressBar *>(opt)->orientation == Qt::Horizontal ? opt->rect.height() : opt->rect.width();
        if (frameRadius >= height) {
            frameRadius = height / 2;
        }
        p->setBrush(getColor(opt, DPalette::Button));
        p->drawRoundedRect(opt->rect, frameRadius, frameRadius);
        return;
    }
    case CE_ProgressBarContents: { //进度滑块显示
        if (const QStyleOptionProgressBar *progBar =  qstyleoption_cast<const QStyleOptionProgressBar *>(opt)) {
            QRect rect = progBar->rect;   //滑块区域矩形
            int min = progBar->minimum;
            int max = progBar->maximum;
            int val = progBar->progress;
            int drawWidth = 0;
            int frameRadius = DStyle::pixelMetric(PM_FrameRadius, opt, w);
            int height = progBar->orientation == Qt::Horizontal ? rect.height() : rect.width();
            if (frameRadius >= height) {
                frameRadius = height / 2;
            }

            if (progBar->orientation == Qt::Horizontal) {
                drawWidth = (val * 1.0 / (max - min)) * rect.width();
                rect = QRect(rect.left(), rect.top(), drawWidth, rect.height());
            } else {
                drawWidth = (val * 1.0 / (max - min)) * rect.height();
                rect = QRect(rect.left(), rect.bottom() - drawWidth, rect.width(), drawWidth);
            }

            p->setBrush(opt->palette.brush(QPalette::ColorRole::Highlight));

            if (progBar->textVisible) {
                QPainterPath pathRect;
                pathRect.addRect(rect);
                QPainterPath pathRoundRect;
                pathRoundRect.addRoundedRect(opt->rect, frameRadius, frameRadius);
                QPainterPath inter = pathRoundRect.intersected(pathRect);
                p->drawPath(inter);
            } else {
                p->drawRoundedRect(rect, frameRadius, frameRadius);
            }
        }
        return;
    }
    case CE_ProgressBarLabel: {
        if (const QStyleOptionProgressBar *progBar =  qstyleoption_cast<const QStyleOptionProgressBar *>(opt)) {
            double val = progBar->progress * 1.0 / (progBar->maximum - progBar->minimum);
            int drawWidth = val * opt->rect.width();
            QRect rect = progBar->fontMetrics.boundingRect(progBar->rect, progBar->textAlignment, progBar->text);

            if (rect.left() <= drawWidth && drawWidth <= rect.right()) {
                double division = (drawWidth - rect.left()) / (rect.width() * 1.0);
                QPointF pointStart(rect.left(), rect.center().y());
                QPointF pointEnd(rect.right(), rect.center().y());
                QLinearGradient linear(pointStart, pointEnd);
                linear.setColorAt(0, getColor(opt, DPalette::HighlightedText));
                linear.setColorAt(division, getColor(opt, DPalette::HighlightedText));
                linear.setColorAt(division + 0.01, getColor(opt, DPalette::ButtonText));
                linear.setColorAt(1, getColor(opt, DPalette::ButtonText));
                linear.setSpread(QGradient::PadSpread);

                p->setPen(QPen(QBrush(linear), 1));      //设置画笔渐变色
            } else if (drawWidth < rect.left()) {
                p->setPen(getColor(opt, DPalette::ButtonText));
            } else {
                p->setPen(getColor(opt, DPalette::HighlightedText));
            }

            p->drawText(progBar->rect, progBar->textAlignment, progBar->text);
        }
        return;
    }
    case CE_ToolButtonLabel: {
        if (const QStyleOptionToolButton *toolbutton = qstyleoption_cast<const QStyleOptionToolButton *>(opt)) {
            QRect rect = toolbutton->rect;
            int radius = DStyle::pixelMetric(PM_FrameRadius, opt, w); //在绘画icon和text之前,先绘画一层表示靠近或按下状态
            p->setRenderHint(QPainter::Antialiasing);
            p->setPen(Qt::NoPen);
            p->setBrush(Qt::NoBrush);

            if (toolbutton->state & (State_MouseOver | State_Sunken))   //hover状态 、press状态
                p->setBrush(getColor(toolbutton, DPalette::Button));

            // Arrow type always overrules and is always shown
            bool hasArrow = toolbutton->features & QStyleOptionToolButton::Arrow;
            if (((!hasArrow && toolbutton->icon.isNull()) && !toolbutton->text.isEmpty())
                || toolbutton->toolButtonStyle == Qt::ToolButtonTextOnly) {               //只显示文字的情景
                int alignment = Qt::AlignCenter | Qt::TextShowMnemonic;
                if (!proxy()->styleHint(SH_UnderlineShortcut, opt, w))
                    alignment |= Qt::TextHideMnemonic;
                p->setFont(toolbutton->font);
                p->drawRoundedRect(rect, radius, radius);

                if (toolbutton->state & State_On) {
                    p->setPen(getColor(toolbutton, DPalette::Highlight));
                } else {
                    p->setPen(getColor(toolbutton, DPalette::ButtonText));
                }

                p->drawText(rect, alignment, toolbutton->text);
            } else { //只显示文字的情景 的 补集
                QIcon icon(toolbutton->icon.pixmap(QSize()));
                QSize pmSize = toolbutton->iconSize;

                switch (toolbutton->arrowType) {
                case Qt::UpArrow:
                    icon = DStyle::standardIcon(SP_ArrowUp);
                    break;
                case Qt::DownArrow:
                    icon = DStyle::standardIcon(SP_ArrowDown);
                    break;
                case Qt::LeftArrow:
                    icon = DStyle::standardIcon(SP_ArrowLeft);
                    break;
                case Qt::RightArrow:
                    icon = DStyle::standardIcon(SP_ArrowRight);
                    break;
                default:
                    icon = toolbutton->icon;
                    break;
                }

                int radius = DStyle::pixelMetric(PM_FrameRadius, opt, w); //在绘画icon和text之前,先绘画一层表示靠近或按下状态
                p->setRenderHint(QPainter::Antialiasing);
                p->setPen(Qt::NoPen);
                p->setBrush(Qt::NoBrush);

                if (toolbutton->state & (State_MouseOver | State_Sunken))   //hover状态 、press状态
                    p->setBrush(getColor(toolbutton, DPalette::Button));

                p->drawRoundedRect(rect, radius, radius);

                QRect pr = rect;
                QRect tr = rect;
                pr.setHeight(pmSize.height());
                pr.setWidth(pmSize.width());

                if (toolbutton->state & State_On)      //active状态
                    p->setPen(getColor(toolbutton, DPalette::Highlight));
                else
                    p->setPen(getColor(toolbutton, DPalette::ButtonText));

                if (toolbutton->toolButtonStyle != Qt::ToolButtonIconOnly) { //只显示icon 的补集情况
                    p->setFont(toolbutton->font);

                    int alignment = Qt::TextShowMnemonic;
                    if (!proxy()->styleHint(SH_UnderlineShortcut, opt, w))
                        alignment |= Qt::TextHideMnemonic;

                    if (toolbutton->toolButtonStyle == Qt::ToolButtonTextUnderIcon) {  //文字在图标下面
                        pr.moveCenter(QPoint(rect.center().x(), rect.center().y() / 2));
                        tr.adjust(0, pr.height(), 0, 0);

                        drawIcon(toolbutton, p, pr, icon);
                        alignment |= Qt::AlignCenter;

                    } else {    //其他几种（文字和icon布局）方式
                        int radius = DStyle::pixelMetric(PM_FrameRadius);
                        pr.moveCenter(QPoint(rect.left(), rect.center().y()));
                        pr.moveLeft(radius);
                        tr.adjust(pr.width() + radius, 0, 0, 0);

                        drawIcon(toolbutton, p, pr, icon);
                        alignment |= Qt::AlignLeft | Qt::AlignVCenter;
                    }

                    p->drawText(tr, alignment, toolbutton->text);
                } else {   //只显示icon情况
                    pr.moveCenter(rect.center());
                    drawIcon(toolbutton, p, pr, icon);
                }
            }
        }
        return;
    }
    case CE_HeaderSection: {
        const auto headerOption( qstyleoption_cast<const QStyleOptionHeader*>( opt ) );
        if (!headerOption) return;
        const bool horizontal( headerOption->orientation == Qt::Horizontal );
        const bool isLast( headerOption->position == QStyleOptionHeader::End );

        // fill background
        QColor color(opt->palette.color(QPalette::Base));
        color = DGuiApplicationHelper::adjustColor(color, 0, 0, -5, 0, 0, 0, 0);
        p->fillRect(opt->rect, QBrush(color));

        QColor lineColor(opt->palette.color(QPalette::Base));
        if (DGuiApplicationHelper::toColorType(lineColor) == DGuiApplicationHelper::LightType) {
            lineColor = DGuiApplicationHelper::adjustColor(color, 0, 0, -10, 0, 0, 0, 0);
        } else {
            lineColor = DGuiApplicationHelper::adjustColor(color, 0, 0, +10, 0, 0, 0, 0);
        }
        p->setPen(lineColor);
        if (horizontal) {
            if (!isLast) {
                QPoint unit(0, opt->rect.height() / 5);
                p->drawLine(opt->rect.topRight() + unit, opt->rect.bottomRight() - unit);
            }
            p->drawLine(opt->rect.bottomLeft(), opt->rect.bottomRight());
        } else {
            if (!isLast) {
                p->drawLine(opt->rect.bottomLeft(), opt->rect.bottomRight());
            }
            p->drawLine(opt->rect.topRight(), opt->rect.bottomRight());
        }
        return;
    }
    default:
        break;
    }

    DStyle::drawControl(element, opt, p, w);
}

bool ChameleonStyle::drawTabBar(QPainter *painter,  const QStyleOptionTab *tab, const QWidget *widget) const
{
    const QTabBar *m_tabbar = qobject_cast<const QTabBar *>(widget);

    if (!m_tabbar)
        return false;

    painter->fillRect(tab->rect, getColor(tab, QPalette::Window));
    painter->save();

    bool isTriangularMode = false;
    bool rtlHorTabs = (tab->direction == Qt::RightToLeft
                       && (tab->shape == QTabBar::RoundedNorth
                           || tab->shape == QTabBar::RoundedSouth));
    bool selected = tab->state & State_Selected && tab->state & State_Enabled;
    bool lastTab = ((!rtlHorTabs && tab->position == QStyleOptionTab::End)
                    || (rtlHorTabs
                        && tab->position == QStyleOptionTab::Beginning));
    bool onlyOne = tab->position == QStyleOptionTab::OnlyOneTab;
    int tabOverlap = proxy()->pixelMetric(PM_TabBarTabOverlap, tab, widget);
    QRect rect = tab->rect.adjusted(0, 0, (onlyOne || lastTab) ? 0 : tabOverlap, 0);

    QRect r2(rect);
    int x1 = r2.left();
    int x2 = r2.right();
    int y1 = r2.top();
    int y2 = r2.bottom();

    QTransform rotMatrix;
    bool flip = false;
    painter->setPen(getColor(tab, DPalette::Shadow));

    switch (tab->shape) {
    case QTabBar::TriangularNorth:
        rect.adjust(0, 0, 0, -tabOverlap);
        isTriangularMode = true;
        break;
    case QTabBar::TriangularSouth:
        rect.adjust(0, tabOverlap, 0, 0);
        isTriangularMode = true;
        break;
    case QTabBar::TriangularEast:
        rect.adjust(tabOverlap, 0, 0, 0);
        isTriangularMode = true;
        break;
    case QTabBar::TriangularWest:
        rect.adjust(0, 0, -tabOverlap, 0);
        isTriangularMode = true;
        break;
    case QTabBar::RoundedNorth:
        break;
    case QTabBar::RoundedSouth:
        rotMatrix.rotate(180);
        rotMatrix.translate(0, -rect.height() + 1);
        rotMatrix.scale(-1, 1);
        painter->setTransform(rotMatrix, true);
        break;
    case QTabBar::RoundedWest:
        rotMatrix.rotate(180 + 90);
        rotMatrix.scale(-1, 1);
        flip = true;
        painter->setTransform(rotMatrix, true);
        break;
    case QTabBar::RoundedEast:
        rotMatrix.rotate(90);
        rotMatrix.translate(0, - rect.width() + 1);
        flip = true;
        painter->setTransform(rotMatrix, true);
        break;
    }

    if (flip) {
        QRect tmp = rect;
        rect = QRect(tmp.y(), tmp.x(), tmp.height(), tmp.width());
        int temp = x1;
        x1 = y1;
        y1 = temp;
        temp = x2;
        x2 = y2;
        y2 = temp;
    }

    QColor lineColor = !isTriangularMode || selected ? Qt::transparent : getColor(tab, QPalette::Light);
    QColor tabFrameColor = selected ? getColor(tab, QPalette::Window) : getColor(tab, QPalette::Button);

    if (!(tab->features & QStyleOptionTab::HasFrame))
        tabFrameColor = getColor(tab, QPalette::Shadow);

    if (!isTriangularMode)
        tabFrameColor = selected ? getColor(tab, QPalette::Highlight) : getColor(tab, QPalette::Button);

    QPen outlinePen(lineColor, proxy()->pixelMetric(PM_DefaultFrameWidth, tab, widget));
    QRect drawRect = rect;
    painter->setPen(outlinePen);
    painter->setBrush(tabFrameColor);
    painter->setRenderHint(QPainter::Antialiasing, true);

    if (!isTriangularMode) {
        int buttonRadius = DStyle::pixelMetric(PM_FrameRadius, tab, widget);
        int buttonBorder = DStyle::pixelMetric(PM_FrameMargins, tab, widget);

        painter->drawRoundedRect(drawRect.adjusted(buttonBorder, buttonBorder, -buttonBorder, -buttonBorder), buttonRadius, buttonRadius);
    } else {
        painter->drawRect(drawRect);
    }

    painter->restore();
    return true;
}

bool ChameleonStyle::drawTabBarLabel(QPainter *painter, const QStyleOptionTab *tab, const QWidget *widget) const
{
    const QTabBar *m_tabbar = static_cast<const QTabBar *>(widget);

    if (!m_tabbar)
        return false;

    bool isTriangularMode = false;
    bool selected = tab->state & State_Selected && tab->state & State_Enabled;

    switch (tab->shape) {
    case QTabBar::TriangularNorth:
    case QTabBar::TriangularSouth:
    case QTabBar::TriangularEast:
    case QTabBar::TriangularWest:
        isTriangularMode = true;
        break;
    default:
        break;
    }

    bool visible_close_button = selected;

    if (visible_close_button) {
        if (const DTabBar *tb = qobject_cast<const DTabBar*>(widget)) {
            visible_close_button = tb->tabsClosable();
        } else {
            visible_close_button = false;
        }
    }

    if (selected) {
        QStyleOptionTab newTab = *tab;
        newTab.palette.setBrush(QPalette::WindowText, getColor(tab, isTriangularMode ? QPalette::BrightText : QPalette::HighlightedText));

        if (visible_close_button) {
            QRect tr = proxy()->subElementRect(SE_TabBarTabText, tab, widget);
            QRect text_rect = tab->fontMetrics.boundingRect(tr, Qt::AlignCenter | Qt::TextShowMnemonic, tab->text);

            int close_button_width = proxy()->pixelMetric(QStyle::PM_TabCloseIndicatorWidth, tab, widget);
            qreal stop = qreal(tr.right() - close_button_width - text_rect.x() - 5) / text_rect.width();

            if (stop < 1.0 && tr.right() - close_button_width < text_rect.right()) {
                const QBrush &brush = newTab.palette.windowText();
                QLinearGradient lg(0, 0, 1, 0);
                QGradientStops stops;
                qreal offset = 5.0 / text_rect.width();

                // 接近关闭按钮部分的颜色渐变到透明
                stops << QGradientStop{0, brush.color()};
                stops << QGradientStop{stop - offset, brush.color()};
                stops << QGradientStop{stop, QColor(brush.color().red(), brush.color().green(), brush.color().blue(), 100)};

                // 保证文字超出最大可显示区域的部分为透明
                if (text_rect.right() > tr.right())
                    stops << QGradientStop{1 - (text_rect.right() - tr.right()) / qreal(text_rect.width()), Qt::transparent};

                stops << QGradientStop{1, Qt::transparent};

                lg.setCoordinateMode(QLinearGradient::ObjectBoundingMode);
                lg.setStops(stops);
                newTab.palette.setBrush(QPalette::WindowText, lg);
            }
        }

        QCommonStyle::drawControl(CE_TabBarTabLabel, &newTab, painter, widget);
    } else {
        QCommonStyle::drawControl(CE_TabBarTabLabel, tab, painter, widget);
    }

    return true;
}

bool ChameleonStyle::drawTabBarCloseButton(QPainter *painter, const QStyleOption *tab, const QWidget *widget) const
{
    const QTabBar *tb = qobject_cast<QTabBar *>(widget->parent());

    if (!tb) {
        if (const QWidget *w = dynamic_cast<const QWidget *>(painter->device()))
            widget = w;

        tb = qobject_cast<QTabBar *>(widget->parent());
    }

    if (Q_UNLIKELY(!tb))
        return false;

    int index = -1;

    for (int i = 0; i < tb->count(); ++i) {

        if (Q_LIKELY(tb->tabButton(i, QTabBar::LeftSide) != widget
                     && tb->tabButton(i, QTabBar::RightSide) != widget)) {
            continue;
        }

        index = i;
        break;
    }

    if (Q_UNLIKELY(index < 0))
        return true;

    QStyleOptionButton buttonOpt;
    buttonOpt.rect = tab->rect;
    buttonOpt.state = tab->state;
    QColor iconColor = getColor(&buttonOpt, QPalette::ButtonText);
    painter->setPen(QPen(iconColor, 2));
    painter->setBrush(getColor(&buttonOpt, QPalette::Button));
    DDrawUtils::drawCloseButton(painter, buttonOpt.rect);

    return true;
}

bool ChameleonStyle::drawTabBarScrollButton(QPainter *painter, const QStyleOption *opt, const QWidget *buttonWidget) const
{
    const QToolButton *tabButton = qobject_cast<const QToolButton *>(buttonWidget);

    if (!tabButton)
        return false;

    if (tabButton->arrowType() == Qt::NoArrow || !tabButton->icon().isNull())
        return false;

    QTabBar *tabBar = qobject_cast<QTabBar *>(buttonWidget->parent());
    bool isTriangularMode = false;

    switch (tabBar->shape()) {
    case QTabBar::TriangularNorth:
    case QTabBar::TriangularSouth:
    case QTabBar::TriangularEast:
    case QTabBar::TriangularWest:
        isTriangularMode = true;
        break;
    default:
        break;
    }

    QStyleOptionToolButton toolButton(*qstyleoption_cast<const QStyleOptionToolButton *>(opt));
    int frameMargin = DStyle::pixelMetric(PM_FrameMargins);
    toolButton.rect -= QMargins(frameMargin, frameMargin, frameMargin, frameMargin);

    QPoint center = toolButton.rect.center();
    qreal sizeRatio = isTriangularMode ? (4.0 / 7.0) : 1.0;
    int minBoundWidth = qMin(toolButton.rect.width(), toolButton.rect.height());
    toolButton.rect.setWidth(qRound(minBoundWidth * sizeRatio));
    toolButton.rect.setHeight(qRound(minBoundWidth * sizeRatio));
    toolButton.rect.moveCenter(center);
    DDrawUtils::Corners corner = static_cast<DDrawUtils::Corners>(0xff);

    int radius = isTriangularMode ? toolButton.rect.width() / 2 :  DStyle::pixelMetric(PM_FrameRadius);
    QLinearGradient lg(QPointF(0, opt->rect.top()),
                       QPointF(0, opt->rect.bottom()));
    lg.setColorAt(0, getColor(opt, QPalette::Light));
    lg.setColorAt(1, getColor(opt, QPalette::Dark));

    painter->setPen(QPen(getColor(opt, DPalette::FrameBorder, buttonWidget), Metrics::Painter_PenWidth));
    painter->setBrush(lg);
    painter->setRenderHint(QPainter::Antialiasing);
    drawShadow(painter, toolButton.rect + frameExtentMargins() * 1.5, getColor(&toolButton, QPalette::Shadow));
    DDrawUtils::drawRoundedRect(painter, toolButton.rect, radius, radius, corner);

    QPoint originCenter = toolButton.rect.center();
    qreal buttonSizeRatio = isTriangularMode ?  3.0 / 4.0 : 1.0 / 2.0;
    toolButton.rect.setWidth(qRound(toolButton.rect.width() * buttonSizeRatio));
    toolButton.rect.setHeight(qRound(toolButton.rect.height() * buttonSizeRatio));
    toolButton.rect.moveCenter(originCenter);
    proxy()->drawControl(CE_ToolButtonLabel, &toolButton, painter, buttonWidget);

    return true;
}

bool ChameleonStyle::drawComboBox(QPainter *painter, const QStyleOptionComboBox *comboBox, const QWidget *widget) const
{
    if (comboBox->editable && comboBox->state.testFlag(QStyle::State_HasFocus)) {
        proxy()->drawPrimitive(PE_FrameFocusRect, comboBox, painter, widget);
    }

    QRect rect(comboBox->rect);
    QStyleOptionComboBox comboBoxCopy = *comboBox;
    QRect downArrowRect = proxy()->subControlRect(CC_ComboBox, &comboBoxCopy, SC_ComboBoxArrow, widget);

    if (comboBox->frame && comboBox->subControls & SC_ComboBoxFrame) {
        int frameRadius = DStyle::pixelMetric(PM_FrameRadius);
        painter->setPen(Qt::NoPen);
        painter->setBrush(comboBoxCopy.palette.button());
        DDrawUtils::drawRoundedRect(painter, comboBoxCopy.rect - frameExtentMargins(), frameRadius, frameRadius,
                                    DDrawUtils::Corner::TopLeftCorner
                                    | DDrawUtils::Corner::TopRightCorner
                                    | DDrawUtils::Corner::BottomLeftCorner
                                    | DDrawUtils::Corner::BottomRightCorner);
    }


    QStyleOptionButton buttonOption;
    buttonOption.QStyleOption::operator=(*comboBox);
    if (comboBox->editable) {
        buttonOption.rect = rect - frameExtentMargins();
        buttonOption.state = (comboBox->state & (State_Enabled | State_MouseOver | State_HasFocus))
                             | State_KeyboardFocusChange; // Always show hig

        if (comboBox->state & State_Sunken) {
            buttonOption.state |= State_Sunken;
            buttonOption.state &= ~State_MouseOver;
        }

        if (comboBox->direction == Qt::LeftToRight)
            buttonOption.rect.setLeft(downArrowRect.left());
        else
            buttonOption.rect.setRight(downArrowRect.right());

        downArrowRect.moveCenter(buttonOption.rect.center());
        proxy()->drawPrimitive(PE_PanelButtonCommand, &buttonOption, painter, widget);
    } else {
        buttonOption.rect = rect;
        buttonOption.state = comboBox->state & (State_Enabled | State_MouseOver | State_HasFocus | State_KeyboardFocusChange);

        if (comboBox->state & State_Sunken) {
            buttonOption.state |= State_Sunken;
            buttonOption.state &= ~State_MouseOver;
        }

        downArrowRect.moveCenter(buttonOption.rect.center());

        if (comboBox->direction == Qt::LeftToRight) {
            downArrowRect.moveRight(buttonOption.rect.right());
        } else {
            downArrowRect.moveLeft(buttonOption.rect.left());
        }

        proxy()->drawPrimitive(PE_PanelButtonCommand, &buttonOption, painter, widget);
    }

    if (comboBox->subControls & SC_ComboBoxArrow) {
        QStyleOption arrowOpt = *comboBox;
        arrowOpt.rect =  downArrowRect - frameExtentMargins();

        if (comboBox->editable) {
            arrowOpt.rect.setSize(QSize(qRound(buttonOption.rect.width() / 3.0), qRound(buttonOption.rect.height() / 3.0)));
            arrowOpt.rect.moveCenter(buttonOption.rect.center());
        } else {
            QPoint center = arrowOpt.rect.center();
            arrowOpt.rect.setSize(QSize(qRound(arrowOpt.rect.height() / 2.4), qRound(arrowOpt.rect.height() / 2.4)));
            arrowOpt.rect.moveCenter(center);
            int radius = DStyle::pixelMetric(PM_FrameRadius);
            arrowOpt.rect = arrowOpt.rect.adjusted(-radius, 0, -radius, 0);
        }

        painter->setPen(getColor(comboBox, DPalette::ButtonText));
        proxy()->drawPrimitive(PE_IndicatorArrowDown, &arrowOpt, painter, widget);
    }

    return true;
}

bool ChameleonStyle::drawComboBoxLabel(QPainter *painter, const QStyleOptionComboBox *cb, const QWidget *widget) const
{
    const bool hasText(!cb->currentText.isEmpty());
    const bool hasIcon(!cb->currentIcon.isNull());
//    const bool enabled(cb->state & QStyle::State_Enabled);
    const bool sunken(cb->state & (QStyle::State_On | QStyle::State_Sunken));
//    const bool mouseOver(cb->state & QStyle::State_MouseOver);
//    const bool hasFocus(cb->state & QStyle::State_HasFocus);
    const bool flat(!cb->frame);
    const bool editable(cb->editable);

    QRect contentsRect(cb->rect);
    if (sunken && !flat) contentsRect.translate(1, 1);
    contentsRect.adjust(Metrics::Layout_ChildMarginWidth, 0, -Metrics::Layout_ChildMarginWidth - DStyle::pixelMetric(PM_FrameRadius), 0);
    QSize iconSize;
    if (hasIcon) {
        iconSize = cb->iconSize;
        if (!iconSize.isValid()) {
            const int metric(widget->style()->pixelMetric(QStyle::PM_SmallIconSize, cb, widget));
            iconSize = QSize(metric, metric);
        }
    }

    int textFlags(Qt::AlignVCenter | Qt::AlignLeft);
    const QSize textSize(cb->fontMetrics.size(textFlags, cb->currentText));

    if (styleHint(SH_UnderlineShortcut, cb, widget))
        textFlags |= Qt::TextShowMnemonic;
    else
        textFlags |= Qt::TextHideMnemonic;

    QRect iconRect;
    QRect textRect;
    QRect downArrowRect = proxy()->subControlRect(CC_ComboBox, cb, SC_ComboBoxArrow, widget);

    if (hasText && !hasIcon) {
        textRect = contentsRect;
        int frame_radius = DStyle::pixelMetric(PM_FrameRadius, cb, widget);
        textRect.adjust(frame_radius, 0, 0, 0);
        textRect.setWidth(textRect.width() - downArrowRect.width());
    }
    else {
        const int contentsWidth(iconSize.width() + textSize.width() + Metrics::Button_ItemSpacing);
        const int contentLeftPadding = flat ? (contentsRect.width() - contentsWidth) / 2 : frameExtentMargins().left();
        iconRect = QRect(QPoint(contentsRect.left() + contentLeftPadding,
                                contentsRect.top() + (contentsRect.height() - iconSize.height()) / 2), iconSize);
        textRect = QRect(QPoint(iconRect.right() + Metrics::Button_ItemSpacing + 1,
                                contentsRect.top() + (contentsRect.height() - textSize.height()) / 2), textSize);
    }

    // handle right to left
    if (iconRect.isValid()) iconRect = visualRect(cb->direction, cb->rect, iconRect);
    if (textRect.isValid()) textRect = visualRect(cb->direction, cb->rect, textRect);

    // render icon
    if (hasIcon && iconRect.isValid()) {
        // icon state and mode
        cb->currentIcon.paint(painter, iconRect, Qt::AlignLeft);
    }

    // render text
    if (hasText && textRect.isValid() && !editable) {
        painter->setPen(getColor(cb, QPalette::ButtonText));
        QString text = painter->fontMetrics().elidedText(cb->currentText, Qt::ElideRight, textRect.width());
        painter->drawText(textRect, textFlags, text);
    }

    return true;
}

void ChameleonStyle::drawSliderHandle(const QStyleOptionComplex *opt, QRectF& rectHandle, QPainter *p, const QWidget *w) const
{
    if (const QStyleOptionSlider *slider = qstyleoption_cast<const QStyleOptionSlider *>(opt)) {
        const DSlider *dslider = qobject_cast<const DSlider *>(w->parent());
        QSlider::TickPosition tickPosition = slider->tickPosition;

        if (dslider)
            tickPosition = dslider->tickPosition();

        if (tickPosition == QSlider::NoTicks) {
            p->drawRoundedRect(rectHandle, DStyle::pixelMetric(DStyle::PM_FrameRadius), DStyle::pixelMetric(DStyle::PM_FrameRadius));
        } else {
            qreal radius = DStyle::pixelMetric(DStyle::PM_FrameRadius);
            QRectF rectRoundedPart(0, 0, 0, 0);

            if (slider->orientation == Qt::Horizontal) {
                if (tickPosition == QSlider::TicksAbove) {  //尖角朝上
                    rectRoundedPart = QRectF(rectHandle.left(), rectHandle.bottom() - 2 * radius, rectHandle.width(), 2 * radius);
                    QPointF polygon[5] = { QPointF(rectHandle.left(), rectHandle.bottom() - radius)
                                           , QPointF(rectHandle.left(), rectHandle.top() + radius)
                                           , QPointF(rectHandle.center().x(), rectHandle.top())
                                           , QPointF(rectHandle.right(), rectHandle.top() + radius)
                                           , QPointF(rectHandle.right(), rectHandle.bottom() - radius)};
                    p->drawPolygon(polygon, 5);
                } else { //尖角朝下
                    rectRoundedPart = QRectF(rectHandle.left(), rectHandle.top(), rectHandle.width(), 2 * radius);
                    QPointF polygon[5] = {   QPointF(rectHandle.left(), rectHandle.top() + radius)
                                           , QPointF(rectHandle.left(), rectHandle.bottom() - radius)
                                           , QPointF(rectHandle.center().x(), rectHandle.bottom())
                                           , QPointF(rectHandle.right(), rectHandle.bottom() - radius)
                                           , QPointF(rectHandle.right(), rectHandle.top() + radius)};
                    p->drawPolygon(polygon, 5);
                }
            } else {
                if (tickPosition == QSlider::TicksLeft) {  //尖角朝左
                    rectRoundedPart = QRectF(rectHandle.right() - 2 * radius, rectHandle.top(), 2 * radius, rectHandle.height());
                    QPointF polygon[5] = {   QPointF(rectHandle.right() - radius, rectHandle.top())
                                           , QPointF(rectHandle.left() + radius, rectHandle.top())
                                           , QPointF(rectHandle.left(), rectHandle.center().y())
                                           , QPointF(rectHandle.left() + radius, rectHandle.bottom())
                                           , QPointF(rectHandle.right() - radius, rectHandle.bottom())};
                    p->drawPolygon(polygon, 5);
                } else { //尖角朝右
                    rectRoundedPart = QRectF(rectHandle.left(), rectHandle.top(), 2 * radius, rectHandle.height());
                    QPointF polygon[5] = {   QPointF(rectHandle.left() + radius, rectHandle.top())
                                           , QPointF(rectHandle.right() - radius, rectHandle.top())
                                           , QPointF(rectHandle.right(), rectHandle.center().y())
                                           , QPointF(rectHandle.right() - radius, rectHandle.bottom())
                                           , QPointF(rectHandle.left() + radius, rectHandle.bottom())};
                    p->drawPolygon(polygon, 5);
                }
            }
            p->drawRoundedRect(rectRoundedPart, DStyle::pixelMetric(DStyle::PM_FrameRadius), DStyle::pixelMetric(DStyle::PM_FrameRadius));
        }
    }
}

void ChameleonStyle::drawIcon(const QStyleOption *opt, QPainter *p, QRect& rect, const QIcon& icon, bool checked) const
{
    bool enabled = opt->state & State_Enabled;
    bool selected = opt->state & State_Selected && enabled;
    QIcon::Mode mode = !enabled ? QIcon::Disabled : (selected ? QIcon::Selected : QIcon::Normal);

    icon.paint(p, rect, Qt::AlignCenter, mode, checked ? QIcon::On : QIcon::Off);
}

#ifndef QT_NO_ANIMATION
dstyle::DStyleAnimation *ChameleonStyle::animation(const QObject *target) const
{
    return animations.value(target);
}

void ChameleonStyle::startAnimation(dstyle::DStyleAnimation *animation) const
{
    connect(animation, SIGNAL(destroyed(QObject*)), SLOT(_q_removeAnimation()), Qt::UniqueConnection);

    animations.insert(animation->target(), animation);
    animation->start();
}

void ChameleonStyle::_q_removeAnimation()
{
    QObject* animation = sender();
    if (animation)
        animations.remove(animation->parent());
}
#endif

bool ChameleonStyle::drawMenuBarItem(const QStyleOptionMenuItem *option, QRect &rect, QPainter *painter, const QWidget *widget) const
{
    bool enabled(option->state & QStyle::State_Enabled);
    bool mouseOver((option->state & QStyle::State_MouseOver) && enabled);
    bool sunken((option->state & QStyle::State_Sunken) && enabled);

    if (mouseOver || sunken) {
        QBrush background(getColor(option, QPalette::Highlight));
        qreal radius = DStyle::pixelMetric(DStyle::PM_FrameRadius);

        painter->setRenderHint(QPainter::Antialiasing);
        painter->setPen(Qt::NoPen);
        painter->setBrush(background);
        painter->drawRoundedRect(rect, radius, radius);
    }

    if (option) {
        int alignment = Qt::AlignCenter | Qt::TextShowMnemonic | Qt::TextDontClip | Qt::TextSingleLine;

        if (!proxy()->styleHint(SH_UnderlineShortcut, option, widget))
            alignment |= Qt::TextHideMnemonic;

        int iconExtent = proxy()->pixelMetric(PM_SmallIconSize);
        QPixmap pix = option->icon.pixmap(widget ? widget->window()->windowHandle() : nullptr, QSize(iconExtent, iconExtent), (enabled) ? (mouseOver ? QIcon::Active : QIcon::Normal) : QIcon::Disabled);

        if (!pix.isNull()) {
            proxy()->drawItemPixmap(painter, option->rect, alignment, pix);
        } else {
            QStyleOptionMenuItem itemOption = *option;

            if (mouseOver || sunken)
                itemOption.palette.setBrush(QPalette::ButtonText, itemOption.palette.highlightedText());

            proxy()->drawItemText(painter, itemOption.rect, alignment, itemOption.palette, enabled,
                                  itemOption.text, QPalette::ButtonText);
        }
    }

    return true;
}

void ChameleonStyle::drawMenuItemBackground(const QStyleOption *option, QPainter *painter) const
{
    QBrush color;

    if ((option->state & QStyle::State_Enabled) && option->state & QStyle::State_Selected) {
        color = option->palette.highlight();
    } else {
        color = option->palette.background().color();

        if (color.color().isValid()) {
            QColor c = color.color();

            // 未开启窗口混成时不应该设置背景色的alpha通道，应当显示纯色背景
            if (DWindowManagerHelper::instance()->hasComposite()) {
                if (DGuiApplicationHelper::toColorType(c) == DGuiApplicationHelper::LightType) {
                    c = Qt::white;
                    c.setAlphaF(0.4);
                } else {
                    c = DGuiApplicationHelper::adjustColor(c, 0, 0, -10, 0, 0, 0, 0);
                    c.setAlphaF(0.8);
                }
            }

            color = c;
        }
    }

    painter->fillRect(option->rect, color);
}

bool ChameleonStyle::drawMenuItem(const QStyleOptionMenuItem *option, QPainter *painter, const QWidget *widget) const
{
    if (const QStyleOptionMenuItem *menuItem = option) {
        //绘制背景
        QRect menuRect = menuItem->rect;
        bool enabled = menuItem->state & State_Enabled;
        bool selected = menuItem->state & State_Selected && enabled;
        bool checkable = menuItem->checkType != QStyleOptionMenuItem::NotCheckable;
        bool checked = menuItem->checked;
        bool sunken = menuItem->state & State_Sunken;

        //绘制分段
        if (menuItem->menuItemType == QStyleOptionMenuItem::Separator) {
            if (!menuItem->text.isEmpty()) {
                painter->setFont(menuItem->font);
                painter->setPen(Qt::NoPen);
                painter->setBrush(Qt::NoBrush);
                proxy()->drawItemText(painter, menuRect
                                      , menuItem->direction == Qt::LeftToRight ? (Qt::AlignLeft | Qt::AlignVCenter) : (Qt::AlignRight | Qt::AlignVCenter)
                                      , menuItem->palette, menuItem->state & State_Enabled, menuItem->text
                                      , QPalette::WindowText);
            }

            return true;
        }

        //绘制背景
        drawMenuItemBackground(option, painter);

        //绘制选择框
        bool ignoreCheckMark = false;

        int frameRadius = DStyle::pixelMetric(PM_FrameRadius);  //打钩矩形的左侧距离item的左边缘； 也是 打钩矩形的右侧距离 图文内容的左边缘
        int smallIconSize = proxy()->pixelMetric(PM_SmallIconSize, option, widget);//打钩的宽度
        int realMargins = smallIconSize + 2 * frameRadius;  //左侧固定预留的margin，无论是否能够打钩都要预留

        if (!ignoreCheckMark) {
            /*checkRect = visualRect(menuItem->direction, menuItem->rect, checkRect);*/
            QRect checkRect(menuItem->rect);

            if (checkable) {
                checkRect.setLeft(frameRadius);
                checkRect.setWidth(smallIconSize);
                checkRect.setHeight(smallIconSize);
                checkRect.moveCenter(QPoint(checkRect.left() + smallIconSize / 2, menuItem->rect.center().y()));
                painter->setRenderHint(QPainter::Antialiasing);

                if (selected)
                    painter->setPen(getColor(option, QPalette::HighlightedText));
                else
                    painter->setPen(getColor(option, QPalette::BrightText));

                if (menuItem->checkType & QStyleOptionMenuItem::Exclusive) { //单选框
                    if (checked || sunken) {
                        QIcon markIcon = DStyle::standardIcon(SP_MarkElement, option, widget);
                        markIcon.paint(painter, checkRect);
                    }
                } else if (checked) { //复选框
                    QIcon markIcon = DStyle::standardIcon(SP_MarkElement, option, widget);
                    markIcon.paint(painter, checkRect);
                } else {
                }
            }
        } else { //ignore checkmark //用于combobox

        }

        if (selected)
            painter->setPen(getColor(option, QPalette::HighlightedText));
        else
            painter->setPen(getColor(option, QPalette::BrightText));

        QSize iconSize(0, 0);
        // 绘制图标
        if (!menuItem->icon.isNull()) {
            /*= visualRect(opt->direction, menuItem->rect,QRect(menuItem->rect.x() + checkColHOffset, menuItem->rect.y(),checkcol, menuitem->rect.height()));*/
            iconSize.setWidth(smallIconSize);
            iconSize.setHeight(smallIconSize);

#if QT_CONFIG(combobox)
            if (const QComboBox *combo = qobject_cast<const QComboBox *>(widget))
                iconSize = combo->iconSize();
#endif

            QRect pmr(menuRect.x() + realMargins, menuRect.center().y() - smallIconSize / 2, iconSize.width(), iconSize.height());
            drawIcon(option, painter, pmr, option->icon, checked);

        }

        // 绘制文本
        int x, y, w, h;
        menuRect.getRect(&x, &y, &w, &h);
        int tab = menuItem->tabWidth;
        int xpos = menuRect.x(); //1.只有文本  2.只有图片加文本  ，xpos为文本的起始坐标

        if (iconSize.width() > 0) {
            xpos += realMargins + smallIconSize + frameRadius;
        } else {
            xpos += realMargins;
        }

        QRect textRect(xpos, y + Menu_ItemHTextMargin, w - xpos - tab, h - 2 * Menu_ItemVTextMargin);
        QRect vTextRect = textRect /*visualRect(option->direction, menuRect, textRect)*/; // 区分左右方向
        QStringRef textRef(&menuItem->text);

        painter->setBrush(Qt::NoBrush);

        if (!textRef.isEmpty()) {
            int tabIndex = textRef.indexOf(QLatin1Char('\t'));
            int text_flags = Qt::AlignVCenter | Qt::TextShowMnemonic | Qt::TextDontClip | Qt::TextSingleLine;

            if (!styleHint(SH_UnderlineShortcut, menuItem, widget))
                text_flags |= Qt::TextHideMnemonic;

            text_flags |= Qt::AlignLeft;

            if (tabIndex >= 0) {
                QPoint vShortcutStartPoint = textRect.topRight();  //快捷键设置显示
                vShortcutStartPoint.setX(vShortcutStartPoint.x() - Menu_PanelRightPadding - realMargins);
                QRect vShortcutRect = QRect(vShortcutStartPoint, QPoint(menuRect.right(), textRect.bottom()));
                /* = visualRect(option->direction,menuRect,QRect(vShortcutStartPoint, QPoint(menuRect.right(), textRect.bottom())))*/;
                const QString textToDraw = textRef.mid(tabIndex + 1).toString();

                painter->drawText(vShortcutRect, text_flags, textToDraw);
                textRef = textRef.left(tabIndex);
            }

            QFont font = menuItem->font;
            font.setPointSizeF(QFontInfo(menuItem->font).pointSizeF());
            painter->setFont(font);
            const QString textToDraw = textRef.left(tabIndex).toString();

            painter->setBrush(Qt::NoBrush);
            painter->drawText(vTextRect, text_flags, textToDraw);
        }

        // 绘制箭头
        if (menuItem->menuItemType == QStyleOptionMenuItem::SubMenu) {// draw sub menu arrow
            int dim = (menuRect.height() - 4) / 2;
            int xpos = menuRect.left() + menuRect.width() - 3 - dim;
            QStyleOptionMenuItem newMI = *menuItem;
            xpos += realMargins + iconSize.width() + frameRadius;
            QPoint topLeft(menuItem->rect.right() - frameRadius - smallIconSize / 2, menuItem->rect.center().y() - smallIconSize / 3);  //箭头rect: Size(smallIconSize, smallIconSize*2/3)
            QPoint RightButtom(topLeft.x() + smallIconSize / 2, menuItem->rect.center().y() + smallIconSize / 3 );
            QRect rectArrow(topLeft, RightButtom);
            newMI.rect = rectArrow;

            newMI.state = !enabled ? State_None : State_Enabled;
            if (selected)
                newMI.palette.setColor(QPalette::Foreground,
                                       newMI.palette.highlightedText().color());

            QIcon markIcon = DStyle::standardIcon(SP_ArrowEnter, &newMI, widget);
            markIcon.paint(painter, newMI.rect);
        }
    }

    return true;
}

QRect ChameleonStyle::subElementRect(QStyle::SubElement r, const QStyleOption *opt,
                                     const QWidget *widget) const
{
    switch (r) {
    case SE_HeaderArrow:{
        QRect rect;
        int h = opt->rect.height();
        int w = opt->rect.width();
        int x = opt->rect.x();
        int y = opt->rect.y();
        int margin = proxy()->pixelMetric(QStyle::PM_HeaderMargin, opt, widget);

        if (opt->state & State_Horizontal) {
            // designer: whatever how big the QHeaderView it is, the arrow size is fixed.
            int horiz_size = 8;
            int vert_size = 5;
            rect.setRect(x + w - margin * 2 - horiz_size, y + (h - vert_size) / 2,
                      horiz_size, vert_size);
        } else {
            int horiz_size = 5;
            int vert_size = 8;
            rect.setRect(x + (w - horiz_size) / 2, y + h - margin * 2 - vert_size,
                      horiz_size, vert_size);
        }
        rect = visualRect(opt->direction, opt->rect, rect);
        return rect;
    }
    case SE_PushButtonFocusRect:
    case SE_ItemViewItemFocusRect:
        return opt->rect;
    case SE_ItemViewItemCheckIndicator: Q_FALLTHROUGH();
    case SE_ItemViewItemDecoration: Q_FALLTHROUGH();
    case SE_ItemViewItemText:
        if (const QStyleOptionViewItem *vopt = qstyleoption_cast<const QStyleOptionViewItem *>(opt)) {
            int frame_radius = DStyle::pixelMetric(PM_FrameRadius, opt, widget);
            QStyleOptionViewItem option(*vopt);
            option.rect = opt->rect.adjusted(frame_radius, 0, -frame_radius, 0);

            // 默认把checkbox放置在右边，因此使用QCommonStyle的Item布局时先移除HasCheckIndicator标志
            option.features &= ~QStyleOptionViewItem::HasCheckIndicator;

            if (r == SE_ItemViewItemDecoration) {
                return DStyle::subElementRect(r, &option, widget);
            }

            QRect text_rect = DStyle::subElementRect(SE_ItemViewItemText, &option, widget);
            int indicator_width = proxy()->pixelMetric(PM_IndicatorWidth, &option, widget);
            int indicator_height = proxy()->pixelMetric(PM_IndicatorHeight, &option, widget);

            const QRect indicator_rect = alignedRect(opt->direction, Qt::AlignRight | Qt::AlignVCenter,
                                                     QSize(indicator_width, indicator_height), text_rect);

            int margin = proxy()->pixelMetric(QStyle::PM_FocusFrameHMargin, opt, widget);
            text_rect.setRight(qMin(text_rect.right(), indicator_rect.left() - margin));

            return r == SE_ItemViewItemText ? text_rect : indicator_rect;
        }
        break;
    case SE_LineEditContents: {
        int frame_margins = DStyle::pixelMetric(PM_FrameMargins, opt, widget);
        int left_margins = DStyle::pixelMetric(PM_ContentsMargins, opt, widget);
        return opt->rect.adjusted(frame_margins + left_margins, 0, -(frame_margins + left_margins), 0);
    }
    case SE_RadioButtonFocusRect:
    case SE_CheckBoxFocusRect: {
        QRect re;
        re = subElementRect(SE_CheckBoxIndicator, opt, widget);
        int margin = DStyle::pixelMetric(PM_FocusBorderWidth) + DStyle::pixelMetric(PM_FocusBorderSpacing);
        re.adjust(-margin, -margin, margin, margin);
        return re;
    }
    case SE_RadioButtonIndicator:
    case SE_RadioButtonContents:
    case SE_RadioButtonClickRect:
    case SE_CheckBoxContents:
    case SE_CheckBoxClickRect:
    case SE_CheckBoxIndicator:
        if (const QStyleOptionButton *vopt = qstyleoption_cast<const QStyleOptionButton *>(opt)) {
            QStyleOptionButton option(*vopt);
            int margin = DStyle::pixelMetric(PM_FocusBorderWidth) + DStyle::pixelMetric(PM_FocusBorderSpacing); //来自SE_CheckBoxFocusRect状态时
            option.rect.translate(margin, 0);   //需往右偏margin数值，FocusRect框显示正常；故对应其size的width也增加margin
            return DStyle::subElementRect(r, &option, widget);
        }
        break;
    case SE_PushButtonContents:
        if (const QStyleOptionButton *vopt = qstyleoption_cast<const QStyleOptionButton *>(opt)) {
            QRect buttonContentRect = vopt->rect;
            int buttonIconMargin = proxy()->pixelMetric(QStyle::PM_ButtonMargin, opt, widget) ;
            buttonContentRect.adjust(buttonIconMargin, 0, -buttonIconMargin, 0);

            return buttonContentRect;
        }
        break;
    case SE_TabBarScrollLeftButton: {
        const bool vertical = opt->rect.width() < opt->rect.height();
        const int buttonWidth = proxy()->pixelMetric(PM_TabBarScrollButtonWidth, opt, widget);
        QRect buttonRect = vertical ? QRect(0, 0, opt->rect.width(), buttonWidth)
                           : QRect(0, 0, buttonWidth, opt->rect.height());

        return buttonRect;
    }
    case SE_TabBarScrollRightButton: {
        const bool vertical = opt->rect.width() < opt->rect.height();
        const int buttonWidth = proxy()->pixelMetric(PM_TabBarScrollButtonWidth, opt, widget);
        QRect buttonRect = vertical ? QRect(0, opt->rect.height() - buttonWidth, opt->rect.width(), buttonWidth)
                           : QRect(opt->rect.width() - buttonWidth, 0, buttonWidth, opt->rect.height());

        return buttonRect;
    }
    case SE_ProgressBarLabel: {
        int radius = DStyle::pixelMetric(PM_FrameRadius);
        return opt->rect.marginsRemoved(QMargins(radius, radius, radius, radius));
    }
    default:
        break;
    }

    return DStyle::subElementRect(r, opt, widget);
}

void ChameleonStyle::drawComplexControl(QStyle::ComplexControl cc, const QStyleOptionComplex *opt,
                                        QPainter *p, const QWidget *w) const
{
    switch (cc) {
    case CC_SpinBox: {
        if (const QStyleOptionSpinBox *option = qstyleoption_cast<const QStyleOptionSpinBox *>(opt)) {
            if (drawSpinBox(option, p, w))
                return;
        }
        break;
    }
    case CC_ToolButton: {
        if (const QStyleOptionToolButton *toolbutton
            = qstyleoption_cast<const QStyleOptionToolButton *>(opt)) {
            QRect button, menuarea;
            button = proxy()->subControlRect(cc, toolbutton, SC_ToolButton, w);
            menuarea = proxy()->subControlRect(cc, toolbutton, SC_ToolButtonMenu, w);

            State bflags = toolbutton->state & ~State_Sunken;

            if (bflags & State_AutoRaise) {
                if (!(bflags & State_MouseOver) || !(bflags & State_Enabled)) {
                    bflags &= ~State_Raised;
                }
            }
            State mflags = bflags;
            if (toolbutton->state & State_Sunken) {
                if (toolbutton->activeSubControls & SC_ToolButton)
                    bflags |= State_Sunken;
                mflags |= State_Sunken;
            }

            QStyleOption tool = *toolbutton;
            if (toolbutton->subControls & SC_ToolButton) {
                if (bflags & (State_Sunken | State_On | State_Raised)) {
                    tool.rect = button;
                    tool.state = bflags;
                    //proxy()->drawPrimitive(PE_PanelButtonTool, &tool, p, w);
                }
            }

            if (toolbutton->state & State_HasFocus) {
                QStyleOptionFocusRect fr;
                fr.QStyleOption::operator=(*toolbutton);
                fr.rect.adjust(3, 3, -3, -3);
                if (toolbutton->features & QStyleOptionToolButton::MenuButtonPopup)
                    fr.rect.adjust(0, 0, -proxy()->pixelMetric(QStyle::PM_MenuButtonIndicator,
                                                      toolbutton, w), 0);
                proxy()->drawPrimitive(PE_FrameFocusRect, &fr, p, w);
            }
            QStyleOptionToolButton label = *toolbutton;
            label.state = bflags;
            int fw = proxy()->pixelMetric(PM_DefaultFrameWidth, opt, w);
            label.rect = button.adjusted(fw, fw, -fw, -fw);

            if (w->property("_d_calendarToolBtn").toBool()) {
                label.palette.setColor(QPalette::ButtonText, QPalette::Highlight);
            }

            proxy()->drawControl(CE_ToolButtonLabel, &label, p, w);

            if (toolbutton->subControls & SC_ToolButtonMenu) {
                tool.rect = menuarea;
                tool.state = mflags;
                if (mflags & (State_Sunken | State_On | State_Raised))
                    proxy()->drawPrimitive(PE_IndicatorButtonDropDown, &tool, p, w);
                proxy()->drawPrimitive(PE_IndicatorArrowDown, &tool, p, w);
            } else if (toolbutton->features & QStyleOptionToolButton::HasMenu) {
                int mbi = proxy()->pixelMetric(PM_MenuButtonIndicator, toolbutton, w);
                QRect ir = toolbutton->rect;
                QStyleOptionToolButton newBtn = *toolbutton;
                newBtn.rect = QRect(ir.right() + 5 - mbi, ir.y() + ir.height() - mbi + 4, mbi - 6, mbi - 6);
                newBtn.rect = visualRect(toolbutton->direction, button, newBtn.rect);
                proxy()->drawPrimitive(PE_IndicatorArrowDown, &newBtn, p, w);
            }
        }
        return;
    }
    case CC_Slider : {
        if (const QStyleOptionSlider *slider = qstyleoption_cast<const QStyleOptionSlider *>(opt)) {
            //各个使用的矩形大小和位置
            QRectF rect = opt->rect;                                                                            //Slider控件最大的矩形(包含如下三个)
            QRectF rectHandle = proxy()->subControlRect(CC_Slider, opt, SC_SliderHandle, w);                    //滑块矩形
            QRectF rectSliderTickmarks = proxy()->subControlRect(CC_Slider, opt, SC_SliderTickmarks, w);        //刻度的矩形
            QRect rectGroove = proxy()->subControlRect(CC_Slider, opt, SC_SliderGroove, w);                     //滑槽的矩形

//            //测试(保留不删)
//            p->fillRect(rect, Qt::gray);
//            p->fillRect(rectSliderTickmarks, Qt::blue);
//            p->fillRect(rectGroove, Qt::red);
//            p->fillRect(rectHandle, Qt::green);
//            qDebug()<<"---rect:"<<rect<<"  rectHandle:"<<rectHandle<<"   rectSliderTickmarks:"<<rectSliderTickmarks<<"   rectGroove:"<<rectGroove;

            QPen pen;
            //绘画 滑槽(线)
            if (opt->subControls & SC_SliderGroove) {
                pen.setStyle(Qt::CustomDashLine);
                QVector<qreal> dashes;
                qreal space = 1.3;
                dashes << 0.1 << space;
                pen.setDashPattern(dashes);
                pen.setWidthF(3);
                pen.setBrush((opt->activeSubControls & SC_SliderHandle) ? getColor(opt, QPalette::Highlight) : opt->palette.highlight());
                p->setPen(pen);
                p->setRenderHint(QPainter::Antialiasing);

                //pen.color 默认进来就是高亮色Highlight
                if (!isNoticks(slider, p, w)) {
                    pen.setColor(getColor(opt, QPalette::Foreground));
                    p->setPen(pen);
                }

                QColor color = getColor(opt, QPalette::Foreground);  //绘画的右侧/上侧的滑槽颜色一定是灰

                if (slider->orientation == Qt::Horizontal) {
                    p->drawLine(QPointF(rectGroove.left(), rectHandle.center().y()), QPointF(rectHandle.left(), rectHandle.center().y()));

                    pen.setColor(color);
                    p->setPen(pen);
                    p->drawLine(QPointF(rectGroove.right(), rectHandle.center().y()), QPointF(rectHandle.right(), rectHandle.center().y()));
                } else {
                    p->drawLine(QPointF(rectGroove.center().x(), rectGroove.bottom()), QPointF(rectGroove.center().x(),  rectHandle.bottom()));

                    pen.setColor(color);
                    p->setPen(pen);
                    p->drawLine(QPointF(rectGroove.center().x(),  rectGroove.top()), QPointF(rectGroove.center().x(),  rectHandle.top()));
                }
            }

            //绘画 滑块
            if (opt->subControls & SC_SliderHandle) {
                pen.setStyle(Qt::SolidLine);
                p->setPen(Qt::NoPen);
                p->setBrush((opt->activeSubControls & SC_SliderHandle) ? getColor(opt, QPalette::Highlight) : opt->palette.highlight());
                drawSliderHandle(opt, rectHandle, p, w);
            }

            //绘画 刻度,绘画方式了参考qfusionstyle.cpp
            if ((opt->subControls & SC_SliderTickmarks) && slider->tickInterval) {                                   //需要绘画刻度
                p->setPen(opt->palette.foreground().color());
                int available = proxy()->pixelMetric(PM_SliderSpaceAvailable, slider, w);  //可用空间
                int interval = slider->tickInterval;                                       //标记间隔
//                int tickSize = proxy()->pixelMetric(PM_SliderTickmarkOffset, opt, w);      //标记偏移
//                int ticks = slider->tickPosition;                                          //标记位置

                int v = slider->minimum;
                int len = proxy()->pixelMetric(PM_SliderLength, slider, w);
                while (v <= slider->maximum + 1) {                                          //此处不添加+1的话, 会少绘画一根线
                    const int v_ = qMin(v, slider->maximum);
                    int pos = sliderPositionFromValue(slider->minimum, slider->maximum, v_, available) + len / 2;

                    if (slider->orientation == Qt::Horizontal) {
                        if (slider->tickPosition == QSlider::TicksBothSides) {              //两侧都会绘画, 总的矩形-中心滑槽滑块最小公共矩形
                            p->drawLine(pos, rect.top(), pos, rectHandle.top());
                            p->drawLine(pos, rect.bottom(), pos, rectHandle.bottom());
                        } else {
                            p->drawLine(pos, rectSliderTickmarks.top(), pos, rectSliderTickmarks.bottom());
                        }
                    } else {
                        if (slider->tickPosition == QSlider::TicksBothSides) {
                            p->drawLine(rect.left(), pos, rectHandle.left(), pos);
                            p->drawLine(rect.right(), pos, rectHandle.right(), pos);
                        } else {
                            p->drawLine(rectSliderTickmarks.left(), pos, rectSliderTickmarks.right(), pos);
                        }
                    }
                    // in the case where maximum is max int
                    int nextInterval = v + interval;
                    if (nextInterval < v)
                        break;
                    v = nextInterval;
                }
            }

        }
        break;
    }
    case CC_ComboBox: {
        if (const QStyleOptionComboBox *comboBox = qstyleoption_cast<const QStyleOptionComboBox *>(opt)) {
            if (drawComboBox(p, comboBox, w))
                return;
        }
        break;
    }
    default:
        break;
    }

    DStyle::drawComplexControl(cc, opt, p, w);
}

bool ChameleonStyle::drawSpinBox(const QStyleOptionSpinBox *opt,
                                 QPainter *painter, const QWidget *widget) const
{
    qreal borderRadius = DStyle::pixelMetric(DStyle::PM_FrameRadius);

    if (opt->frame && (opt->subControls & SC_SpinBoxFrame)) {
        QRect frameRect = proxy()->subControlRect(CC_SpinBox, opt, SC_SpinBoxFrame, widget);
        painter->setRenderHint(QPainter::Antialiasing, true);
        painter->setPen(Qt::NoPen);
        painter->setBrush(opt->palette.button());
        painter->drawRoundedRect(frameRect, borderRadius, borderRadius);
    }

    if (opt->subControls & SC_SpinBoxUp) {
        bool upIsActive = opt->activeSubControls == SC_SpinBoxUp;
        bool upIsEnabled = opt->stepEnabled & QAbstractSpinBox::StepUpEnabled && opt->state.testFlag(State_Enabled);
        QRect subRect = proxy()->subControlRect(CC_SpinBox, opt, SC_SpinBoxUp, widget);
        QStyleOptionButton buttonOpt;
        buttonOpt.rect = subRect;
        updateSpinBoxButtonState(opt, buttonOpt, upIsActive, upIsEnabled);
        bool isPlusMinus = opt->buttonSymbols & QAbstractSpinBox::PlusMinus;
        proxy()->drawControl(CE_PushButton, &buttonOpt, painter, widget);

        if (isPlusMinus) {
            buttonOpt.rect -= frameExtentMargins();
            QRectF plusRect = proxy()->subElementRect(SE_PushButtonContents, &buttonOpt, widget);
            QRectF cent_rect(0, 0, plusRect.width() / 2, plusRect.width() / 2);
            cent_rect.moveCenter(plusRect.center());
            DDrawUtils::drawPlus(painter, cent_rect, getColor(opt, QPalette::ButtonText), 1);
        } else {
            QRect arrowRect = subRect;
            arrowRect.setWidth(arrowRect.width() / 3);
            arrowRect.setHeight(arrowRect.width() / 2);
            arrowRect.moveCenter(subRect.center());
            DDrawUtils::drawArrow(painter, arrowRect, getColor(opt, QPalette::ButtonText), Qt::ArrowType::UpArrow, 1);
        }
    }

    if (opt->subControls & SC_SpinBoxDown) {
        bool downIsActive = opt->activeSubControls == SC_SpinBoxDown;
        bool downIsEnabled = opt->stepEnabled & QAbstractSpinBox::StepDownEnabled && opt->state.testFlag(State_Enabled);
        QRect subRect = proxy()->subControlRect(CC_SpinBox, opt, SC_SpinBoxDown, widget);
        QStyleOptionButton buttonOpt;
        buttonOpt.rect = subRect;
        updateSpinBoxButtonState(opt, buttonOpt, downIsActive, downIsEnabled);
        bool isPlusMinus = opt->buttonSymbols & QAbstractSpinBox::PlusMinus;
        proxy()->drawControl(CE_PushButton, &buttonOpt, painter, widget);

        if (isPlusMinus) {
            buttonOpt.rect -= frameExtentMargins();
            QRectF subtractRect = proxy()->subElementRect(SE_PushButtonContents, &buttonOpt, widget);
            QRectF cent_rect(0, 0, subtractRect.width() / 2, subtractRect.height() / 2);
            cent_rect.moveCenter(subtractRect.center());
            DDrawUtils::drawSubtract(painter, cent_rect, getColor(opt, QPalette::ButtonText), 1);
        } else {
            QRect arrowRect = subRect;
            arrowRect.setWidth(arrowRect.width() / 3);
            arrowRect.setHeight(arrowRect.width() / 2);
            arrowRect.moveCenter(subRect.center());
            DDrawUtils::drawArrow(painter, arrowRect, getColor(opt, QPalette::ButtonText), Qt::ArrowType::DownArrow, 1);
        }
    }

    return true;
}

void ChameleonStyle::updateSpinBoxButtonState(const QStyleOptionSpinBox *opt, QStyleOptionButton &buttonOpt, bool isActive, bool isEnabled) const
{
    State buttonState = opt->state;

    if (!isActive) {
        buttonState &= ~State_MouseOver;
        buttonState &= ~State_Sunken;
        buttonState &= ~State_Active;
        buttonState &= ~State_HasFocus;
    }

    if (!isEnabled) {
        buttonState &= ~State_Enabled;
        buttonState &= ~State_MouseOver;
        buttonState &= ~State_Sunken;
        buttonState &= ~State_On;
    }

    buttonOpt.state = buttonState;
}

QStyle::SubControl ChameleonStyle::hitTestComplexControl(QStyle::ComplexControl cc, const QStyleOptionComplex *opt,
                                                         const QPoint &pt, const QWidget *w) const
{
    return DStyle::hitTestComplexControl(cc, opt, pt, w);
}

QRect ChameleonStyle::subControlRect(QStyle::ComplexControl cc, const QStyleOptionComplex *opt,
                                     QStyle::SubControl sc, const QWidget *w) const
{
    switch (cc) {
    case CC_SpinBox: {
        if (const QStyleOptionSpinBox *option = qstyleoption_cast<const QStyleOptionSpinBox *>(opt)) {

            switch (sc) {
            case SC_SpinBoxEditField: {
                if (option->buttonSymbols == QAbstractSpinBox::NoButtons)
                    return proxy()->subControlRect(CC_SpinBox, opt, SC_SpinBoxFrame, w);

                int rightBorder = option->frame ? proxy()->pixelMetric(PM_SpinBoxFrameWidth, opt, w) * 2 : 0;
                QRect spinboxRect = option->rect;
                QRect dButtonRect = proxy()->subControlRect(CC_SpinBox, opt, SC_SpinBoxDown, w);
                spinboxRect.setRight(dButtonRect.left() - rightBorder - frameExtentMargins().left());
                return spinboxRect;
            }
            case SC_SpinBoxUp: {
                if (option->buttonSymbols == QAbstractSpinBox::NoButtons)
                    return QRect();

                int rectHeight = option->rect.height();
                rectHeight -= (option->frame ? proxy()->pixelMetric(PM_SpinBoxFrameWidth, opt, w) * 2 : 0);

                if (opt->rect.width() < rectHeight * 3)
                    break;

                QRect buttonRect(option->rect.topLeft(), QSize(rectHeight, rectHeight)); //按高度计算
                buttonRect.moveRight(option->rect.right());
                return buttonRect.marginsRemoved(frameExtentMargins());
            }
            case SC_SpinBoxDown: {
                if (option->buttonSymbols == QAbstractSpinBox::NoButtons)
                    return QRect();

                int rectHeight = option->rect.height();
                rectHeight -= (option->frame ? proxy()->pixelMetric(PM_SpinBoxFrameWidth, opt, w) * 2 : 0);

                if (opt->rect.width() < rectHeight * 3)
                    break;

                QRect uButtonRect = proxy()->subControlRect(CC_SpinBox, opt, SC_SpinBoxUp, w);
                QRect buttonRect = option->rect;
                buttonRect.setLeft(uButtonRect.left() - rectHeight);
                buttonRect.setRight(uButtonRect.left());
                buttonRect.setSize(QSize(rectHeight, rectHeight));
                return buttonRect.marginsRemoved(frameExtentMargins());
            }
            case SC_SpinBoxFrame: {
                return option->rect - frameExtentMargins() ;
            }
            default:
                break;
            }
        }
        break;
    }
    case CC_Slider: {
        if (const QStyleOptionSlider *option = qstyleoption_cast<const QStyleOptionSlider *>(opt)) {
            QRectF rect = option->rect;                                                    //Slider控件总的大小矩形
            int slider_size = proxy()->pixelMetric(PM_SliderControlThickness, opt, w);     //滑块的高度
//            int tick_size = proxy()->pixelMetric(PM_SliderTickmarkOffset, opt, w);         //刻度的高度
            QRectF slider_handle_rect = rect;                                              //滑块和滑漕的的最小公共矩形 (后面被用作临时且被改变的)

            if (option->orientation == Qt::Horizontal) {
                slider_handle_rect.setHeight(slider_size);
                if (option->tickPosition == QSlider::TicksAbove) slider_handle_rect.moveBottom(rect.bottom());
                if (option->tickPosition == QSlider::TicksBelow) slider_handle_rect.moveTop(rect.top());
                if (option->tickPosition == QSlider::TicksBothSides) slider_handle_rect.moveCenter(rect.center());
            } else {
                slider_handle_rect.setWidth(slider_size);
                if (option->tickPosition == QSlider::TicksRight)  slider_handle_rect.moveLeft(rect.left());
                if (option->tickPosition == QSlider::TicksLeft)   slider_handle_rect.moveRight(rect.right());
                if (option->tickPosition == QSlider::TicksBothSides) slider_handle_rect.moveCenter(rect.center());
            }

            QRectF rectStatic =  slider_handle_rect;   //rectStatic作为 滑块和滑漕的的最小公共矩形(不改变)

            switch (sc) {
            case SC_SliderGroove: {  //滑漕
                qreal groove_size = slider_size / 4.0;
                QRectF groove_rect;

                if (option->orientation == Qt::Horizontal) {
                    groove_rect.setWidth(slider_handle_rect.width());
                    groove_rect.setHeight(groove_size);
                } else {
                    groove_rect.setWidth(groove_size);
                    groove_rect.setHeight(slider_handle_rect.height());
                }

                groove_rect.moveCenter(slider_handle_rect.center());
                return groove_rect.toRect();
            }
            case SC_SliderHandle: {  //滑块
                int sliderPos = 0;
                int len = proxy()->pixelMetric(PM_SliderLength, option, w);
                bool horizontal = option->orientation == Qt::Horizontal;
                sliderPos = sliderPositionFromValue(option->minimum, option->maximum, option->sliderPosition,
                                                    (horizontal ? slider_handle_rect.width() : slider_handle_rect.height()) - len, option->upsideDown);
                if (horizontal) {
                    slider_handle_rect.moveLeft(slider_handle_rect.left() + sliderPos);
                    slider_handle_rect.setWidth(len);
                    slider_handle_rect.moveTop(rectStatic.top());
                } else {
                    slider_handle_rect.moveTop(slider_handle_rect.top() + sliderPos);
                    slider_handle_rect.setHeight(len);
                    slider_handle_rect.moveLeft(rectStatic.left());
                }

                return slider_handle_rect.toRect();
            }
            case SC_SliderTickmarks: {  //刻度的矩形
                if (option->tickPosition & QSlider::NoTicks)
                    return QRect(0, 0, 0, 0);

                QRectF tick_rect = rect;

                if (option->orientation == Qt::Horizontal) {
                    tick_rect.setHeight(rect.height() - slider_handle_rect.height());

                    if (option->tickPosition == QSlider::TicksAbove) {
                        tick_rect.moveTop(rect.top());
                    } else if (option->tickPosition == QSlider::TicksBelow) {
                        tick_rect.moveBottom(rect.bottom());
                    }
                } else {
                    tick_rect.setWidth(rect.width() - slider_handle_rect.width());

                    if (option->tickPosition == QSlider::TicksLeft) {
                        tick_rect.moveLeft(rect.left());
                    } else if (option->tickPosition == QSlider::TicksRight) {
                        tick_rect.moveRight(rect.right());
                    }
                }

                return tick_rect.toRect();
            }
            default:
                break;
            }
        }
        break;
    }
    case CC_ComboBox: {
        if (qstyleoption_cast<const QStyleOptionComboBox *>(opt)) {
            DStyleHelper dstyle(proxy());
            int frameMargins = dstyle.pixelMetric(PM_FrameMargins, opt, w);

            switch (sc) {
            case SC_ComboBoxArrow: {
                QRect rect(0, 0, opt->rect.height(), opt->rect.height()) ;
                int boxHeight = qAbs(rect.height());

                if (w && !static_cast<const QComboBox *>(w)->isEditable())
                    break;
                if (opt->direction == Qt::LeftToRight)
                    rect.moveRight(opt->rect.right());
                else
                    rect.moveLeft(opt->rect.left());

                int buttonRectSize = boxHeight;
                rect.setSize(QSize(buttonRectSize, buttonRectSize));

                return rect;
            }
            case SC_ComboBoxEditField: {
                QRect rect = opt->rect;
                const QRect arrow_rect = proxy()->subControlRect(cc, opt, SC_ComboBoxArrow, w);

                if (opt->direction == Qt::LeftToRight) {
                    rect.setRight(arrow_rect.left());
                    rect.adjust(frameMargins, 0, 0, 0);
                } else {
                    rect.setLeft(arrow_rect.right());
                    rect.adjust(0, 0, -frameMargins, 0);
                }

                return rect;
            }
            case SC_ComboBoxFrame: {
                return opt->rect;
            }
            case SC_ComboBoxListBoxPopup: {
                QSize size = proxy()->sizeFromContents(CT_Menu, opt, opt->rect.size(), w);
                QRect rect = opt->rect;

                if (auto styopt = static_cast<const QStyleOption *>(opt)) {
                    if (auto menuItem = static_cast<const QStyleOptionMenuItem *>(styopt)) {
                        if (menuItem->icon.isNull()) {
                            rect.setWidth(size.width() + Menu_CheckMarkWidth);
                        }
                    }
                }
                return rect;
            }
            default:
                break;
            }
        }
        break;
    }
    default:
        break;
    }
    return DStyle::subControlRect(cc, opt, sc, w);
}

QSize ChameleonStyle::sizeFromContents(QStyle::ContentsType ct, const QStyleOption *opt,
                                       const QSize &contentsSize, const QWidget *widget) const
{
    QSize size = DStyle::sizeFromContents(ct, opt, contentsSize, widget);

    switch (ct) {
    case CT_LineEdit: {
        int button_margin = proxy()->pixelMetric(QStyle::PM_ButtonMargin, opt, widget);
        size += QSize(button_margin, button_margin);
        Q_FALLTHROUGH();
    }
    case CT_ComboBox: {
        if (auto com = qobject_cast<const QComboBox *>(widget)) {
            if (com->isEditable()) {
                //这是从lineedit设置margin处拿来
                int frame_margins = DStyle::pixelMetric(PM_FrameMargins, opt, widget);
                int left_margins = DStyle::pixelMetric(PM_ContentsMargins, opt, widget);
                size.setWidth(size.width() + frame_margins + left_margins);
            }
        }
        Q_FALLTHROUGH();
    }
    case CT_PushButton: {
        int frame_margins = DStyle::pixelMetric(PM_FrameMargins, opt, widget);
        size += QSize(frame_margins* 2, frame_margins * 2);

        if (const QStyleOptionButton *bopt = qstyleoption_cast<const QStyleOptionButton*>(opt)) {
            int frame_radius = DStyle::pixelMetric(PM_FrameRadius, opt, widget);

            // 为文本添加额外的margin
            if (!bopt->text.isEmpty()) {
                size.rwidth() += 2 * frame_radius;
            }

            if (bopt->features & QStyleOptionButton::HasMenu)
                size.rwidth() += frame_margins;   //qt源码会在带有menu的btn样式中,添加一个箭头矩形的width
        }

        int button_min_size = DStyle::pixelMetric(PM_ButtonMinimizedSize, opt, widget);
        size = size.expandedTo(QSize(button_min_size, button_min_size));
        break;
    }
    case CT_ItemViewItem: {
        if (const QStyleOptionViewItem *vopt = qstyleoption_cast<const QStyleOptionViewItem *>(opt)) {
            const QMargins &item_margins = qvariant_cast<QMargins>(vopt->index.data(Dtk::MarginsRole));

            if (!item_margins.isNull()) {
                //加上Item自定义的margins
                size = QRect(QPoint(0, 0), size).marginsAdded(item_margins).size();
            }

            return size;
        }
        break;
    }
    case CT_Slider: {
        if (const QStyleOptionSlider *slider = qstyleoption_cast<const QStyleOptionSlider *>(opt)) {
            /*2019-09-19　约定枚举值含义　　　　　中文含义
             * PM_SliderThickness:　　　　　　Slider总的高度　＝　滑块高度＋刻度高度
             * PM_SliderControlThickness:   只是滑块的单独高度
             * PM_SliderLength:             只是滑块的长度
             * PM_SliderTickmarkOffset:     用作slider的刻度线的高度
             * PM_SliderSpaceAvailable      暂时未用到
             */

            int sliderContHeight = proxy()->pixelMetric(PM_SliderControlThickness, opt, widget);
            int tickMarkHeight = proxy()->pixelMetric(PM_SliderTickmarkOffset, opt, widget);
            sliderContHeight += tickMarkHeight;

            if (slider->tickPosition == QSlider::NoTicks) {
                sliderContHeight -= tickMarkHeight;
            } else if (slider->tickPosition == QSlider::TicksBothSides) {
                sliderContHeight += tickMarkHeight;
            } else {
            }

            if (slider->orientation == Qt::Horizontal){
                size.setHeight(qMax(size.height(), sliderContHeight));
            } else {
                size.setWidth(qMax(size.width(), sliderContHeight));
            }
        }
        break;
    }
    case CT_MenuBarItem: {
        int frame_margins = DStyle::pixelMetric(PM_FrameMargins, opt, widget);
        size += QSize(frame_margins * 2, frame_margins * 2);
        break;
    }
    case CT_MenuItem: {
        if (const QStyleOptionMenuItem *menuItem = qstyleoption_cast<const QStyleOptionMenuItem *>(opt)) {
            int m_width = size.width();
            bool hideShortcutText = false;

            if (hideShortcutText) {
                m_width -= menuItem->tabWidth;
                int tabIndex = menuItem->text.indexOf(QLatin1Char('\t'));

                if (tabIndex != -1) {
                    int textWidth = menuItem->fontMetrics.width(menuItem->text.mid(tabIndex + 1));

                    if (menuItem->tabWidth == 0)
                        m_width -= textWidth;
                }
            }

            int tabSpacing = MenuItem_TabSpacing;
            if (menuItem->text.contains(QLatin1Char('\t'))) {  //若是项有快捷键，文本内容会以'\t'连接快捷键文本
                if (!hideShortcutText)
                    m_width += tabSpacing;
            } else {
                if (menuItem->menuItemType == QStyleOptionMenuItem::SubMenu) {
                    m_width += 2 * Menu_ArrowHMargin;
                }
            }

            int frameRadius = DStyle::pixelMetric(PM_FrameRadius);  //打钩矩形的左侧距离item的左边缘； 也是 打钩矩形的右侧距离 图文内容的左边缘
            int smallIconSize = proxy()->pixelMetric(PM_SmallIconSize, opt, widget);//打钩的宽度
            int realMargins = smallIconSize + 2 * frameRadius;  //左侧固定预留的margin，无论是否能够打钩都要预留

            m_width = realMargins;
            int textWidth = opt->fontMetrics.size(Qt::TextSingleLine, menuItem->text).width();

            if (!menuItem->text.isEmpty())
                m_width += (textWidth + frameRadius);

            if (!menuItem->icon.isNull())
                m_width += (smallIconSize + + frameRadius);

            m_width += (smallIconSize + frameRadius);
            size.setWidth(m_width);

            if (menuItem->menuItemType == QStyleOptionMenuItem::Separator) {
                if (!menuItem->text.isEmpty()) {
                    size.setHeight(menuItem->fontMetrics.height());
                } else {
                    size.setHeight(2);
                }
            } else if (!menuItem->icon.isNull()) {
                if (const QComboBox *combo = qobject_cast<const QComboBox *>(widget)) {
                    size.setHeight(qMax(combo->iconSize().height() + 2, size.height()));
                }
            }
        }

        size.setWidth(qMax(162, size.width()));
        size.setHeight(size.height() + qMax(Menu_ItemVMargin * 2, 0));
        break;
    }
    case CT_ScrollBar: {
        if (size.width() > size.height())
            size.setHeight(size.height() / 2.0);
        if (size.width() < size.height())
            size.setWidth(size.width() / 2.0);
        return size;
    }
    case CT_RadioButton:
    case CT_CheckBox: {
        size.rwidth() += 2 * (DStyle::pixelMetric(PM_FocusBorderWidth) + DStyle::pixelMetric(PM_FocusBorderSpacing));
        break;
    }
    case CT_ToolButton: {
        qreal radius = DStyle::pixelMetric(DStyle::PM_FrameRadius);
        return QSize(size.width() + radius, size.height() + radius);
    }
    case CT_ProgressBar: {
        if (const QStyleOptionProgressBar *pbo = qstyleoption_cast<const QStyleOptionProgressBar*>(opt)) {
            int radius = DStyle::pixelMetric(PM_FrameRadius);

            if (!pbo->textVisible) {
                size.setWidth(qMax(size.width(), 2 * radius));
                size.setHeight(qMax(size.height(), 2 * radius));
            } else {
                QSize text_size = opt->fontMetrics.size(0, pbo->text.isEmpty() ? "%" : pbo->text);
                size.setWidth(qMax(size.width(), 2 * radius + text_size.width()));
                size.setHeight(qMax(size.height(), 2 * radius + text_size.height()));
            }
        }
        break;
    }
    case CT_TabBarTab: {
        int tabCloseWidth = DStyle::pixelMetric(PM_TabCloseIndicatorWidth, opt, widget);
        size += QSize(tabCloseWidth, 0);
        break;
    }
    case CT_SpinBox:
        if (qstyleoption_cast<const QStyleOptionSpinBox *>(opt)) {
            size += QSize(size.height() * 2, 0);
            return size;
        }
    break;
    default:
        break;
    }

    return size;
}

int ChameleonStyle::pixelMetric(QStyle::PixelMetric m, const QStyleOption *opt,
                                const QWidget *widget) const
{
    switch (static_cast<int>(m)) {
    case PM_SpinBoxSliderHeight:
        return SpinBox_MiniHeight;
    case PM_SpinBoxFrameWidth:
        return SpinBox_FrameWidth;
    case PM_TabCloseIndicatorWidth:
    case PM_TabCloseIndicatorHeight:
        return 20;
    case PM_TabBarTabVSpace:
        return TabBar_TabMarginHeight * 2;
    case PM_TabBarTabHSpace :
        return TabBar_TabMarginWidth * 2;
    case PM_TabBarTabOverlap:
        return TabBar_TabOverlap;
    case PM_TabBarBaseOverlap:
        return TabBar_BaseOverlap;
    case PM_TabBarTabShiftHorizontal:
    case PM_TabBarTabShiftVertical:
        return 0;
    case PM_TabBarScrollButtonWidth:
        return 40;
    case PM_MenuScrollerHeight:
        return 10 + Metrics::Frame_FrameWidth ;
    case PM_MenuPanelWidth:
        return 0;
    case PM_ComboBoxFrameWidth: { //这是ComboBox VMargin
        const QStyleOptionComboBox *comboBoxOption(qstyleoption_cast< const QStyleOptionComboBox *>(opt));
        return comboBoxOption && comboBoxOption->editable ? Metrics::ComboBox_FrameWidth : Metrics::LineEdit_FrameWidth ;
    }
    case PM_MenuVMargin:
        return 8;
    case PM_MenuHMargin:
        return 0;
    default:
        break;
    }

    return DStyle::pixelMetric(m, opt, widget);
}

int ChameleonStyle::styleHint(QStyle::StyleHint sh, const QStyleOption *opt,
                              const QWidget *w, QStyleHintReturn *shret) const
{
    switch (sh) {
    case SH_ItemView_ShowDecorationSelected:
    case SH_ScrollBar_Transient:
        return true;
    //增加菜单鼠标事件跟踪
    case SH_MenuBar_MouseTracking:
    case SH_Menu_MouseTracking:
        return true;
    //增加TabBar超出范围的左右导航按钮
    case SH_TabBar_PreferNoArrows:
        return false;
    case SH_ComboBox_Popup:
        return true;
    case SH_ComboBox_PopupFrameStyle:
        return true;
    case SH_Slider_AbsoluteSetButtons:
        return Qt::LeftButton | Qt::MidButton;
    case SH_ToolTipLabel_Opacity:
        return 255;
    default:
        break;
    }

    return DStyle::styleHint(sh, opt, w, shret);
}

void ChameleonStyle::polish(QWidget *w)
{
    DStyle::polish(w);

    if (qobject_cast<QAbstractButton *>(w)
            || qobject_cast<QComboBox *>(w)
            || qobject_cast<QScrollBar *>(w)
            || qobject_cast<QCheckBox *>(w)
            || qobject_cast<QAbstractSpinBox *>(w)
            || qobject_cast<QTabBar *>(w)
            || qobject_cast<QCheckBox *>(w)) {
        w->setAttribute(Qt::WA_Hover, true);
    }

    if (auto view = qobject_cast<QAbstractItemView *>(w)) {
        view->viewport()->setAttribute(Qt::WA_Hover, true);
    }

    if (auto scrollbar = qobject_cast<QScrollBar *>(w)) {
        scrollbar->setAttribute(Qt::WA_OpaquePaintEvent, false);
    }

    if (auto container = qobject_cast<QComboBoxPrivateContainer *>(w)) {
        DPlatformWindowHandle handle(container);
        handle.setWindowRadius(DStyle::pixelMetric(PM_FrameRadius));

        container->setFrameStyle(QFrame::NoFrame);
    }

    if (auto calendar = qobject_cast<QCalendarWidget* >(w)) {
        int radius = DStyle::pixelMetric(PM_TopLevelWindowRadius);
        DPlatformWindowHandle handle(calendar);

        handle.setWindowRadius(radius);
        calendar->setVerticalHeaderFormat(QCalendarWidget::NoVerticalHeader);

        auto topWidget = calendar->findChild<QWidget *>("qt_calendar_navigationbar");
        topWidget->setBackgroundRole(DPalette::Window);

        auto layout = qobject_cast<QLayout*>(topWidget->layout());
        layout->setMargin(radius / 2);
    }

    if (w->objectName() == "qt_calendar_yearbutton"
                        || w->objectName() == "qt_calendar_monthbutton") {
        w->setProperty("_d_calendarToolBtn", true);
    }

    if (DApplication::isDXcbPlatform()) {
        bool is_menu = qobject_cast<QMenu *>(w);
        bool is_tip = w->inherits("QTipLabel");

        // 当窗口已经创建对应的native窗口，要判断当前是否已经设置了窗口背景透明
        // Bug: https://github.com/linuxdeepin/internal-discussion/issues/323
        if (is_menu && w->windowHandle()) {
            if (const QPlatformWindow *handle = w->windowHandle()->handle()) {
                if (!w->testAttribute(Qt::WA_TranslucentBackground) && !handle->isExposed()) {
                    // 销毁现有的native窗口，否则设置Qt::WA_TranslucentBackground不会生效
                    class DQWidget : public QWidget {public: using QWidget::destroy;};
                    reinterpret_cast<DQWidget *>(w)->destroy(true, false);
                }
            }
        }

        if (is_menu) {
            DPlatformWindowHandle handle(w);

            if (DPlatformWindowHandle::isEnabledDXcb(w)) {
                handle.setEnableBlurWindow(true);
                handle.setWindowRadius(8);
                w->setAttribute(Qt::WA_TranslucentBackground);
            }
        } else if (is_tip) {
            DPlatformWindowHandle handle(w);
            handle.setWindowRadius(DStyle::pixelMetric(PM_FrameRadius));
            QLabel *label = qobject_cast<QLabel *>(w);
            label->setTextFormat(DStyle::tooltipTextFormat());
        }
    }
}

void ChameleonStyle::unpolish(QWidget *w)
{
    DStyle::unpolish(w);

    if (qobject_cast<QAbstractButton *>(w)
            || qobject_cast<QComboBox *>(w)
            || qobject_cast<QScrollBar *>(w)
            || qobject_cast<QCheckBox *>(w)
            || qobject_cast<QAbstractSpinBox *>(w)
            || qobject_cast<QTabBar *>(w)
            || qobject_cast<QCheckBox *>(w)) {
        w->setAttribute(Qt::WA_Hover, false);
    }

    if (auto view = qobject_cast<QAbstractItemView *>(w)) {
        view->viewport()->setAttribute(Qt::WA_Hover, false);
    }

    if (auto scrollbar = qobject_cast<QScrollBar *>(w)) {
        scrollbar->setAttribute(Qt::WA_OpaquePaintEvent, true);
    }
}

void ChameleonStyle::unpolish(QApplication *application)
{
    DStyle::unpolish(application);
}

void ChameleonStyle::drawShadow(QPainter *p, const QRect &rect, const QColor &color) const
{
    int frame_radius = DStyle::pixelMetric(PM_FrameRadius);
    int shadow_xoffset = DStyle::pixelMetric(PM_ShadowHOffset);
    int shadow_yoffset = DStyle::pixelMetric(PM_ShadowVOffset);

    QRect shadow = rect;
    QPoint pointOffset(rect.center().x() + shadow_xoffset, rect.center().y() + shadow_yoffset);
    shadow.moveCenter(pointOffset);

    p->setBrush(color);
    p->setPen(Qt::NoPen);
    p->setRenderHint(QPainter::Antialiasing);
    p->drawRoundedRect(shadow, frame_radius, frame_radius);
}

void ChameleonStyle::drawBorder(QPainter *p, const QStyleOption *opt, const QWidget *w) const
{
    const QColor &focus_color = getColor(opt, QPalette::Highlight);

    if (!focus_color.isValid() || focus_color.alpha() == 0)
        return;

    int frame_radius = DStyle::pixelMetric(PM_FrameRadius);
    int margins = DStyle::pixelMetric(PM_FrameMargins);

    QRect border = opt->rect.adjusted(margins, margins, -margins, -margins);
    p->setRenderHint(QPainter::Antialiasing);
    QPen pen;
    pen.setWidth(2);
    pen.setColor(focus_color);
    p->setPen(pen);
    p->setBrush(Qt::NoBrush);

    bool table = qobject_cast<const QTableView *>(w);

    if (table) {
        p->drawRect(border);
    } else {
        p->drawRoundedRect(border, frame_radius + margins, frame_radius + margins);
    }

    pen.setWidth(1);
    pen.setColor(getColor(opt, QPalette::Base));
    p->setPen(pen);
    const int offset = 2;

    if (table) {
        QRect rect = border.adjusted(offset, offset, -1, -1);
        p->drawLine(rect.topLeft(), rect.topRight());
        p->drawLine(rect.bottomLeft(), rect.bottomRight());
        p->drawLine(rect.topLeft(), rect.bottomLeft());
        p->drawLine(rect.topRight(), rect.bottomRight());
    } else {
        p->drawRoundedRect(border.adjusted(offset, offset, -offset, -offset), frame_radius, frame_radius);
    }

}

bool ChameleonStyle::isNoticks(const QStyleOptionSlider *slider, QPainter *p, const QWidget *w) const
{
    Q_UNUSED(p)
    const DSlider *dslider = qobject_cast<const DSlider *>(w->parent());
    QSlider::TickPosition tickPosition = slider->tickPosition;

    if (dslider)
        tickPosition = dslider->tickPosition();

    return tickPosition == QSlider::NoTicks;
}

QBrush ChameleonStyle::generatedBrush(StateFlags flags, const QBrush &base, QPalette::ColorGroup cg, QPalette::ColorRole role, const QStyleOption *option) const
{
    return DStyle::generatedBrush(flags, base, cg, role, option);
}

QBrush ChameleonStyle::generatedBrush(DStyle::StateFlags flags, const QBrush &base, QPalette::ColorGroup cg, DPalette::ColorType type, const QStyleOption *option) const
{
    return DStyle::generatedBrush(flags, base, cg, type, option);
}

QColor ChameleonStyle::getColor(const QStyleOption *option, QPalette::ColorRole role) const
{
    return DStyle::generatedBrush(option, option->palette.brush(role), option->palette.currentColorGroup(), role).color();
}

QColor ChameleonStyle::getColor(const QStyleOption *option, DPalette::ColorType type, const QWidget *widget) const
{
    const DPalette &pa = DApplicationHelper::instance()->palette(widget, option->palette);

    return DStyle::generatedBrush(option, pa.brush(type), pa.currentColorGroup(), type).color();
}

QMargins ChameleonStyle::frameExtentMargins() const
{
    int margins = DStyle::pixelMetric(PM_FrameMargins);

    return QMargins(margins, margins, margins, margins);
}

QRect ChameleonStyle::drawButtonDownArrow(const QStyleOption *opt, QPainter *p, const QWidget *w) const
{
    const QStyleOptionButton *btn = qstyleoption_cast<const QStyleOptionButton *>(opt);

    if (!btn)
        return QRect(-1, -1, -1, -1);

    QRect rectOpt = btn->rect;                      //实际绘画箭头所占的小矩形
    int arrowWidth = DStyle::pixelMetric(PM_MenuButtonIndicator, opt, w);
    int arrowHeight = arrowWidth;
    QRect rectArrow(0, 0 , arrowWidth, arrowHeight);
    rectArrow.moveCenter(rectOpt.center());

    QStyleOptionButton newBtn = *btn;                 //绘画箭头的大矩形(不要竖线)
    QRect &newRect = newBtn.rect;
    newRect.setHeight(rectOpt.height());
    newRect.setWidth(arrowWidth);
    newRect.moveCenter(rectOpt.center());

    if (btn->direction == Qt::LeftToRight) {
        rectArrow.moveRight(rectOpt.right());
        newRect.moveRight(rectOpt.right());
    } else {
        rectArrow.moveLeft(rectOpt.left());
        newRect.moveLeft(rectOpt.left());
    }

    if (p == nullptr || w == nullptr)
        return newRect;

    QStyleOptionButton arrowDrawBtn  = newBtn;
    arrowDrawBtn.rect = rectArrow;
    proxy()->drawPrimitive(PE_IndicatorArrowDown, &arrowDrawBtn, p, w);

    return newRect;
}

} // namespace chameleon
