/*
 * Copyright (C) 2017 ~ 2017 Deepin Technology Co., Ltd.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef XCBNATIVEEVENTFILTER_H
#define XCBNATIVEEVENTFILTER_H

#include "global.h"

#include <QAbstractNativeEventFilter>
#include <QClipboard>

#include <xcb/xproto.h>

QT_BEGIN_NAMESPACE
class QXcbConnection;
QT_END_NAMESPACE

DPP_BEGIN_NAMESPACE

class XcbNativeEventFilter : public QAbstractNativeEventFilter
{
public:
    XcbNativeEventFilter(QXcbConnection *connection);

    QClipboard::Mode clipboardModeForAtom(xcb_atom_t a) const;
    bool nativeEventFilter(const QByteArray &eventType, void *message, long *result) Q_DECL_OVERRIDE;

private:
    QXcbConnection *m_connection;
};

DPP_END_NAMESPACE

#endif // XCBNATIVEEVENTFILTER_H
