#include "style.h"
#include "commonhelper.h"
#include "paletteextended.h"

#include <QPainter>
#include <QStyleOptionMenuItem>
#include <QComboBox>
#include <QDebug>

namespace dstyle {
bool Style::drawMenuItemControl(const QStyleOption *option, QPainter *painter, const QWidget *widget) const
{
    Q_D(const Style);

    painter->save();
    // Draws one item in a popup menu.
    if (const QStyleOptionMenuItem *menuItem = qstyleoption_cast<const QStyleOptionMenuItem *>(option)) {
        //TODO(zccrs): Temporarily fix the font size to 13px
        const_cast<QStyleOptionMenuItem*>(menuItem)->font.setPixelSize(13);

        QColor highlightOutline = d->highlightedOutline(option->palette);
        QColor highlight = option->palette.highlight().color();
        if (menuItem->menuItemType == QStyleOptionMenuItem::Separator) {
            int w = 0;
            if (!menuItem->text.isEmpty()) {
                painter->setFont(menuItem->font);
                proxy()->drawItemText(painter, menuItem->rect.adjusted(Menu_SeparatorItemHMargin, 0, -Menu_SeparatorItemHMargin, 0), Qt::AlignLeft | Qt::AlignVCenter,
                                      menuItem->palette, menuItem->state & State_Enabled, menuItem->text,
                                      QPalette::Text);
                w = menuItem->fontMetrics.width(menuItem->text) + Menu_SeparatorItemHMargin;
            }
            painter->setPen(m_palette->brush(PaletteExtended::Menu_SeparatorColor, option).color());
            bool reverse = menuItem->direction == Qt::RightToLeft;
            painter->drawLine(menuItem->rect.left() + Menu_SeparatorItemHMargin + (reverse ? 0 : w), menuItem->rect.center().y(),
                              menuItem->rect.right() - Menu_SeparatorItemHMargin - (reverse ? w : 0), menuItem->rect.center().y());
            painter->restore();
            return true;
        }
        bool selected = menuItem->state & State_Selected && menuItem->state & State_Enabled;
        if (selected) {
            QRect r = option->rect;
            painter->fillRect(r, highlight);
            painter->setPen(QPen(highlightOutline));
            painter->drawRect(QRectF(r));
        }
        bool checkable = menuItem->checkType != QStyleOptionMenuItem::NotCheckable;
        bool checked = menuItem->checked;
//        bool sunken = menuItem->state & State_Sunken;
//        bool enabled = menuItem->state & State_Enabled;

        bool ignoreCheckMark = false;
        int checkcol = qMax(menuItem->maxIconWidth, 20);

        if (qobject_cast<const QComboBox*>(widget) ||
            (option->styleObject && option->styleObject->property("_q_isComboBoxPopupItem").toBool()))
            ignoreCheckMark = true; //ignore the checkmarks provided by the QComboMenuDelegate

        if (!ignoreCheckMark) {
            // Check
            QRect checkRect(option->rect.left() + 10, option->rect.center().y() - 5, 14, 14);
            checkRect = visualRect(menuItem->direction, menuItem->rect, checkRect);
            if (checkable) {
//                if (menuItem->checkType & QStyleOptionMenuItem::Exclusive) {
                    // Radio button
                    if (checked/* || sunken*/) {
                        QStyleOptionMenuItem newMI = *menuItem;
                        newMI.rect = checkRect;

                        drawDeepinStyleIcon("check", &newMI, painter, widget);
                    }
                /*} else {
                    // Check box
                    if (menuItem->icon.isNull()) {
                        QStyleOptionButton box;
                        box.QStyleOption::operator=(*option);
                        box.rect = checkRect;
                        if (checked)
                            box.state |= State_On;
                        proxy()->drawPrimitive(PE_IndicatorCheckBox, &box, painter, widget);
                    }
                }*/
            }
        } else { //ignore checkmark
            if (menuItem->icon.isNull())
                checkcol = 0;
            else
                checkcol = menuItem->maxIconWidth;
        }

        // Text and icon, ripped from windows style
        bool dis = !(menuItem->state & State_Enabled);
        bool act = menuItem->state & State_Selected;
        const QStyleOption *opt = option;
        const QStyleOptionMenuItem *menuitem = menuItem;

        QPainter *p = painter;
        QRect vCheckRect = visualRect(opt->direction, menuitem->rect,
                                      QRect(menuitem->rect.x() + 4, menuitem->rect.y(),
                                            checkcol, menuitem->rect.height()));
        if (!menuItem->icon.isNull()) {
            QIcon::Mode mode = dis ? QIcon::Disabled : QIcon::Normal;
            if (act && !dis)
                mode = QIcon::Active;
            QPixmap pixmap;

            int smallIconSize = proxy()->pixelMetric(PM_SmallIconSize, option, widget);
            QSize iconSize(smallIconSize, smallIconSize);
            if (const QComboBox *combo = qobject_cast<const QComboBox*>(widget))
                iconSize = combo->iconSize();
            if (checked)
                pixmap = menuItem->icon.pixmap(iconSize, mode, QIcon::On);
            else
                pixmap = menuItem->icon.pixmap(iconSize, mode);

            const int pixw = pixmap.width() / pixmap.devicePixelRatio();
            const int pixh = pixmap.height() / pixmap.devicePixelRatio();

            QRect pmr(0, 0, pixw, pixh);
            pmr.moveCenter(vCheckRect.center());
            painter->setPen(menuItem->palette.text().color());
            if (!ignoreCheckMark && checkable && checked) {
                QStyleOption opt = *option;
                if (act) {
                    QColor activeColor = mergedColors(option->palette.background().color(),
                                                      option->palette.highlight().color());
                    opt.palette.setBrush(QPalette::Button, activeColor);
                }
                opt.state |= State_Sunken;
                opt.rect = vCheckRect;
                proxy()->drawPrimitive(PE_PanelButtonCommand, &opt, painter, widget);
            }
            painter->drawPixmap(pmr.topLeft(), pixmap);
        }
        if (selected) {
            painter->setPen(menuItem->palette.highlightedText().color());
        } else {
            painter->setPen(menuItem->palette.text().color());
        }
        int x, y, w, h;
        menuitem->rect.getRect(&x, &y, &w, &h);
        int tab = menuitem->tabWidth;
        QColor discol;
        if (dis) {
            discol = menuitem->palette.brush(QPalette::Disabled, QPalette::Text).color();
            p->setPen(discol);
        }
        int xm = Menu_ItemFrameWidth + checkcol + Menu_ItemHTextMargin + 2;
        int xpos = menuitem->rect.x() + xm;

        QRect textRect(xpos, y + Menu_ItemVTextMargin, w - xm - Menu_PanelRightPadding - tab + 1, h - 2 * Menu_ItemVTextMargin);
        QRect vTextRect = visualRect(opt->direction, menuitem->rect, textRect);
        QString s = menuitem->text;
        if (!s.isEmpty()) {                     // draw text
            p->save();
            QFont font = menuitem->font;
            // font may not have any "hard" flags set. We override
            // the point size so that when it is resolved against the device, this font will win.
            // This is mainly to handle cases where someone sets the font on the window
            // and then the combo inherits it and passes it onward. At that point the resolve mask
            // is very, very weak. This makes it stonger.
            font.setPointSizeF(QFontInfo(menuItem->font).pointSizeF());

            if (menuitem->menuItemType == QStyleOptionMenuItem::DefaultItem)
                font.setBold(true);

            p->setFont(font);

            int t = s.indexOf(QLatin1Char('\t'));
            int text_flags = Qt::AlignVCenter | Qt::TextShowMnemonic | Qt::TextDontClip | Qt::TextSingleLine;
            if (!styleHint(SH_UnderlineShortcut, menuitem, widget))
                text_flags |= Qt::TextHideMnemonic;
            text_flags |= Qt::AlignLeft;
            if (t >= 0) {
                QRect vShortcutRect = visualRect(opt->direction, menuitem->rect,
                                                 QRect(textRect.topRight(), QPoint(menuitem->rect.right(), textRect.bottom())));
                if (dis && !act && proxy()->styleHint(SH_EtchDisabledText, option, widget)) {
                    p->setPen(menuitem->palette.light().color());
                    p->drawText(vShortcutRect.adjusted(1, 1, 1, 1), text_flags, s.mid(t + 1));
                    p->setPen(discol);
                }
                p->drawText(vShortcutRect, text_flags, s.mid(t + 1));
                s = s.left(t);
            }

            if (dis && !act && proxy()->styleHint(SH_EtchDisabledText, option, widget)) {
                p->setPen(menuitem->palette.light().color());
                p->drawText(vTextRect.adjusted(1, 1, 1, 1), text_flags, s.left(t));
                p->setPen(discol);
            }
            p->drawText(vTextRect, text_flags, s.left(t));
            p->restore();
        }

        // Arrow
        if (menuItem->menuItemType == QStyleOptionMenuItem::SubMenu) {// draw sub menu arrow
            int dim = (menuItem->rect.height() - 4) / 2;
//            PrimitiveElement arrow;
//            arrow = option->direction == Qt::RightToLeft ? PE_IndicatorArrowLeft : PE_IndicatorArrowRight;
            int xpos = menuItem->rect.left() + menuItem->rect.width() - 3 - dim;
            QRect  vSubMenuRect = visualRect(option->direction, menuItem->rect,
                                             QRect(xpos, menuItem->rect.top() + menuItem->rect.height() / 2 - dim / 2, dim, dim));
            QStyleOptionMenuItem newMI = *menuItem;
            newMI.rect = vSubMenuRect;
            if (selected)
                newMI.palette.setColor(QPalette::Foreground,
                                       newMI.palette.highlightedText().color());

            drawDeepinStyleIcon("arrow-right", &newMI, painter, widget);
        }
    }
    painter->restore();

    return true;
}

bool Style::drawMenuBarItemControl(const QStyleOption *option, QPainter *painter, const QWidget *widget) const
{
    Style *style = CommonHelper::widgetStyle(widget);
    if (!style) return false;

    const bool enabled(option->state & QStyle::State_Enabled);
    const bool mouseOver(option->state & QStyle::State_MouseOver);
    const bool sunken((option->state | QStyle::State_Sunken) == option->state);

    if (mouseOver || sunken) {
        const QColor shadow( Qt::transparent );
        const QBrush outline(style->m_palette->brush(PaletteExtended::PushButton_BorderBrush, option));
        const QBrush background(style->m_palette->brush(PaletteExtended::PushButton_BackgroundBrush, option));

        // render
        drawPushButtonFrame(painter, option->rect, background, outline, shadow );
    }

    if (const QStyleOptionMenuItem *mbi = qstyleoption_cast<const QStyleOptionMenuItem *>(option)) {
        uint alignment = Qt::AlignCenter | Qt::TextShowMnemonic | Qt::TextDontClip
                        | Qt::TextSingleLine;
        if (!proxy()->styleHint(SH_UnderlineShortcut, mbi, widget))
            alignment |= Qt::TextHideMnemonic;
        int iconExtent = proxy()->pixelMetric(PM_SmallIconSize);
        QPixmap pix = mbi->icon.pixmap(qt_getWindow(widget), QSize(iconExtent, iconExtent), (enabled) ? (mouseOver ? QIcon::Active : QIcon::Normal) : QIcon::Disabled);
        if (!pix.isNull())
            proxy()->drawItemPixmap(painter, mbi->rect, alignment, pix);
        else
            proxy()->drawItemText(painter, mbi->rect, alignment, mbi->palette, enabled,
                         mbi->text, QPalette::ButtonText);
    }

    return true;
}

} // end namespace dstyle
