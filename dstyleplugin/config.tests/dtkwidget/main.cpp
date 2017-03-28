#include <dplatformwindowhandle.h>
#include <QWindow>

int main()
{
    QWindow *w = new QWindow();

    DPlatformWindowHandle handle(w);

    handle.setEnableBlurWindow(true);

    return 0;
}
