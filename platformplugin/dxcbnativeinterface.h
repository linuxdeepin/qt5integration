#ifndef DPLATFORMNATIVEINTERFACE_H
#define DPLATFORMNATIVEINTERFACE_H

#include "qxcbnativeinterface.h"

class DXcbNativeInterface : public QXcbNativeInterface
{
public:
    DXcbNativeInterface();

    QFunctionPointer platformFunction(const QByteArray &function) const Q_DECL_OVERRIDE;
};

#endif // DPLATFORMNATIVEINTERFACE_H
