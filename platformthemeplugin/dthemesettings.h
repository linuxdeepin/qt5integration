/*
 * Copyright (C) 2017 ~ 2018 Deepin Technology Co., Ltd.
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

/*!
    \typedef DDpi
    \relates DThemeSettings

    Synonym for QPair<qreal, qreal>.
*/
typedef QPair<qreal, qreal> DDpi;

class DThemeSettings : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString iconThemeName READ iconThemeName NOTIFY iconThemeNameChanged)
    Q_PROPERTY(QString fallbackIconThemeName READ fallbackIconThemeName NOTIFY fallbackIconThemeNameChanged)
    Q_PROPERTY(QString systemFont READ systemFont NOTIFY systemFontChanged)
    Q_PROPERTY(QString systemFixedFont READ systemFixedFont NOTIFY systemFixedFontChanged)
    Q_PROPERTY(qreal systemFontPointSize READ systemFontPointSize NOTIFY systemFontPointSizeChanged)
    Q_PROPERTY(QStringList styleNames READ styleNames NOTIFY styleNamesChanged)
    Q_PROPERTY(int touchFlickBeginMoveDelay READ touchFlickBeginMoveDelay NOTIFY touchFlickBeginMoveDelayChanged)
    Q_PROPERTY(qreal scaleFactor READ scaleFactor NOTIFY scaleFactorChanged)
    Q_PROPERTY(QByteArray screenScaleFactors READ screenScaleFactors NOTIFY screenScaleFactorsChanged)
    Q_PROPERTY(DDpi scaleLogicalDpi READ scaleLogicalDpi NOTIFY scaleLogicalDpiChanged)
    Q_PROPERTY(bool autoScaleWindow READ autoScaleWindow NOTIFY autoScaleWindowChanged)

public:
    DThemeSettings(bool watchFile = true, QObject *parent = 0);

    static QSettings *makeSettings();

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
    qreal systemFontPointSize() const;
    bool isSetSystemFixedFont() const;
    QString systemFixedFont() const;
    int touchFlickBeginMoveDelay() const;
    qreal scaleFactor() const;
    QByteArray screenScaleFactors() const;
    DDpi scaleLogicalDpi() const;
    bool autoScaleWindow() const;

signals:
    void valueChanged(const QString &key, const QVariant &oldValue, const QVariant &newValue);
    void iconThemeNameChanged(QString iconThemeName);
    void fallbackIconThemeNameChanged(QString fallbackIconThemeName);
    void systemFontChanged(QString systemFont);
    void styleNamesChanged(QStringList styleNames);
    void systemFixedFontChanged(QString systemFixedFont);
    void systemFontPointSizeChanged(qreal systemFontPointSize);
    void touchFlickBeginMoveDelayChanged(int touchFlickBeginMoveDelay);
    void scaleFactorChanged(const qreal &scaleFactor);
    void screenScaleFactorsChanged(const QByteArray &screenScaleFactors);
    void scaleLogicalDpiChanged(const DDpi scaleLogicalDpi);
    void autoScaleWindowChanged(bool autoScaleWindow);

private:
    QSettings *settings;

    void onConfigChanged();
};

#endif // DTHEMESETTINGS_H
