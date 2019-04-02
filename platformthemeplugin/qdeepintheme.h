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

#ifndef QDEEPINTHEME_H
#define QDEEPINTHEME_H

#include <QMimeDatabase>

#include <private/qgenericunixthemes_p.h>
#include <qpa/qplatformwindow.h>
#include <qpa/qplatformnativeinterface.h>

class DThemeSettings;
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
    const QFont *font(Font type) const Q_DECL_OVERRIDE;
    DThemeSettings *settings() const;
    static DThemeSettings *getSettings();

    static const char *name;

private:
    static bool m_usePlatformNativeDialog;
    static QMimeDatabase m_mimeDatabase;
    static DThemeSettings *m_settings;

    friend class QDeepinFileDialogHelper;
    friend class QDeepinPlatformMenu;
};

#endif // QDEEPINTHEME_H
