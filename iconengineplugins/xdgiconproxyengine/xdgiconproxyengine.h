/*
 * Copyright (C) 2021 ~ 2021 Deepin Technology Co., Ltd.
 *
 * Author:     Chen Bin <chenbin@uniontech.com>
 *
 * Maintainer: Chen Bin <chenbin@uniontech.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef XDGICONPROXYENGINE_H
#define XDGICONPROXYENGINE_H

#include <QIconEngine>
#if XDG_ICON_VERSION_MAR >= 3
#define private public
#include <private/xdgiconloader/xdgiconloader_p.h>
#undef private
#elif XDG_ICON_VERSION_MAR == 2
//这个版本中的xdgiconloader_p.h定义和qiconloader_p.h有冲突
//只能通过此方式提供创建XdgIconLoaderEngine对象的接口
#include "xdgiconenginecreator.h"
#endif

namespace DEEPIN_XDG_THEME {
enum PaletteType {
    Text,
    Background,
    Highlight,
};
typedef QMap<PaletteType, QString> PALETTE_MAP;
};

class ScalableEntry;
class QIconLoaderEngineEntry;
QT_BEGIN_NAMESPACE
#if XDG_ICON_VERSION_MAR >= 3
class XdgIconProxyEngine : public QIconEngine
{
public:
    XdgIconProxyEngine(XdgIconLoaderEngine *proxy);
    virtual ~XdgIconProxyEngine() override;

    static quint64 entryCacheKey(const ScalableEntry *color_entry, const QIcon::Mode mode, const QIcon::State state);

    QPixmap followColorPixmap(ScalableEntry *color_entry, const QSize &size, QIcon::Mode mode, QIcon::State state);

    QPixmap pixmapByEntry(QIconLoaderEngineEntry *entry, const QSize &size, QIcon::Mode mode, QIcon::State state);
    void paint(QPainter *painter, const QRect &rect, QIcon::Mode mode, QIcon::State state) override;
    QPixmap pixmap(const QSize &size, QIcon::Mode mode, QIcon::State state) override;
    void addPixmap(const QPixmap &pixmap, QIcon::Mode mode, QIcon::State state) override;
    void addFile(const QString &fileName, const QSize &size, QIcon::Mode mode, QIcon::State state) override;
    QString key() const override;
    QSize actualSize(const QSize &size, QIcon::Mode mode, QIcon::State state) override;
    QIconEngine *clone() const override;
    bool read(QDataStream &in) override;
    bool write(QDataStream &out) const override;
    void virtual_hook(int id, void *data) override;

private:
    XdgIconLoaderEngine *engine;
    QHash<quint64, DEEPIN_XDG_THEME::PALETTE_MAP> entryToColorScheme;
    QIcon::Mode lastMode;
};
#endif

QT_END_NAMESPACE
#endif // XDGICONPROXYENGINE_H
