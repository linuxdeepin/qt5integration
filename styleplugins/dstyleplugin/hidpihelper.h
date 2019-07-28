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
