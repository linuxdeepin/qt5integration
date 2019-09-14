/*
 * Copyright (C) 2019 ~ 2019 Deepin Technology Co., Ltd.
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
#ifndef DBUILTINICONENGINE_H
#define DBUILTINICONENGINE_H

#include <QIconEngine>
#include <private/qiconloader_p.h>

// 内置的主题引擎，会从Qt资源文件中查找图标
class DBuiltinIconEnginePrivate;
class DBuiltinIconEngine : public QIconEngine
{
public:
    DBuiltinIconEngine(const QString &iconName);
    ~DBuiltinIconEngine();
    void paint(QPainter *painter, const QRect &rect,
               QIcon::Mode mode, QIcon::State state);
    QSize actualSize(const QSize &size, QIcon::Mode mode,
                     QIcon::State state);
    QPixmap pixmap(const QSize &size, QIcon::Mode mode,
                   QIcon::State state);

    QString key() const;
    QIconEngine *clone() const;
    bool read(QDataStream &in);
    bool write(QDataStream &out) const;

    QString iconName() const override;

    static QThemeIconInfo loadIcon(const QString &iconName, uint key);

private:
    bool hasIcon() const;
    void ensureLoaded();
    void virtual_hook(int id, void *data) override;

    DBuiltinIconEngine(const DBuiltinIconEngine &other);
    QThemeIconInfo m_info;
    QString m_iconName;
    uint m_key;

    friend class QIconLoader;
};

#endif // DBUILTINICONENGINE_H
