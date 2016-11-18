#include "qdeepinplatformmenu.h"
#include "qdeepintheme.h"
#include "qdeepinplatformmenuitem.h"

#include <dplatformwindowhandle.h>
#include <DApplication>

#include <QMenu>
#include <QFile>
#include <QDebug>

#include <private/qwidgetwindow_p.h>

#define ITEM2ACTION(item) ({item ? reinterpret_cast<QAction*>(item->tag()) : Q_NULLPTR;})

DWIDGET_USE_NAMESPACE

QDeepinPlatformMenu::QDeepinPlatformMenu()
    : QPlatformMenu()
{
    bool tmp = QDeepinTheme::m_usePlatformNativeMenu;
    QDeepinTheme::m_usePlatformNativeMenu = false;
    menu = new QMenu();

    if (DApplication::isDXcbPlatform()) {
        DPlatformWindowHandle::enableDXcbForWindow(menu);
    }

    QFile file(":/menu.qss");

    if (file.open(QIODevice::ReadOnly)) {
        menu->setStyleSheet(file.readAll());
        file.close();
    }

    QDeepinTheme::m_usePlatformNativeMenu = tmp;
}

QDeepinPlatformMenu::~QDeepinPlatformMenu()
{
    for (QPlatformMenuItem *item : itemList) {
        item->deleteLater();
    }

    if (menu)
        menu->deleteLater();
}

void QDeepinPlatformMenu::insertMenuItem(QPlatformMenuItem *menuItem, QPlatformMenuItem *before)
{
    menu->insertAction(ITEM2ACTION(before), ITEM2ACTION(menuItem));
    itemList.insert(std::find(itemList.begin(),
                              itemList.end(),
                              before),
                    menuItem);

    connect(menuItem, &QPlatformMenuItem::destroyed, this, &QDeepinPlatformMenu::onMenuItemDestory);
}

void QDeepinPlatformMenu::removeMenuItem(QPlatformMenuItem *menuItem)
{
    if (!menuItem)
        return;

    menu->removeAction(ITEM2ACTION(menuItem));
    itemList.removeOne(menuItem);
}

void QDeepinPlatformMenu::syncMenuItem(QPlatformMenuItem *menuItem)
{
    Q_UNUSED(menuItem)
}

void QDeepinPlatformMenu::syncSeparatorsCollapsible(bool enable)
{
    menu->setSeparatorsCollapsible(enable);
}

void QDeepinPlatformMenu::setTag(quintptr tag)
{
    reinterpret_cast<QAction*>(tag)->setMenu(menu);
}

quintptr QDeepinPlatformMenu::tag() const
{
    return (quintptr)menu->menuAction();
}

void QDeepinPlatformMenu::setText(const QString &text)
{
    if (menu->menuAction())
        menu->menuAction()->setText(text);
}

void QDeepinPlatformMenu::setIcon(const QIcon &icon)
{
    if (menu->menuAction())
        menu->menuAction()->setIcon(icon);
}

void QDeepinPlatformMenu::setEnabled(bool enabled)
{
    menu->setEnabled(enabled);
}

bool QDeepinPlatformMenu::isEnabled() const
{
    return menu->isEnabled();
}

void QDeepinPlatformMenu::setVisible(bool visible)
{
    menu->setVisible(visible);
}

void QDeepinPlatformMenu::setMinimumWidth(int width)
{
    menu->setMinimumWidth(width);
}

void QDeepinPlatformMenu::setFont(const QFont &font)
{
    menu->setFont(font);
}

void QDeepinPlatformMenu::setMenuType(QPlatformMenu::MenuType type)
{
    Q_UNUSED(type)
}

void QDeepinPlatformMenu::showPopup(const QWindow *parent, const QRect &targetRect, const QPlatformMenuItem *item)
{
    if (parent && parent->metaObject()->className() == QStringLiteral("QWidgetWindow")) {
        menu->setParent(static_cast<QWidgetWindow*>(const_cast<QWindow*>(parent))->widget());
    }

    menu->popup(targetRect.topLeft(), ITEM2ACTION(item));
}

QPlatformMenuItem *QDeepinPlatformMenu::menuItemAt(int position) const
{
    return itemList.value(position);
}

QPlatformMenuItem *QDeepinPlatformMenu::menuItemForTag(quintptr tag) const
{
    Q_UNUSED(tag)
//    for (QPlatformMenuItem *item : itemList) {
//        if (item->tag() == tag)
//            return item;
//    }

    return 0;
}

QPlatformMenuItem *QDeepinPlatformMenu::createMenuItem() const
{
    return new QDeepinPlatformMenuItem;
}

QPlatformMenu *QDeepinPlatformMenu::createSubMenu() const
{
    return new QDeepinPlatformMenu;
}

void QDeepinPlatformMenu::onMenuItemDestory()
{
    QPlatformMenuItem *item = qobject_cast<QPlatformMenuItem*>(sender());

    if (item)
        itemList.removeOne(item);
}
