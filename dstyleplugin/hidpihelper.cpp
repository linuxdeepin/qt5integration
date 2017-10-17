#include "hidpihelper.h"

#include <QApplication>
#include <QImageReader>
#include <QIcon>

namespace dstyle {

QPixmap HiDPIHelper::loadPixmap(const QString &fileName)
{
    qreal sourceDevicePixelRatio = 1.0;
    qreal devicePixelRatio = qApp->devicePixelRatio();
    QPixmap pixmap;

    if (!qFuzzyCompare(sourceDevicePixelRatio, devicePixelRatio)) {
        QImageReader reader;
        reader.setFileName(qt_findAtNxFile(fileName, devicePixelRatio, &sourceDevicePixelRatio));
        if (reader.canRead()) {
            reader.setScaledSize(reader.size() * (devicePixelRatio / sourceDevicePixelRatio));
            pixmap = QPixmap::fromImage(reader.read());
            pixmap.setDevicePixelRatio(devicePixelRatio);
        }
    } else {
        pixmap.load(fileName);
    }

    return pixmap;
}

}

