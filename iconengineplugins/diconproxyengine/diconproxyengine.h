// SPDX-FileCopyrightText: 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef DICONPROXYENGINE_H
#define DICONPROXYENGINE_H

#include <QIconEngine>

class DIconProxyEngine : public QIconEngine
{
public:
    explicit DIconProxyEngine(const QString &iconName);
    virtual ~DIconProxyEngine() override;
    void paint(QPainter *painter, const QRect &rect, QIcon::Mode mode, QIcon::State state) override;
    QSize actualSize(const QSize &size, QIcon::Mode mode, QIcon::State state) override;
    QPixmap pixmap(const QSize &size, QIcon::Mode mode, QIcon::State state) override;

    QString key() const override;
    QIconEngine *clone() const override;
    bool read(QDataStream &in) override;
    bool write(QDataStream &out) const override;

    QString iconName() const override;
    inline QString themeName() const { return m_iconThemeName; }
private:
    void virtual_hook(int id, void *data) override;

    void ensureEngine();
    DIconProxyEngine(const DIconProxyEngine &other);
    QString m_iconName;
    QString m_iconThemeName;
    QIconEngine *m_iconEngine = nullptr;
};

#endif // DICONPROXYENGINE_H
