// SPDX-FileCopyrightText: 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "dciiconengine.h"

#include <DGuiApplicationHelper>
#include <DPlatformTheme>

#include <QPainter>
#include <QPixmap>

static inline DDciIcon::Theme dciTheme()
{
    auto theme = DGuiApplicationHelper::instance()->themeType();
    return theme == DGuiApplicationHelper::DarkType ? DDciIcon::Dark : DDciIcon::Light;
}

static inline DDciIcon::Mode dciMode(QIcon::Mode mode)
{
    // QIcon only support DDciIcon::Disabled and DDciIcon::Normal
    return mode == QIcon::Disabled ? DDciIcon::Disabled : DDciIcon::Normal;
}

static inline DDciIconPalette dciPalettle(QPaintDevice *paintDevice = nullptr)
{
    QPalette pa;
    if (!paintDevice || paintDevice->devType() != QInternal::Widget) {
        pa = qApp->palette();
    } else if (QObject *obj = dynamic_cast<QObject *>(paintDevice)) {
        pa = qvariant_cast<QPalette>(obj->property("palette"));
    } else {
        pa = qApp->palette();
    }

    return DDciIconPalette(pa.windowText().color(), pa.window().color(),
                           pa.highlight().color(), pa.highlightedText().color());
}

static inline qreal deviceRadio(QPaintDevice *paintDevice = nullptr)
{
    qreal scale = 1.0;
    if (qApp->testAttribute(Qt::AA_UseHighDpiPixmaps))
        scale = paintDevice ? paintDevice->devicePixelRatioF() : qApp->devicePixelRatio();

    return scale;
}

DDciIconEngine::DDciIconEngine(const QString &iconName)
    : m_iconName(iconName)
    , m_dciIcon(DDciIcon::fromTheme(iconName))
{
}

DDciIconEngine::DDciIconEngine(const DDciIconEngine &other)
    : QIconEngine(other)
    , m_iconName(other.m_iconName)
    , m_dciIcon(other.m_dciIcon)
{

}

DDciIconEngine::~DDciIconEngine()
{

}

QSize DDciIconEngine::actualSize(const QSize &size, QIcon::Mode mode, QIcon::State state)
{
    Q_UNUSED(state);
    int s = m_dciIcon.actualSize(qMin(size.width(), size.height()), dciTheme(), dciMode(mode));
    return QSize(s, s).boundedTo(size);
}

QPixmap DDciIconEngine::pixmap(const QSize &size, QIcon::Mode mode, QIcon::State state)
{
    return pixmap(size, mode, state, 0);
}

QPixmap DDciIconEngine::pixmap(const QSize &size, QIcon::Mode mode, QIcon::State state, qreal radio)
{
    Q_UNUSED(state)
    return m_dciIcon.pixmap(qFuzzyIsNull(radio) ? deviceRadio() : radio,
                            qMin(size.width(), size.height()), dciTheme(),
                            dciMode(mode), dciPalettle());
}

void DDciIconEngine::paint(QPainter *painter, const QRect &rect, QIcon::Mode mode, QIcon::State state)
{
    Q_UNUSED(state);
    m_dciIcon.paint(painter, rect, deviceRadio(painter->device()), dciTheme(),
                    dciMode(mode), Qt::AlignCenter, dciPalettle(painter->device()));
}

QString DDciIconEngine::key() const
{
    return QLatin1String("DDciIconEngine");
}

QIconEngine *DDciIconEngine::clone() const
{
    return new DDciIconEngine(*this);
}

bool DDciIconEngine::read(QDataStream &in)
{
    in >> m_iconName >> m_dciIcon;
    return true;
}

bool DDciIconEngine::write(QDataStream &out) const
{
    out << m_iconName << m_dciIcon;
    return true;
}

QString DDciIconEngine::iconName() const
{
    return m_iconName;
}

void DDciIconEngine::virtual_hook(int id, void *data)
{
    switch (id) {
    case QIconEngine::AvailableSizesHook:
        {
            auto &arg = *reinterpret_cast<QIconEngine::AvailableSizesArgument*>(data);
            auto availableSizes = m_dciIcon.availableSizes(dciTheme(), DDciIcon::Normal);
            QList<QSize> sizes;
            sizes.reserve(availableSizes.size());

            for (int size : availableSizes)
                sizes.append(QSize(size, size));

            arg.sizes.swap(sizes); // commit
        }
        break;
    case QIconEngine::IconNameHook:
        {
            QString &name = *reinterpret_cast<QString*>(data);
            name = iconName();
        }
        break;
    case QIconEngine::IsNullHook:
        {
            *reinterpret_cast<bool*>(data) = m_dciIcon.isNull();
        }
        break;
    case QIconEngine::ScaledPixmapHook:
        {
            QIconEngine::ScaledPixmapArgument &arg = *reinterpret_cast<QIconEngine::ScaledPixmapArgument*>(data);
            arg.pixmap = pixmap(arg.size, arg.mode, arg.state, arg.scale);
        }
        break;
    default:
        QIconEngine::virtual_hook(id, data);
    }
}
