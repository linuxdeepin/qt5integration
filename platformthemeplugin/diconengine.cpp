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
    , m_szIconFilePath(nullptr)
{
}

DIconEngine::~DIconEngine()
{
    if (m_szIconFilePath)
        free_cstring(const_cast<char *>(m_szIconFilePath));
    m_szIconFilePath = nullptr;
}

void DIconEngine::paint(QPainter *painter, const QRect &rect, QIcon::Mode mode, QIcon::State state)
{
    qDebug() << Q_FUNC_INFO << rect << mode << state;

    const QSize &pixSize = rect.size();

    painter->drawPixmap(rect, pixmap(pixSize, mode, state));
}

QIconEngine *DIconEngine::clone() const
{
    qDebug() << Q_FUNC_INFO;

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
    qDebug() << "Caca";

    return QString();
}

QList<QSize> DIconEngine::availableSizes(QIcon::Mode mode, QIcon::State state) const
{
    qDebug() << Q_FUNC_INFO << mode << state;
    Q_UNUSED(mode);
    Q_UNUSED(state);
//    return QList<QSize>({ {48, 48}, {16, 16}, {22, 22}, {24, 24} });

    // just return a invalid size
    return QList<QSize>({ QSize() });
}

QPixmap DIconEngine::pixmap(const QSize &size, QIcon::Mode mode, QIcon::State state)
{
    qDebug() << Q_FUNC_INFO << size << mode << state;
    qDebug() << m_iconName;

    const auto szFilePath = find_icon_with_theme_name("Sea", m_iconName.toStdString().c_str(), size.width(), 1);
    qDebug() << szFilePath;

    if (!szFilePath)
        return QPixmap();

    const QPixmap &pix = DSvgRenderer::render(szFilePath, size);

    free_cstring(const_cast<char *>(szFilePath));

    return pix;
}
