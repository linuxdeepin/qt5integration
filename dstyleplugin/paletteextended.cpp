/**
 * Copyright (C) 2016 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#include "paletteextended.h"

#include <QSettings>
#include <QString>
#include <QColor>
#include <QMetaEnum>
#include <QFile>
#include <QDebug>

#include <private/qcssparser_p.h>

QT_BEGIN_NAMESPACE
QDebug operator<<(QDebug deg, const QCss::Value &value)
{
    deg << value.toString();

    return deg;
}
QT_END_NAMESPACE

namespace dstyle {

PaletteExtended::PaletteExtended(StyleType type, QObject *parent) :
    QObject(parent),
    m_colorScheme(new QCss::StyleSheet)
{
    setType(type);
}

PaletteExtended::~PaletteExtended()
{
    delete m_colorScheme;
}

QColor PaletteExtended::color(PaletteExtended::ColorName name) const
{
    if (m_colorCache.contains(name))
        return m_colorCache.value(name);

    QMetaEnum metaEnum = QMetaEnum::fromType<PaletteExtended::ColorName>();
    QString colorName = metaEnum.valueToKey(name);

    const QStringList &path = colorName.split("_");
    const QCss::StyleRule &rule = m_colorScheme->nameIndex.value(path.first());

    foreach (const QCss::Declaration &declaration, rule.declarations) {
        if (declaration.d->property == path.last()) {
            const QColor &color = declaration.colorValue();
            m_colorCache[name] = color;

            return color;
        }
    }

    m_colorCache[name] = QColor();

    return QColor();
}

void PaletteExtended::setType(StyleType type)
{
    QFile file;

    if (type == StyleType::StyleDark) {
        file.setFileName(":/colorschemes/ddark.css");
    } else if (type == StyleType::StyleLight) {
        file.setFileName(":/colorschemes/dlight.css");
    }

    if (!file.open(QIODevice::ReadOnly))
        return;

    QCss::Parser parser(QString::fromLocal8Bit(file.readAll()));

    parser.parse(m_colorScheme);
}

void PaletteExtended::polish(QPalette &p)
{
    p.setColor(QPalette::Base, color(QPalette_Base));
    p.setColor(QPalette::Text, color(QPalette_Text));
    p.setColor(QPalette::Window, color(QPalette_Window));
    p.setColor(QPalette::WindowText, color(QPalette_WindowText));
    p.setColor(QPalette::Highlight, color(QPalette_Highlight));
    p.setColor(QPalette::HighlightedText, color(QPalette_HighlightedText));
}

}
