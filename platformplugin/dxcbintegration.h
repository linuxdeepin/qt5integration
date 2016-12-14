#ifndef GENERICPLUGIN_H
#define GENERICPLUGIN_H

#include "qxcbintegration.h"

class XcbWindowHook;
class XcbNativeEventFilter;

class DXcbIntegration : public QXcbIntegration
{
public:
    DXcbIntegration(const QStringList &parameters, int &argc, char **argv);
    ~DXcbIntegration();

    QPlatformWindow *createPlatformWindow(QWindow *window) const Q_DECL_OVERRIDE;
    QPlatformBackingStore *createPlatformBackingStore(QWindow *window) const Q_DECL_OVERRIDE;
    void initialize() Q_DECL_OVERRIDE;

private:
    XcbNativeEventFilter *m_eventFilter = Q_NULLPTR;
};


#endif // GENERICPLUGIN_H
