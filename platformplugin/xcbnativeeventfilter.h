/**
 * Copyright (C) 2017 by Deepin Technology Co., Ltd.
 *
 * Use of this source code is governed by GPLv3
 * that can be found in the LICENSE file.
 **/

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
