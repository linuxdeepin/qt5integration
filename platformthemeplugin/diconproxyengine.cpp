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

#include "diconproxyengine.h"

DIconProxyEngine::DIconProxyEngine(const QIcon &proxyIcon)
    : QIconEngine()
    , m_proxyIcon(proxyIcon)
{

}

void DIconProxyEngine::paint(QPainter *painter, const QRect &rect, QIcon::Mode mode, QIcon::State state)
{
    m_proxyIcon.paint(painter, rect, Qt::AlignCenter, mode, state);
}

QPixmap DIconProxyEngine::pixmap(const QSize &size, QIcon::Mode mode, QIcon::State state)
{
    return m_proxyIcon.pixmap(size, mode, state);
}

QSize DIconProxyEngine::actualSize(const QSize &size, QIcon::Mode mode, QIcon::State state)
{
    return m_proxyIcon.actualSize(size, mode, state);
}

QIconEngine *DIconProxyEngine::clone() const
{
    return new DIconProxyEngine(m_proxyIcon);
}

bool DIconProxyEngine::read(QDataStream &in)
{
    in << m_proxyIcon.name();
    return true;
}

bool DIconProxyEngine::write(QDataStream &out) const
{
    Q_UNUSED(out)

    return false;
}
