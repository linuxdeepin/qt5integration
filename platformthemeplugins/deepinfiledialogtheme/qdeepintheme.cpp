/*
 * SPDX-FileCopyrightText: 2017 - 2023 UnionTech Software Technology Co., Ltd.
 * SPDX-License-Identifier: LGPL-3.0-or-later
 */
#include "qdeepintheme.h"
#include "qdeepinfiledialoghelper.h"
#include "filedialogmanager_interface.h"

#include <QGuiApplication>
#include <QIconEnginePlugin>

#include <private/qguiapplication_p.h>
#include <qpa/qwindowsysteminterface_p.h>
#include <qpa/qplatformscreen.h>
#include <qpa/qplatformcursor.h>

#undef signals
#include <X11/Xlib.h>


QT_BEGIN_NAMESPACE

const char *QDeepinTheme::name = "deepin";
bool QDeepinTheme::m_usePlatformNativeDialog = true;

QDeepinTheme::QDeepinTheme()
{
}

QDeepinTheme::~QDeepinTheme()
{
    if (QDeepinFileDialogHelper::manager) {
        QDeepinFileDialogHelper::manager->deleteLater();
        QDeepinFileDialogHelper::manager = Q_NULLPTR;
    }
}

bool QDeepinTheme::usePlatformNativeDialog(DialogType type) const
{
    if (type == FileDialog) {
        if (qgetenv("_d_disableDBusFileDialog") == "true")
            return false;

        static bool dbusDialogManagerInitialized = false;

        if (!dbusDialogManagerInitialized) {
            dbusDialogManagerInitialized = true;
            QDeepinFileDialogHelper::initDBusFileDialogManager();
        }

        return m_usePlatformNativeDialog
                && QDeepinFileDialogHelper::manager
                && QDeepinFileDialogHelper::manager->isUseFileChooserDialog();
    }

    return QGenericUnixTheme::usePlatformNativeDialog(type);
}

QPlatformDialogHelper *QDeepinTheme::createPlatformDialogHelper(DialogType type) const
{
    if (type == FileDialog && usePlatformNativeDialog(type))
        return new QDeepinFileDialogHelper();

    return QGenericUnixTheme::createPlatformDialogHelper(type);
}

QT_END_NAMESPACE

#include "qdeepintheme.moc"
