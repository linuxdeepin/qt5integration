// SPDX-FileCopyrightText: 2011 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "ddbustrayicon.h"

#include <QDBusInterface>
#include <QDBusReply>
DDBusTrayIcon::DDBusTrayIcon()
    : QDBusTrayIcon()
{

}

QRect DDBusTrayIcon::geometry() const
{
    // get geometry by dbus with instanceId
    QDBusInterface watcherInter("org.kde.StatusNotifierWatcher", "/StatusNotifierWatcher", "org.kde.StatusNotifierWatcher");

    QDBusReply<QString> hostReply = watcherInter.call("GetHostServiceName");
    if (!hostReply.isValid())
        return QRect();

    QDBusInterface hostInter(hostReply.value(), "/StatusNotifierHost", "org.kde.StatusNotifierHost");
    QDBusReply<QRect> geoReply = hostInter.call("GetSNIGeometry", this->instanceId());
    if (!geoReply.isValid())
        return QRect();

    return geoReply.value();
}
