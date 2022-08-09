/*
 * SPDX-FileCopyrightText: 2017 - 2022 UnionTech Software Technology Co., Ltd.  
 * SPDX-License-Identifier: GPL-3.0-or-later
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
