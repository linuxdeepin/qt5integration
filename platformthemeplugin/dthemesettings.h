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
#ifndef DTHEMESETTINGS_H
#define DTHEMESETTINGS_H

#include <QObject>
#include <QSettings>

class DThemeSettings : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString iconThemeName READ iconThemeName NOTIFY iconThemeNameChanged)
    Q_PROPERTY(QString fallbackIconThemeName READ fallbackIconThemeName NOTIFY fallbackIconThemeNameChanged)
    Q_PROPERTY(QString systemFont READ systemFont NOTIFY systemFontChanged)
    Q_PROPERTY(int systemFontPixelSize READ systemFontPixelSize NOTIFY systemFontPixelSizeChanged)
    Q_PROPERTY(QStringList styleNames READ styleNames NOTIFY styleNamesChanged)

public:
    explicit DThemeSettings(QObject *parent = 0);

    bool contains(const QString &key) const;
    QVariant value(const QString &key, const QVariant &defaultValue = QVariant()) const;

    bool isSetIconThemeName() const;
    QString iconThemeName() const;
    bool isSetFallbackIconThemeName() const;
    QString fallbackIconThemeName() const;
    bool isSetSystemFont() const;
    QString systemFont() const;
    bool isSetStyleNames() const;
    QStringList styleNames() const;
    bool isSetSystemFontPixelSize() const;
    int systemFontPixelSize() const;

signals:
    void valueChanged(const QString &key, const QVariant &oldValue, const QVariant &newValue);
    void iconThemeNameChanged(QString iconThemeName);
    void fallbackIconThemeNameChanged(QString fallbackIconThemeName);
    void systemFontChanged(QString systemFont);
    void styleNamesChanged(QStringList styleNames);
    void systemFontPixelSizeChanged(int systemFontPixelSize);

private:
    QSettings settings;

    void onConfigChanged();
};

#endif // DTHEMESETTINGS_H
