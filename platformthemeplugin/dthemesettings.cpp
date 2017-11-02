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
#include "dthemesettings.h"

#include <QFile>
#include <QTimer>

#include <DFileWatcherManager>

#define ICON_THEME_NAME QStringLiteral("IconThemeName")
#define F_ICON_THEME_NAME QStringLiteral("FallBackIconThemeName")
#define STYLE_NAMES QStringLiteral("StyleNames")
#define SYSTEM_FONT QStringLiteral("Font")
#define SYSTEM_FIXED_FONT QStringLiteral("MonoFont")
#define SYSTEM_FONT_POINT_SIZE QStringLiteral("FontSize")

DCORE_USE_NAMESPACE

DThemeSettings::DThemeSettings(QObject *parent)
    : QObject(parent)
    , settings(QSettings::IniFormat,
               QSettings::UserScope,
               "deepin", "qt-theme")
{
    settings.setIniCodec("utf-8");
    settings.beginGroup("Theme");

    QStringList list;

    list << settings.fileName();
    list << QSettings(QSettings::IniFormat,
                      QSettings::SystemScope,
                      "deepin", "qt-theme").fileName();

    DFileWatcherManager *watcher = new DFileWatcherManager(this);

    for (const QString &path : list) {
        QFile file(path);

        if (!file.exists()) {
            file.open(QFile::WriteOnly);
            file.close();
        }

        if (QFile::exists(path))
            watcher->add(path);
    }

    connect(watcher, &DFileWatcherManager::fileModified, this, &DThemeSettings::onConfigChanged);
}

bool DThemeSettings::contains(const QString &key) const
{
    return settings.contains(key);
}

QVariant DThemeSettings::value(const QString &key, const QVariant &defaultValue) const
{
    return settings.value(key, defaultValue);
}

bool DThemeSettings::isSetIconThemeName() const
{
    return contains(ICON_THEME_NAME);
}

QString DThemeSettings::iconThemeName() const
{
    return value(ICON_THEME_NAME).toString();
}

bool DThemeSettings::isSetFallbackIconThemeName() const
{
    return contains(F_ICON_THEME_NAME);
}

QString DThemeSettings::fallbackIconThemeName() const
{
    return value(F_ICON_THEME_NAME).toString();
}

bool DThemeSettings::isSetSystemFont() const
{
    return contains(SYSTEM_FONT);
}

QString DThemeSettings::systemFont() const
{
    return value(SYSTEM_FONT).toString();
}

bool DThemeSettings::isSetStyleNames() const
{
    return contains(STYLE_NAMES);
}

QStringList DThemeSettings::styleNames() const
{
    return value(STYLE_NAMES).toStringList();
}

bool DThemeSettings::isSetSystemFontPixelSize() const
{
    return contains(SYSTEM_FONT_POINT_SIZE);
}

qreal DThemeSettings::systemFontPointSize() const
{
    return value(SYSTEM_FONT_POINT_SIZE, 9.0).toDouble();
}

bool DThemeSettings::isSetSystemFixedFont() const
{
    return contains(SYSTEM_FIXED_FONT);
}

QString DThemeSettings::systemFixedFont() const
{
    return value(SYSTEM_FIXED_FONT).toString();
}

void DThemeSettings::onConfigChanged()
{
    QVariantMap config;

    for (const QString &v : settings.allKeys()) {
        config[v] = settings.value(v);
    }

    settings.sync();

    for (const QString &v : settings.allKeys()) {
        const QVariant &old_value = config.value(v);
        const QVariant &new_value = settings.value(v);

        if (old_value != new_value) {
            if (v == ICON_THEME_NAME)
                emit iconThemeNameChanged(new_value.toString());
            else if (v == F_ICON_THEME_NAME)
                emit fallbackIconThemeNameChanged(new_value.toString());
            else if (v == SYSTEM_FONT)
                emit systemFontChanged(new_value.toString());
            else if (v == STYLE_NAMES)
                emit styleNamesChanged(new_value.toStringList());
            else if (v == SYSTEM_FIXED_FONT)
                emit systemFixedFontChanged(new_value.toString());
            else if (v == SYSTEM_FONT_POINT_SIZE)
                emit systemFontPointSizeChanged(new_value.toInt());

            emit valueChanged(v, old_value, new_value);
        }
    }
}
