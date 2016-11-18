#ifndef QDEEPINPLATFORMMENUITEM_H
#define QDEEPINPLATFORMMENUITEM_H

#include <qpa/qplatformmenu.h>

QT_BEGIN_NAMESPACE
class QAction;
QT_END_NAMESPACE

class QDeepinPlatformMenuItem : public QPlatformMenuItem
{
public:
    QDeepinPlatformMenuItem();

    void setTag(quintptr tag) Q_DECL_OVERRIDE;
    quintptr tag()const Q_DECL_OVERRIDE;

    void setText(const QString &text) Q_DECL_OVERRIDE;
    void setIcon(const QIcon &icon) Q_DECL_OVERRIDE;
    void setMenu(QPlatformMenu *menu) Q_DECL_OVERRIDE;
    void setVisible(bool isVisible) Q_DECL_OVERRIDE;
    void setIsSeparator(bool isSeparator) Q_DECL_OVERRIDE;
    void setFont(const QFont &font) Q_DECL_OVERRIDE;
    void setRole(MenuRole role) Q_DECL_OVERRIDE;
    void setCheckable(bool checkable) Q_DECL_OVERRIDE;
    void setChecked(bool isChecked) Q_DECL_OVERRIDE;
    void setShortcut(const QKeySequence& shortcut) Q_DECL_OVERRIDE;
    void setEnabled(bool enabled) Q_DECL_OVERRIDE;
    void setIconSize(int size) Q_DECL_OVERRIDE;

private:
    QAction *action = Q_NULLPTR;
};

#endif // QDEEPINPLATFORMMENUITEM_H
