// SPDX-FileCopyrightText: 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "diconproxyengine.h"

#include <DGuiApplicationHelper>
#include <DPlatformTheme>
#include <DIconTheme>

#include <QIconEnginePlugin>
#include <QPainter>
#include <QPixmap>
#include <QDebug>
#include <QDir>
#include <KIconLoader>
#include <KIconEngine>

#include <private/qiconloader_p.h>
#include <private/qguiapplication_p.h>

DGUI_USE_NAMESPACE

static inline QString iconThemeName()
{
    return DGuiApplicationHelper::instance()->applicationTheme()->iconThemeName();
}

static QIconEnginePlugin *getIconEngineFactory(const QString &key)
{
    static QFactoryLoader loader("org.qt-project.Qt.QIconEngineFactoryInterface",
                                 QLatin1String("/iconengines"), Qt::CaseSensitive);
    int index = loader.indexOf(key);

    if (index != -1) {
        return qobject_cast<QIconEnginePlugin *>(loader.instance(index));
    }

    return nullptr;
}

static QIconEngine *createIconEngineWithKey(const QString &iconName, const QString &key)
{
    QIconEnginePlugin *plugin = getIconEngineFactory(key);
    if (!plugin)
        return nullptr;

    QIconEngine *iconEngine = plugin->create(iconName);
    if (!iconEngine)
        return nullptr;

    if (iconEngine->isNull()) {
        delete iconEngine;
        return nullptr;
    }

    return iconEngine;
}

static inline QIconEngine *createXdgProxyIconEngine(const QString &iconName)
{
    return new KIconEngine(iconName, KIconLoader::global());
//    const QString &key(qEnvironmentVariable("D_PROXY_ICON_ENGINE", QLatin1String("XdgIconProxyEngine")));
//    return createIconEngineWithKey(iconName, key);
}

static inline QIconEngine *createDciIconEngine(const QString &iconName)
{
    return createIconEngineWithKey(iconName, QLatin1String("DDciIconEngine"));
}

static inline QIconEngine *createDBuiltinIconEngine(const QString &iconName)
{
    static QSet<QString> non_builtin_icon_cache;

    if (!non_builtin_icon_cache.contains(iconName)) {
        // 记录下来此种类型的icon为内置图标
        // 因此，此处添加的缓存不考虑更新
        // 优先使用内置图标
        if (QIconEngine *engine = createIconEngineWithKey(iconName, QStringLiteral("DBuiltinIconEngine"))) {
            return engine;
        } else {
            non_builtin_icon_cache.insert(iconName);
        }
    }

    return nullptr;
}

static bool hasDciIcon(const QString iconName, const QString iconThemeName)
{
    QString iconPath;
    if (auto cached = DIconTheme::cached()) {
        iconPath = cached->findDciIconFile(iconName, iconThemeName);
    } else {
        iconPath = DIconTheme::findDciIconFile(iconName, iconThemeName);
    }

    return !iconPath.isEmpty();
}

static inline bool isDciIconEngine(QIconEngine *engine)
{
    return engine ? engine->key() == QLatin1String("DDciIconEngine") : false;
}

DIconProxyEngine::DIconProxyEngine(const QString &iconName)
    : m_iconName(iconName)
{
    ensureEngine();
}

DIconProxyEngine::DIconProxyEngine(const DIconProxyEngine &other)
    : QIconEngine(other)
    , m_iconName(other.m_iconName)
    , m_iconThemeName(other.m_iconThemeName)
    , m_iconEngine(other.m_iconEngine->clone())
{
    ensureEngine();
}

DIconProxyEngine::~DIconProxyEngine()
{
    if (m_iconEngine)
        delete m_iconEngine;
}

QSize DIconProxyEngine::actualSize(const QSize &size, QIcon::Mode mode, QIcon::State state)
{
    ensureEngine();
    return m_iconEngine ? m_iconEngine->actualSize(size, mode, state) :QSize();
}

QPixmap DIconProxyEngine::pixmap(const QSize &size, QIcon::Mode mode, QIcon::State state)
{
    ensureEngine();
    return m_iconEngine ? m_iconEngine->pixmap(size, mode, state) : QPixmap();
}

void DIconProxyEngine::paint(QPainter *painter, const QRect &rect, QIcon::Mode mode, QIcon::State state)
{
    ensureEngine();
    if (m_iconEngine)
        m_iconEngine->paint(painter, rect, mode, state);
}

QString DIconProxyEngine::key() const
{
    return QLatin1String("DIconProxyEngine");
}

QIconEngine *DIconProxyEngine::clone() const
{
    return new DIconProxyEngine(*this);
}

bool DIconProxyEngine::read(QDataStream &in)
{
    // read m_iconName and m_iconThemeName first
    in >> m_iconName >> m_iconThemeName;

    ensureEngine();
    return m_iconEngine ? m_iconEngine->read(in) : false;
}

bool DIconProxyEngine::write(QDataStream &out) const
{
    out << m_iconName << m_iconThemeName;
    return m_iconEngine ? m_iconEngine->write(out) : false;
}

QString DIconProxyEngine::iconName() const
{
    return m_iconName;
}

void DIconProxyEngine::virtual_hook(int id, void *data)
{
    ensureEngine();
    if (m_iconEngine) {
        m_iconEngine->virtual_hook(id, data);
        return;
    }

    switch (id) {
    case QIconEngine::IsNullHook:
        {
            *reinterpret_cast<bool*>(data) = true;
        }
        break;
    default:
        QIconEngine::virtual_hook(id, data);
    }
}

void DIconProxyEngine::ensureEngine()
{
    if (m_iconName.isEmpty())
        return;

    const QString &theme = iconThemeName();
    if (theme == m_iconThemeName && m_iconEngine)
        return;

    static QMap<QString, QSet<QString>> nonCache;
    const auto it = nonCache.find(theme);
    if (it != nonCache.end() && it->contains(m_iconName))
        return;

    if (m_iconEngine) {
        // dci => dci
        // xdg => xdg
        if (!(hasDciIcon(m_iconName, theme) ^ isDciIconEngine(m_iconEngine))) {
            m_iconThemeName = theme;
            return;
        }

         // delete old engine and create a new engine
        delete m_iconEngine;
        m_iconEngine = nullptr;
    }

    // null => dci
    // null => xdg
    // dci  => xdg
    // xdg  => dci
    if (!m_iconEngine) {
        // 1. try create dci iconengine
        // 2. try create builtin iconengine
        // 3. create xdgiconproxyengine
        if (QIconEngine *iconEngine = createDciIconEngine(m_iconName)) {
            m_iconEngine = iconEngine;
        } else if (QIconEngine *iconEngine = createDBuiltinIconEngine(m_iconName)) {
            m_iconEngine = iconEngine;
        } else if (QIconEngine *iconEngine = createXdgProxyIconEngine(m_iconName)) {
            m_iconEngine = iconEngine;
        } else {
            qErrnoWarning("create icon [%s] engine failed.[theme:%s] nonCache[theme].size[%d]",
                          m_iconName.toUtf8().data(),
                          theme.toUtf8().data(), nonCache[theme].size());
            nonCache[theme].insert(m_iconName);
            return;
        }

        m_iconThemeName = theme;
    }
}
