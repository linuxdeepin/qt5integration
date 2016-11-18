#ifndef DICONPROXYENGINE_H
#define DICONPROXYENGINE_H

#include <QIconEngine>

class DIconProxyEngine : public QIconEngine
{
public:
    explicit DIconProxyEngine(const QIcon &proxyIcon);

    void paint(QPainter *painter, const QRect &rect, QIcon::Mode mode, QIcon::State state) Q_DECL_OVERRIDE;
    QPixmap pixmap(const QSize &size, QIcon::Mode mode, QIcon::State state) Q_DECL_OVERRIDE;
    QSize actualSize(const QSize &size, QIcon::Mode mode, QIcon::State state) Q_DECL_OVERRIDE;
    QIconEngine *clone() const Q_DECL_OVERRIDE;
    bool read(QDataStream &in) Q_DECL_OVERRIDE;
    bool write(QDataStream &out) const Q_DECL_OVERRIDE;

private:
    QIcon m_proxyIcon;
};

#endif // DICONPROXYENGINE_H
