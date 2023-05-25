// SPDX-FileCopyrightText: 2011 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later
#ifndef DDBUSTRAYICON_H
#define DDBUSTRAYICON_H
#include "../3rdparty/qdbustrayicon_p.h"

class DDBusTrayIcon : public thirdparty::QDBusTrayIcon
{
    Q_OBJECT
public:
    explicit DDBusTrayIcon();

    QRect geometry() const override;

};

#endif // DDBUSTRAYICON_H
