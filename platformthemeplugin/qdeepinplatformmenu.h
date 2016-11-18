#ifndef QDEEPINPLATFORMMENU_H
#define QDEEPINPLATFORMMENU_H

#include <QPointer>
#include <QMap>

#include <qpa/qplatformmenu.h>

QT_BEGIN_NAMESPACE
class QMenu;
class QAction;
QT_END_NAMESPACE

class QDeepinPlatformMenu : public QPlatformMenu
{
    Q_OBJECT
public:
    QDeepinPlatformMenu();
    ~QDeepinPlatformMenu();

    void insertMenuItem(QPlatformMenuItem *menuItem, QPlatformMenuItem *before) Q_DECL_OVERRIDE;
    void removeMenuItem(QPlatformMenuItem *menuItem) Q_DECL_OVERRIDE;
    void syncMenuItem(QPlatformMenuItem *menuItem) Q_DECL_OVERRIDE;
    void syncSeparatorsCollapsible(bool enable) Q_DECL_OVERRIDE;

    void setTag(quintptr tag) Q_DECL_OVERRIDE;
    quintptr tag()const Q_DECL_OVERRIDE;

    void setText(const QString &text) Q_DECL_OVERRIDE;
    void setIcon(const QIcon &icon) Q_DECL_OVERRIDE;
    void setEnabled(bool enabled) Q_DECL_OVERRIDE;
    bool isEnabled() const Q_DECL_OVERRIDE;
    void setVisible(bool visible) Q_DECL_OVERRIDE;
    void setMinimumWidth(int width) Q_DECL_OVERRIDE;
    void setFont(const QFont &font) Q_DECL_OVERRIDE;
    void setMenuType(MenuType type) Q_DECL_OVERRIDE;

    void showPopup(const QWindow *parentWindow, const QRect &targetRect, const QPlatformMenuItem *item);
//    void dismiss() { } // Closes this and all its related menu popups

    QPlatformMenuItem *menuItemAt(int position) const Q_DECL_OVERRIDE;
    QPlatformMenuItem *menuItemForTag(quintptr tag) const Q_DECL_OVERRIDE;

    QPlatformMenuItem *createMenuItem() const  Q_DECL_OVERRIDE;
    QPlatformMenu *createSubMenu() const  Q_DECL_OVERRIDE;

private:
    void onMenuItemDestory();

    QPointer<QMenu> menu;
    QList<QPlatformMenuItem*> itemList;

    friend class QDeepinPlatformMenuItem;
};

#endif // QDEEPINPLATFORMMENU_H
