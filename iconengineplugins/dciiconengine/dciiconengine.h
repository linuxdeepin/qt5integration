// SPDX-FileCopyrightText: 2022 - 2023 UnionTech Software Technology Co., Ltd.
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

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    QString iconName() override;
#else
    QString iconName() const override;
#endif
private:
    void virtual_hook(int id, void *data) override;
    void ensureIconTheme();

    DDciIconEngine(const DDciIconEngine &other);
    QString m_iconName;
    QString m_iconThemeName;
    DDciIcon m_dciIcon;
};

#endif // DDCIICONENGINE_H
