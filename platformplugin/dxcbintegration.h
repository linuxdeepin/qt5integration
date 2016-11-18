#ifndef GENERICPLUGIN_H
#define GENERICPLUGIN_H

#include "qxcbintegration.h"

class XcbWindowHook;

class DXcbIntegration : public QXcbIntegration
{
public:
    DXcbIntegration(const QStringList &parameters, int &argc, char **argv);

    QPlatformWindow *createPlatformWindow(QWindow *window) const Q_DECL_OVERRIDE;
    QPlatformBackingStore *createPlatformBackingStore(QWindow *window) const Q_DECL_OVERRIDE;
};


#endif // GENERICPLUGIN_H
