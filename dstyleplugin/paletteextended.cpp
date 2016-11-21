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
    m_brushScheme(new QCss::StyleSheet)
{
    setType(type);
}

PaletteExtended::~PaletteExtended()
{
    delete m_brushScheme;
}

QBrush PaletteExtended::brush(PaletteExtended::BrushName name) const
{
    if (m_brushCache.contains(name))
        return m_brushCache.value(name);

    QMetaEnum metaEnum = QMetaEnum::fromType<PaletteExtended::BrushName>();
    QString colorName = metaEnum.valueToKey(name);

    const QStringList &path = colorName.split("_");
    const QCss::StyleRule &rule = m_brushScheme->nameIndex.value(path.first());

    foreach (const QCss::Declaration &declaration, rule.declarations) {
        if (declaration.d->property == path.last()) {
            const QBrush &brush = declaration.brushValue();
            m_brushCache[name] = brush;

            return brush;
        }
    }

    m_brushCache[name] = Qt::NoBrush;

    return Qt::NoBrush;
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

    parser.parse(m_brushScheme);
}

void PaletteExtended::polish(QPalette &p)
{
    p.setBrush(QPalette::Base, brush(QPalette_Base));
    p.setBrush(QPalette::Text, brush(QPalette_Text));
    p.setBrush(QPalette::Window, brush(QPalette_Window));
    p.setBrush(QPalette::WindowText, brush(QPalette_WindowText));
    p.setBrush(QPalette::Highlight, brush(QPalette_Highlight));
    p.setBrush(QPalette::HighlightedText, brush(QPalette_HighlightedText));
}

}
