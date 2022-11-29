// SPDX-FileCopyrightText: 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef DDCIICONENGINE_H
#define DDCIICONENGINE_H

#include <DDciIcon>
#include <QIconEngine>
#include <private/qiconloader_p.h>

DGUI_USE_NAMESPACE

class DDciIconEngine : public QIconEngine
{
public:
    DDciIconEngine(const QString &iconName);
    virtual ~DDciIconEngine() override;
    void paint(QPainter *painter, const QRect &rect, QIcon::Mode mode, QIcon::State state) override;
    QSize actualSize(const QSize &size, QIcon::Mode mode, QIcon::State state) override;
    QPixmap pixmap(const QSize &size, QIcon::Mode mode, QIcon::State state) override;
    QPixmap pixmap(const QSize &size, QIcon::Mode mode, QIcon::State state, qreal radio);

    QString key() const override;
    QIconEngine *clone() const override;
    bool read(QDataStream &in) override;
    bool write(QDataStream &out) const override;

    QString iconName() const override;
private:
    bool hasIcon() const;
    void ensureLoaded();
    void virtual_hook(int id, void *data) override;

    DDciIconEngine(const DDciIconEngine &other);
    QString m_iconName;
    DDciIcon m_dciIcon;
    friend class QIconLoader;
};

#endif // DDCIICONENGINE_H
