/*
 * SPDX-FileCopyrightText: 2017-2022 UnionTech Software Technology Co., Ltd.  
 * SPDX-License-Identifier: GPL-3.0-or-later
 */
#ifndef HIDPIHELPER_H
#define HIDPIHELPER_H

#include <QPixmap>

namespace dstyle {

class HiDPIHelper
{
public:
    static QPixmap loadPixmap(const QString &fileName, qreal devicePixelRatio = 0);
};

}

#endif // HIDPIHELPER_H
