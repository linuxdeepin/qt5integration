/*
 * SPDX-FileCopyrightText: 2017 - 2022 UnionTech Software Technology Co., Ltd.  
 * SPDX-License-Identifier: LGPL-3.0-or-later
 */
#include "xdgiconenginecreator.h"

#include <private/xdgiconloader/xdgiconloader_p.h>

QIconEngine * XdgIconEngineCreator::create(const QString &name)
{
    return new XdgIconLoaderEngine(name);
}
