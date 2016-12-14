#ifndef XCBNATIVEEVENTFILTER_H
#define XCBNATIVEEVENTFILTER_H

#include <QAbstractNativeEventFilter>
#include <QClipboard>

#include <xcb/xproto.h>

QT_BEGIN_NAMESPACE
class QXcbConnection;
QT_END_NAMESPACE

class XcbNativeEventFilter : public QAbstractNativeEventFilter
{
public:
    XcbNativeEventFilter(QXcbConnection *connection);

    QClipboard::Mode clipboardModeForAtom(xcb_atom_t a) const;
    bool nativeEventFilter(const QByteArray &eventType, void *message, long *result) Q_DECL_OVERRIDE;

private:
    QXcbConnection *m_connection;
};

#endif // XCBNATIVEEVENTFILTER_H
