/**
 * Copyright (C) 2017 by Deepin Technology Co., Ltd.
 *
 * Use of this source code is governed by GPLv3
 * that can be found in the LICENSE file.
 **/

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
