/*
 * SPDX-FileCopyrightText: 2017 - 2022 UnionTech Software Technology Co., Ltd.  
 * SPDX-License-Identifier: GPL-3.0-or-later
 */
#include "qdeepinfiledialoghelper.h"
#include "qdeepintheme.h"

#include "filedialogmanager_interface.h"
#include "filedialog_interface.h"

#include <QDialog>
#include <QEvent>
#include <QWindow>
#include <QDBusObjectPath>
#include <QFileDialog>
#include <QX11Info>
#include <QDebug>
#include <QApplication>
#include <private/qguiapplication_p.h>

#include <X11/Xlib.h>

QT_BEGIN_NAMESPACE
#define DIALOG_SERVICE "com.deepin.filemanager.filedialog"

QList<QUrl> stringList2UrlList(const QStringList &list)
{
    QList<QUrl> urlList;

    for (const QString &str : list)
        urlList << str;

    return urlList;
}

QStringList urlList2StringList(const QList<QUrl> &list)
{
    QStringList stringList;

    for (const QUrl &url : list)
        stringList << url.toString();

    return stringList;
}

DFileDialogManager *QDeepinFileDialogHelper::manager = Q_NULLPTR;

QDeepinFileDialogHelper::QDeepinFileDialogHelper()
{
    connect(this, &QDeepinFileDialogHelper::accept, this, [this] {
        if (sourceDialog && filedlgInterface) {
            const QMap<QString, QVariant> &map_lineedit = filedlgInterface->allCustomWidgetsValue(LineEditType);
            const QMap<QString, QVariant> &map_combobox = filedlgInterface->allCustomWidgetsValue(ComboBoxType);

            auto map_eidt_begin = map_lineedit.constBegin();

            while (map_eidt_begin != map_lineedit.constEnd()) {
                sourceDialog->setProperty(QString("_dtk_widget_lineedit_%1_value").arg(map_eidt_begin.key()).toUtf8(),
                                          map_eidt_begin.value());
                ++map_eidt_begin;
            }

            auto map_combobox_begion = map_combobox.constBegin();

            while (map_combobox_begion != map_combobox.constEnd()) {
                sourceDialog->setProperty(QString("_dtk_widget_combobox_%1_value").arg(map_combobox_begion.key()).toUtf8(),
                                          map_combobox_begion.value());
                ++map_combobox_begion;
            }
        }
    });
}

QDeepinFileDialogHelper::~QDeepinFileDialogHelper()
{
    if (auxiliaryWindow)
        auxiliaryWindow->deleteLater();

    if (filedlgInterface) {
        filedlgInterface->deleteLater(); // dbus
        filedlgInterface->QObject::deleteLater();
    }
}

void QDeepinFileDialogHelper::onApplicationStateChanged(Qt::ApplicationState state)
{
    if (state == Qt::ApplicationActive && filedlgInterface)
        filedlgInterface->activateWindow();
}

void QDeepinFileDialogHelper::onWindowActiveChanged()
{
    if (qApp->platformName() != "dxcb" && !qApp->property("_d_isDxcb").toBool())
        return;

    QWindow *focus_window = qApp->focusWindow();
    if (!focus_window)
        return;

    if (focus_window->type() != Qt::Widget
            && focus_window->type() != Qt::Window
            && focus_window->type() != Qt::Dialog) {
        return;
    }

    if (filedlgInterface && !filedlgInterface->windowActive() && qApp->applicationState() == Qt::ApplicationActive) {
        filedlgInterface->activateWindow();
    }
}

bool QDeepinFileDialogHelper::show(Qt::WindowFlags flags, Qt::WindowModality modality, QWindow *parent)
{
    ensureDialog();
    applyOptions();

    if (filedlgInterface) {
        if (parent)
            activeWindow = parent;
        else
            activeWindow = QGuiApplication::focusWindow();

        filedlgInterface->setParent(parent);

        Qt::WindowFlags nd_flags = static_cast<Qt::WindowFlags>(filedlgInterface->windowFlags());
        Qt::WindowFlags need_flags = Qt::WindowTitleHint | Qt::WindowSystemMenuHint
                                        | Qt::WindowMinMaxButtonsHint | Qt::WindowContextHelpButtonHint
                                        | Qt::WindowStaysOnTopHint | Qt::WindowTransparentForInput
                                        | Qt::WindowDoesNotAcceptFocus | Qt::WindowStaysOnBottomHint
                                        | Qt::WindowCloseButtonHint | Qt::BypassWindowManagerHint;

        if (flags & need_flags)
            filedlgInterface->setWindowFlags(nd_flags | (flags & need_flags));

        static bool i_am_dbus_server = iAmFileDialogDBusServer();

        if (i_am_dbus_server) {
            WId native_dialog_winId = filedlgInterface->winId();
            QWindow *real_native_dialog = nullptr;

            for (QWindow *window : qGuiApp->topLevelWindows()) {
                if (window->winId() == native_dialog_winId) {
                    real_native_dialog = window;
                    break;
                }
            }

            if (real_native_dialog) {
                real_native_dialog->setTransientParent(parent);
                real_native_dialog->setModality(modality);
                // call later
                QMetaObject::invokeMethod(filedlgInterface.data(), "show", Qt::QueuedConnection);

                return true;
            }
        }

        auxiliaryWindow->setParent(parent);
        auxiliaryWindow->setFlags(flags);
        auxiliaryWindow->setModality(modality);

        if (modality != Qt::NonModal) {
            QGuiApplicationPrivate::showModalWindow(auxiliaryWindow);

            if (modality == Qt::ApplicationModal) {
                connect(qApp, &QGuiApplication::applicationStateChanged, this, &QDeepinFileDialogHelper::onApplicationStateChanged);
                connect(filedlgInterface, &DFileDialogHandle::windowActiveChanged, this, &QDeepinFileDialogHelper::onWindowActiveChanged);
            }
        }
    }

    if (filedlgInterface)
        filedlgInterface->show();

    if (filedlgInterface && parent) {
        // 如果能获取到wayland下的display 应该 XSetTransientForHint(wayland_dispaly
        // 此处暂时返回，至少不会导致崩溃。。。task-view-31919.
        if (qApp->platformName() != "dxcb" && !qApp->property("_d_isDxcb").toBool())
            return true;

        XSetTransientForHint(QX11Info::display(), filedlgInterface->winId(), parent->winId());
    }
    // 如果没有dbus接口 return false 至少可以显示默认的文件选择对话框
    return filedlgInterface;
}

void QDeepinFileDialogHelper::exec()
{
    qDebug() << "exec";

    ensureDialog();

    // 快速打开关闭多次有一定概率出现没有调用 show 的情况,这里先 show
    // 保证exec一定会显示出现
    if (filedlgInterface)
        filedlgInterface->show();

    // block input to the window, allow input to other GTK dialogs
    QEventLoop loop;
    connect(this, SIGNAL(accept()), &loop, SLOT(quit()));
    connect(this, SIGNAL(reject()), &loop, SLOT(quit()));
    loop.exec();
}

void QDeepinFileDialogHelper::hide()
{
    qDebug() << "hide";

    ensureDialog();

    if (filedlgInterface)
        filedlgInterface->hide();

    if (auxiliaryWindow) {
        QGuiApplicationPrivate::hideModalWindow(auxiliaryWindow);

        if (activeWindow) {
            activeWindow->requestActivate();
            activeWindow.clear();
        }
    }
}

bool QDeepinFileDialogHelper::defaultNameFilterDisables() const
{
    qDebug() << __FUNCTION__;

    return true;
}

void QDeepinFileDialogHelper::setDirectory(const QUrl &directory)
{
    if (QDeepinFileDialogHelper::directory() == directory) {
        return;
    }

    qDebug() << __FUNCTION__ << directory;

    ensureDialog();

    if (filedlgInterface)
        filedlgInterface->setDirectoryUrl(directory.toString());
    else
        options()->setInitialDirectory(directory);
}

QUrl QDeepinFileDialogHelper::directory() const
{
    qDebug() << __FUNCTION__;

    ensureDialog();

    if (filedlgInterface)
        return QUrl(filedlgInterface->directoryUrl());

    return options()->initialDirectory();
}

void QDeepinFileDialogHelper::selectFile(const QUrl &fileUrl)
{
    qDebug() << __FUNCTION__ << fileUrl;

    ensureDialog();

    if (filedlgInterface)
        filedlgInterface->selectUrl(fileUrl.toString());
    else
        options()->setInitiallySelectedFiles({fileUrl});
}

QList<QUrl> QDeepinFileDialogHelper::selectedFiles() const
{
    qDebug() << __FUNCTION__;

    ensureDialog();

    if (filedlgInterface)
        return stringList2UrlList(filedlgInterface->selectedUrls());

    return options()->initiallySelectedFiles();
}

void QDeepinFileDialogHelper::setFilter()
{
    qDebug() << __FUNCTION__;

    ensureDialog();

    if (filedlgInterface)
        filedlgInterface->setFilter(options()->filter());
}

void QDeepinFileDialogHelper::selectNameFilter(const QString &filter)
{
    qDebug() << __FUNCTION__ << filter;

    ensureDialog();

    if (filedlgInterface)
        filedlgInterface->selectNameFilter(filter);
    else
        options()->setInitiallySelectedNameFilter(filter);
}

QString QDeepinFileDialogHelper::selectedNameFilter() const
{
    qDebug() << __FUNCTION__;

    ensureDialog();

    if (filedlgInterface)
        return filedlgInterface->selectedNameFilter();

    return options()->initiallySelectedNameFilter();
}

void QDeepinFileDialogHelper::initDBusFileDialogManager()
{
    if (manager)
        return;

    if (QDBusConnection::sessionBus().interface()->isServiceRegistered(DIALOG_SERVICE).value()
        || QFile::exists("/usr/bin/dde-desktop")) {
        manager = new DFileDialogManager(DIALOG_SERVICE, "/com/deepin/filemanager/filedialogmanager", QDBusConnection::sessionBus());
    }
}

bool QDeepinFileDialogHelper::iAmFileDialogDBusServer()
{
    static bool _value = manager->connection().interface()->servicePid(manager->service()) == static_cast<uint>(getpid());

    return _value;
}

void QDeepinFileDialogHelper::ensureDialog() const
{
    if (filedlgInterface)
        return;

    if (manager) {
        QDBusPendingReply<QDBusObjectPath> reply = manager->createDialog(QString());
        reply.waitForFinished();
        const QString &path = reply.value().path();

        if (path.isEmpty()) {
            qWarning("Can not create native dialog, Will be use QFileDialog");
        } else {
            filedlgInterface = new DFileDialogHandle(DIALOG_SERVICE, path, QDBusConnection::sessionBus());
            auxiliaryWindow = new QWindow();
            auxiliaryWindow->setObjectName("QDeepinFileDialogHelper_auxiliaryWindow");

            connect(filedlgInterface, &QObject::destroyed, auxiliaryWindow, &QWindow::deleteLater);
            connect(filedlgInterface, &QObject::destroyed, filedlgInterface, &DFileDialogHandle::deleteLater);
            connect(filedlgInterface, &DFileDialogHandle::accepted, this, &QDeepinFileDialogHelper::accept);
            connect(filedlgInterface, &DFileDialogHandle::rejected, this, &QDeepinFileDialogHelper::reject);
            connect(filedlgInterface, &DFileDialogHandle::destroyed, this, &QDeepinFileDialogHelper::reject);
            connect(filedlgInterface, &DFileDialogHandle::destroyed, this, [this](){
                qWarning("filedialog dbus service destroyed.");
                if (filedlgInterface) {
                    filedlgInterface->QObject::deleteLater();
                    filedlgInterface = nullptr;
                }

                if (auxiliaryWindow && auxiliaryWindow->isModal() && qApp->modalWindow() == auxiliaryWindow)
                    QGuiApplicationPrivate::hideModalWindow(auxiliaryWindow);
            });

            QTimer *heartbeatTimer = new QTimer(filedlgInterface);

            connect(heartbeatTimer, &QTimer::timeout, this, [this, heartbeatTimer] {
                QDBusPendingReply<> reply = filedlgInterface->makeHeartbeat();

                reply.waitForFinished();

                if (reply.isError()) {
                    qWarning() << "Make heartbeat is failed:" << reply.error();

                    if (reply.error().type() == QDBusError::UnknownMethod) {
                        qWarning() << "Make heartbeat is't support for current dbus file dialog, Will be stop heartbeat timer.";

                        heartbeatTimer->stop();
                        return;
                    }

                    filedlgInterface->QObject::deleteLater();
                    const_cast<QDeepinFileDialogHelper*>(this)->reject();
                }
            });
            int heartbeatInterval = filedlgInterface->heartbeatInterval();
            heartbeatTimer->setInterval(qMax(1 * 1000, qMin(int(heartbeatInterval / 1.5), heartbeatInterval - 5 * 1000)));
            heartbeatTimer->start();
        }
    }

    if (!filedlgInterface && qobject_cast<QApplication*>(qGuiApp)) {
        QDeepinTheme::m_usePlatformNativeDialog = false;
    }
}

void QDeepinFileDialogHelper::applyOptions()
{
    QFileDialogOptions *options = this->options().data();

    for (int i = 0; i < QFileDialogOptions::DialogLabelCount; ++i) {
        if (options->isLabelExplicitlySet((QFileDialogOptions::DialogLabel)i)) {
            if (filedlgInterface)
                filedlgInterface->setLabelText(i, options->labelText((QFileDialogOptions::DialogLabel)i));
            else
                qWarning() << "DFileDialogHandle invalid!!";

        }
    }
    if (filedlgInterface) {
        filedlgInterface->setOptions(int(options->options()));
        filedlgInterface->setFilter(int(options->filter()));
        filedlgInterface->setWindowTitle(options->windowTitle());
        filedlgInterface->setViewMode(int(options->viewMode()));
        filedlgInterface->setFileMode(int(options->fileMode()));
        filedlgInterface->setAcceptMode(int(options->acceptMode()));
        filedlgInterface->setNameFilters(options->nameFilters());
    }


    if (options->initialDirectory().isLocalFile())
        setDirectory(options->initialDirectory());

    foreach (const QUrl &filename, options->initiallySelectedFiles())
        selectFile(filename);

    selectNameFilter(options->initiallySelectedNameFilter());

    if (!sourceDialog) {
        sourceDialog = reinterpret_cast<QObject*>(qvariant_cast<quintptr>(property("_dtk_widget_QFileDialog")));

        if (sourceDialog && filedlgInterface) {
            const QStringList lineedit_list = sourceDialog->property("_dtk_widget_custom_lineedit_list").toStringList();
            const QStringList combobox_list = sourceDialog->property("_dtk_widget_custom_combobox_list").toStringList();

            filedlgInterface->beginAddCustomWidget();

            for (const QString &i : lineedit_list)
                filedlgInterface->addCustomWidget(LineEditType, i);

            for (const QString &i : combobox_list)
                filedlgInterface->addCustomWidget(ComboBoxType, i);

            filedlgInterface->endAddCustomWidget();

            const QVariant & mixedSelection = sourceDialog->property("_dtk_widget_filedialog_mixed_selection");
            if (mixedSelection.isValid()) {
                filedlgInterface->setAllowMixedSelection(mixedSelection.toBool());
            }
        }
    }
}

QT_END_NAMESPACE
