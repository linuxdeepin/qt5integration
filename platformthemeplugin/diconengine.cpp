/*
 * SPDX-FileCopyrightText: 2018 - 2022 UnionTech Software Technology Co., Ltd.  
 * SPDX-License-Identifier: LGPL-3.0-or-later
 */
#include "diconengine.h"

#include <themed_icon_lookup.h>

#include <QPainter>
#include <QDebug>

#include <DSvgRenderer>

DWIDGET_USE_NAMESPACE

using namespace themed_icon_lookup;

DIconEngine::DIconEngine(const QString &iconName)
    : QIconEngine()
    , m_iconName(iconName)
{
}

DIconEngine::~DIconEngine()
{
}

void DIconEngine::paint(QPainter *painter, const QRect &rect, QIcon::Mode mode, QIcon::State state)
{
    const QSize &pixSize = rect.size();

    painter->drawPixmap(rect, pixmap(pixSize, mode, state));
}

QIconEngine *DIconEngine::clone() const
{
    return nullptr;
}

QSize DIconEngine::actualSize(const QSize &size, QIcon::Mode mode, QIcon::State state)
{
    Q_UNUSED(mode);
    Q_UNUSED(state);

    return size;
}

QString DIconEngine::key() const
{
    return QString();
}

QList<QSize> DIconEngine::availableSizes(QIcon::Mode mode, QIcon::State state) const
{
    Q_UNUSED(mode);
    Q_UNUSED(state);

    // just return a invalid size
    return QList<QSize>({ QSize() });
}

QPixmap DIconEngine::pixmap(const QSize &size, QIcon::Mode mode, QIcon::State state)
{
    Q_UNUSED(mode);
    Q_UNUSED(state);

    const char *szFilePath = find_icon_with_theme_name("Sea", m_iconName.toStdString().c_str(), size.width(), 1);
    const QString fpath(szFilePath);
    free_cstring(const_cast<char *>(szFilePath));

    if (fpath.isEmpty())
    {
        qWarning() << m_iconName << size;
        return QPixmap();
    }

    if (fpath.endsWith(".svg"))
        return DSvgRenderer::render(fpath, size);

    return QPixmap(fpath);
}
