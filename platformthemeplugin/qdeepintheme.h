#ifndef QDEEPINTHEME_H
#define QDEEPINTHEME_H

#include <QMimeDatabase>

#include <private/qgenericunixthemes_p.h>
#include <qpa/qplatformwindow.h>
#include <qpa/qplatformnativeinterface.h>

class QDeepinTheme : public QGenericUnixTheme
{
public:
    QDeepinTheme();
    ~QDeepinTheme();

    bool usePlatformNativeDialog(DialogType type) const Q_DECL_OVERRIDE;
    QPlatformDialogHelper *createPlatformDialogHelper(DialogType type) const Q_DECL_OVERRIDE;

    QIconEngine *createIconEngine(const QString &iconName) const Q_DECL_OVERRIDE;
    QPixmap standardPixmap(StandardPixmap sp, const QSizeF &size) const Q_DECL_OVERRIDE;
    QPixmap fileIconPixmap(const QFileInfo &fileInfo, const QSizeF &size,
                           QPlatformTheme::IconOptions iconOptions = 0) const Q_DECL_OVERRIDE;

    QVariant themeHint(ThemeHint hint) const Q_DECL_OVERRIDE;

    static const char *name;

private:
    static bool m_usePlatformNativeDialog;
    static QMimeDatabase m_mimeDatabase;

    friend class QDeepinFileDialogHelper;
    friend class QDeepinPlatformMenu;
};

#endif // QDEEPINTHEME_H
