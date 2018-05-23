#ifndef DICONENGINE_H
#define DICONENGINE_H

#include <QIconEngine>

class DIconEngine : public QIconEngine
{
public:
    explicit DIconEngine(const QString &iconName);
    ~DIconEngine();

    void paint(QPainter *painter, const QRect &rect, QIcon::Mode mode, QIcon::State state);

    QString key() const;
    QString iconName() const { return m_iconName; }

    QSize actualSize(const QSize &size, QIcon::Mode mode, QIcon::State state);
    QPixmap pixmap(const QSize &size, QIcon::Mode mode, QIcon::State state);

    QList<QSize> availableSizes(QIcon::Mode mode = QIcon::Normal, QIcon::State state = QIcon::Off) const;
    QIconEngine *clone() const;

private:
    const QString m_iconName;
};

#endif // DICONENGINE_H
