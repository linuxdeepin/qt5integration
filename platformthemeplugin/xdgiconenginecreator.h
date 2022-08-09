/*
 * SPDX-FileCopyrightText: 2017-2022 UnionTech Software Technology Co., Ltd.  
 * SPDX-License-Identifier: GPL-3.0-or-later
 */
#ifndef XDGICONENGINECREATOR_H
#define XDGICONENGINECREATOR_H

#include <QIconEngine>

class XdgIconEngineCreator
{
public:
    static QIconEngine *create(const QString &name);
};

#endif // XDGICONENGINECREATOR_H
