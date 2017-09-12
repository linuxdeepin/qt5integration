/**
 * Copyright (C) 2017 by Deepin Technology Co., Ltd.
 *
 * Use of this source code is governed by GPLv3
 * that can be found in the LICENSE file.
 **/

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
#if QT_VERSION >= QT_VERSION_CHECK(5, 8, 0)
    QIcon fileIcon(const QFileInfo &fileInfo,
                   QPlatformTheme::IconOptions iconOptions = 0) const Q_DECL_OVERRIDE;
#else
    QPixmap fileIconPixmap(const QFileInfo &fileInfo, const QSizeF &size,
                           QPlatformTheme::IconOptions iconOptions = 0) const Q_DECL_OVERRIDE;
#endif

    QVariant themeHint(ThemeHint hint) const Q_DECL_OVERRIDE;

    static const char *name;

private:
    static bool m_usePlatformNativeDialog;
    static QMimeDatabase m_mimeDatabase;

    friend class QDeepinFileDialogHelper;
    friend class QDeepinPlatformMenu;
};

#endif // QDEEPINTHEME_H
