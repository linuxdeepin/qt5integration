/*
 * Copyright (C) 2017 ~ 2017 Deepin Technology Co., Ltd.
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

#ifndef DICONPROXYENGINE_H
#define DICONPROXYENGINE_H

#include <QIconEngine>

class DIconProxyEngine : public QIconEngine
{
public:
    explicit DIconProxyEngine(const QIcon &proxyIcon);

    void paint(QPainter *painter, const QRect &rect, QIcon::Mode mode, QIcon::State state) Q_DECL_OVERRIDE;
    QPixmap pixmap(const QSize &size, QIcon::Mode mode, QIcon::State state) Q_DECL_OVERRIDE;
    QSize actualSize(const QSize &size, QIcon::Mode mode, QIcon::State state) Q_DECL_OVERRIDE;
    QIconEngine *clone() const Q_DECL_OVERRIDE;
    bool read(QDataStream &in) Q_DECL_OVERRIDE;
    bool write(QDataStream &out) const Q_DECL_OVERRIDE;

private:
    QIcon m_proxyIcon;
};

#endif // DICONPROXYENGINE_H
