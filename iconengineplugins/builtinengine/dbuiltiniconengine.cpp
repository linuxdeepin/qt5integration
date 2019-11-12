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
#include "dbuiltiniconengine.h"

#include <DGuiApplicationHelper>

#include <QPainter>
#include <QPixmap>
#include <QPixmapCache>
#include <QImageReader>
#include <QDir>
#include <qmath.h>
#include <private/qiconloader_p.h>
#include <private/qguiapplication_p.h>
#include <QDebug>

#define BUILTIN_ICON_PATH ":/icons/deepin/builtin"

DGUI_USE_NAMESPACE

class ImageEntry : public QIconLoaderEngineEntry
{
public:
    enum Type {
        TextType, // 完全跟随画笔前景色变化
        ActionType, // 只在非normal状态下跟随画笔前景色
        IconType // 任务状态都不跟随画笔颜色
    };

    ImageEntry(Type t)
        : type(t)
    {

    }

    QPixmap pixmap(const QSize &size, QIcon::Mode mode, QIcon::State state) override {
        Q_UNUSED(state)

        if (Q_UNLIKELY(!reader.device())) {
            reader.setFileName(filename);
        }

        if (dir.type == QIconDirInfo::Scalable)
            reader.setScaledSize(size);

        QPixmap pm = QPixmap::fromImageReader(&reader);

        if (type == IconType && qobject_cast<QGuiApplication *>(QCoreApplication::instance())) {
            const QPixmap generated = QGuiApplicationPrivate::instance()->applyQIconStyleHelper(mode, pm);
            if (!generated.isNull())
                pm = generated;
        }

        return pm;
    }

    Type type;
    QImageReader reader;
};

class DirImageEntry : public ImageEntry
{
public:
    using ImageEntry::ImageEntry;

    static QString getIconFile(const QString &key, const QDir &dir, const QString &suffix)
    {
        if (dir.exists(key + "." + suffix)) {
            return dir.filePath(key + "." + suffix);
        }

        int _index = key.indexOf('_');

        if (_index > 0) {
            const QString &mode = key.left(_index);

            if (dir.exists(mode + "." + suffix)) {
                return dir.filePath(mode + "." + suffix);
            }

            const QString &state = key.mid(_index);

            if (dir.exists("normal" + state + "." + suffix)) {
                return dir.filePath("normal" + state + "." + suffix);
            }
        }

        return dir.filePath("normal." + suffix);
    }

    QPixmap pixmap(const QSize &size, QIcon::Mode mode, QIcon::State state) override {
        if (iconFileMap.isEmpty()) {
            const QString &suffix = QFileInfo(filename).suffix();
            QDir dir(filename);

            iconFileMap[QIcon::Disabled << 8 | QIcon::On] = getIconFile("disabled_on", dir, suffix);
            iconFileMap[QIcon::Disabled << 8 | QIcon::Off] = getIconFile("disabled_off", dir, suffix);
            iconFileMap[QIcon::Active << 8 | QIcon::On] = getIconFile("active_on", dir, suffix);
            iconFileMap[QIcon::Active << 8 | QIcon::Off] = getIconFile("active_off", dir, suffix);
            iconFileMap[QIcon::Selected << 8 | QIcon::On] = getIconFile("selected_on", dir, suffix);
            iconFileMap[QIcon::Selected << 8 | QIcon::Off] = getIconFile("selected_off", dir, suffix);
            iconFileMap[QIcon::Normal << 8 | QIcon::On] = getIconFile("normal_on", dir, suffix);
            iconFileMap[QIcon::Normal << 8 | QIcon::Off] = getIconFile("normal_off", dir, suffix);
        }

        reader.setFileName(iconFileMap.value(mode << 8 | state));

        return ImageEntry::pixmap(size, mode, state);
    }

    QMap<qint16, QString> iconFileMap;
};

DBuiltinIconEngine::DBuiltinIconEngine(const QString &iconName)
    : m_iconName(iconName)
{

}

DBuiltinIconEngine::DBuiltinIconEngine(const DBuiltinIconEngine &other)
    : QIconEngine(other)
    , m_iconName(other.m_iconName)
    , m_key(0)
{

}

DBuiltinIconEngine::~DBuiltinIconEngine()
{

}

QSize DBuiltinIconEngine::actualSize(const QSize &size, QIcon::Mode mode,
                                 QIcon::State state)
{
    Q_UNUSED(mode);
    Q_UNUSED(state);

    ensureLoaded();

    QIconLoaderEngineEntry *entry = QIconLoaderEngine::entryForSize(m_info, size);
    if (entry) {
        const QIconDirInfo &dir = entry->dir;
        if (dir.type == QIconDirInfo::Scalable) {
            return size;
        } else {
            int result = qMin<int>(dir.size, qMin(size.width(), size.height()));
            return QSize(result, result);
        }
    }
    return QSize(0, 0);
}

QPixmap DBuiltinIconEngine::pixmap(const QSize &size, QIcon::Mode mode,
                               QIcon::State state)
{
    ensureLoaded();

    QIconLoaderEngineEntry *entry = QIconLoaderEngine::entryForSize(m_info, size);
    if (entry)
        return entry->pixmap(size, mode, state);

    return QPixmap();
}

void DBuiltinIconEngine::paint(QPainter *painter, const QRect &rect,
                           QIcon::Mode mode, QIcon::State state)
{
    ensureLoaded();

    QSize pixmapSize = rect.size();
    qreal scale = 1;
    if (painter->device())
        scale = painter->device()->devicePixelRatioF();

    pixmapSize *= scale;

    QIconLoaderEngineEntry *entry = QIconLoaderEngine::entryForSize(m_info, pixmapSize);
    if (!entry)
        return;

    QPixmap pm = entry->pixmap(pixmapSize, mode, state);
    ImageEntry::Type type = static_cast<ImageEntry*>(entry)->type;

    if (type == ImageEntry::TextType || (type == ImageEntry::ActionType && mode != QIcon::Normal)) {
        QPainter pa(&pm);
        pa.setCompositionMode(QPainter::CompositionMode_SourceIn);
        pa.fillRect(pm.rect(), painter->pen().brush());
    }

    pm.setDevicePixelRatio(scale);
    painter->drawPixmap(rect, pm);
}

QString DBuiltinIconEngine::key() const
{
    return QLatin1String("DBuiltinIconEngine");
}

QIconEngine *DBuiltinIconEngine::clone() const
{
    return new DBuiltinIconEngine(*this);
}


bool DBuiltinIconEngine::read(QDataStream &in)
{
    in >> m_iconName;
    return true;
}


bool DBuiltinIconEngine::write(QDataStream &out) const
{
    out << m_iconName;
    return true;
}

QString DBuiltinIconEngine::iconName() const
{
    return m_iconName;
}

QThemeIconInfo DBuiltinIconEngine::loadIcon(const QString &iconName, uint key)
{
    QThemeIconInfo info;
    info.iconName = iconName;

    QString theme_name = (key == DGuiApplicationHelper::DarkType ? "dark" : "light");
    QStringList iconDirList {
        QString("%1/%2/texts").arg(BUILTIN_ICON_PATH, theme_name),
        QString("%1/%2/actions").arg(BUILTIN_ICON_PATH, theme_name),
        QString("%1/%2/icons").arg(BUILTIN_ICON_PATH, theme_name),
        QString("%1/texts").arg(BUILTIN_ICON_PATH),
        QString("%1/actions").arg(BUILTIN_ICON_PATH),
        QString("%1/icons").arg(BUILTIN_ICON_PATH)
    };

    QDir dir;
    dir.setFilter(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot);

    for (int i = 0; i < iconDirList.count(); ++i) {
        dir.setPath(iconDirList.at(i));

        if (!dir.exists()) {
            continue;
        }

        ImageEntry::Type type = static_cast<ImageEntry::Type>(i % 3);

        for (const QFileInfo &icon_file_info : dir.entryInfoList()) {
            const QString &file_name = icon_file_info.fileName();

            if (!file_name.startsWith(iconName))
                continue;

            // 先找到px所在位置
            int size_str_pos = iconName.length() + 1;
            int px_str_pos = file_name.indexOf("px.", size_str_pos + 1);

            if (px_str_pos < 0)
                continue;

            bool ok = false;
            int size = file_name.mid(size_str_pos, px_str_pos - size_str_pos).toInt(&ok);

            if (Q_UNLIKELY(!ok || size <= 0)) {
                continue;
            }

            ImageEntry *entry = icon_file_info.isDir() ? new DirImageEntry(type) : new ImageEntry(type);
            entry->filename = icon_file_info.absoluteFilePath();
            entry->dir.path = icon_file_info.absolutePath();
            entry->dir.size = size;
            entry->dir.type = icon_file_info.suffix().startsWith("svg") ? QIconDirInfo::Scalable : QIconDirInfo::Fixed;
            info.entries.append(entry);
        }

        // 已经找到图标时不再继续
        if (!info.entries.isEmpty()) {
            break;
        }
    }

    return info;
}

bool DBuiltinIconEngine::hasIcon() const
{
    return !m_info.entries.isEmpty();
}

void DBuiltinIconEngine::virtual_hook(int id, void *data)
{
    ensureLoaded();

    switch (id) {
    case QIconEngine::AvailableSizesHook:
        {
            QIconEngine::AvailableSizesArgument &arg
                    = *reinterpret_cast<QIconEngine::AvailableSizesArgument*>(data);
            const int N = m_info.entries.size();
            QList<QSize> sizes;
            sizes.reserve(N);

            // Gets all sizes from the DirectoryInfo entries
            for (int i = 0; i < N; ++i) {
                const QIconLoaderEngineEntry *entry = m_info.entries.at(i);
                int size = entry->dir.size;
                sizes.append(QSize(size, size));
            }
            arg.sizes.swap(sizes); // commit
        }
        break;
    case QIconEngine::IconNameHook:
        {
            QString &name = *reinterpret_cast<QString*>(data);
            name = m_info.iconName;
        }
        break;
    case QIconEngine::IsNullHook:
        {
            *reinterpret_cast<bool*>(data) = m_info.entries.isEmpty();
        }
        break;
    case QIconEngine::ScaledPixmapHook:
        {
            QIconEngine::ScaledPixmapArgument &arg = *reinterpret_cast<QIconEngine::ScaledPixmapArgument*>(data);
            // QIcon::pixmap() multiplies size by the device pixel ratio.
            const int integerScale = qCeil(arg.scale);
            QIconLoaderEngineEntry *entry = QIconLoaderEngine::entryForSize(m_info, arg.size / integerScale, integerScale);
            arg.pixmap = entry ? entry->pixmap(arg.size, arg.mode, arg.state) : QPixmap();
        }
        break;
    default:
        QIconEngine::virtual_hook(id, data);
    }
}

void DBuiltinIconEngine::ensureLoaded()
{
    if (DGuiApplicationHelper::instance()->themeType() == m_key)
        return;

    qDeleteAll(m_info.entries);
    m_info.entries.clear();
    m_info.iconName.clear();

    Q_ASSERT(m_info.entries.size() == 0);
    m_key = DGuiApplicationHelper::instance()->themeType();
    m_info = loadIcon(m_iconName, m_key);
}
