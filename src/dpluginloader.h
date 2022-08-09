/*
 * SPDX-FileCopyrightText: 2021-2022 UnionTech Software Technology Co., Ltd.  
 * SPDX-License-Identifier: GPL-3.0-or-later
 */
#ifndef DPLUGINLOADER_H
#define DPLUGINLOADER_H
#include <QCoreApplication>
#include <QFileInfo>
#include <QLibrary>
#include <QTextStream>
#ifdef Q_OS_LINUX
#include <dlfcn.h>
#endif
#include <QPluginLoader>
#include <QDebug>
#include <QLoggingCategory>
#include <QRegularExpression>

#ifdef QT_DEBUG
Q_LOGGING_CATEGORY(lcDPlugin, "dtk.dpluginloader")
#else
Q_LOGGING_CATEGORY(lcDPlugin, "dtk.dpluginloader", QtInfoMsg)
#endif

#define VERSION_STR_SYMBOL "dtkVersionString"

class DPluginLoader
{
public:
    static QString soAbsoluteFilePath()
    {
#ifdef Q_OS_LINUX
        Dl_info info;
        int rc = dladdr(reinterpret_cast<void *>(DPluginLoader::soAbsoluteFilePath), &info);
        if (!rc)
            qFatal("dladdr error: %s", dlerror());

        return QString(info.dli_fname);
#elif
        return QString();
#endif
    }

    static QFunctionPointer resolve(const char *symbol)
    {
#ifdef Q_OS_LINUX
        /**
      * ！！不要使用qt_linux_find_symbol_sys函数去获取符号
      *
      * 在龙芯平台上，qt_linux_find_symbol_sys 无法获取部分已加载动态库的符号，
      * 可能的原因是这个函数对 dlsym 的调用是在 libQt5Core 动态库中，这个库加载的比较早，
      * 有可能是因此导致无法获取比这个库加载更晚的库中的符号(仅为猜测)
      */
        return QFunctionPointer(dlsym(RTLD_DEFAULT, symbol));
#else
        // TODO
        return nullptr;
#endif
    }

    static QString resolveFromPSM(const QString &moduleName)
    {
        QFile f("/proc/self/maps");
        if (!f.open(QIODevice::ReadOnly))
            qFatal("%s", f.errorString().toLocal8Bit().data());

        QString versoinName;
        QByteArray data = f.readAll();
        QTextStream ts(data);
        while (Q_UNLIKELY(!ts.atEnd())) {
            const QString line = ts.readLine();
            const QStringList &maps = line.split(' ', QString::SplitBehavior::SkipEmptyParts);
            if (Q_UNLIKELY(maps.size() < 6))
                continue;

            QFileInfo info(maps.value(5));
            if (!info.fileName().contains(moduleName))
                continue;

            int idx = info.fileName().indexOf(moduleName) + moduleName.length();
            int idx2 = info.fileName().indexOf(QLatin1String(".so"));
            versoinName = info.fileName().mid(idx, idx2 - idx);
            break;
        }
        return versoinName;
    }

    static QString pluginName(const QString &baseName)
    {
        QString pluginName = baseName;
        QFunctionPointer versionStrFunc = resolve(VERSION_STR_SYMBOL);
        if (versionStrFunc) {
            QString versionStr = reinterpret_cast<const char *(*)()>(versionStrFunc)();
            // 5.5.3.01 ==> 5.5
            const QRegularExpression reg(R"(\d+\.\d+)");
            QRegularExpressionMatch rem = reg.match(versionStr);
            if (rem.hasMatch())
                pluginName += rem.captured();
            else if (!versionStr.isEmpty())
                qCDebug(lcDPlugin) << versionStr << "is invalid";
        } else {
            qCDebug(lcDPlugin) << VERSION_STR_SYMBOL << "resolve failed, trying to read self maps";
            QString versoinName = resolveFromPSM(QLatin1String("dtkcore"));
            if (!versoinName.isEmpty())
                pluginName += versoinName;
            else
                qCDebug(lcDPlugin) << "versoinName is empty";
        }

        qCDebug(lcDPlugin) << "baseName:" << baseName << "pluginName:" << pluginName;
        return pluginName;
    }

    template<class FactoryInterface>
    static FactoryInterface *load(const QString &baseName)
    {
        FactoryInterface *infc = nullptr;
        QFileInfo soInfo(soAbsoluteFilePath());
        if (!soInfo.exists()) {
            qCWarning(lcDPlugin) << soInfo.absoluteFilePath() << " not existed!";
            return infc;
        }

        QString realTargetPath = soInfo.absolutePath() + QString("/%1/").arg(BASED_DTK_DIR) + pluginName(baseName) + QLatin1String(".so");
        QPluginLoader pluginLoader(realTargetPath);
        QObject *plugin = pluginLoader.instance();
        if (plugin) {
            qCDebug(lcDPlugin) << realTargetPath << "loaded successfully";

            infc = qobject_cast<FactoryInterface *>(plugin);
            /*
             * Unless unload() was called explicitly, the plugin stays in memory
             * until the application terminates.
             */
            if (!infc) {
                QString msg = QString("qobject_cast to %1 failed").arg(typeid(FactoryInterface).name());
                qCDebug(lcDPlugin) << msg;

                pluginLoader.unload();
            }
        } else {
            qCCritical(lcDPlugin) << pluginLoader.errorString() << realTargetPath << "loaded failed";
        }

        return infc;
    }
};
#endif // DPLUGINLOADER_H
