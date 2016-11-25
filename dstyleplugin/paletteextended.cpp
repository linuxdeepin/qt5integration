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
QDebug operator<<(QDebug deg, const QCss::Selector &selector)
{
    deg << "specificity:" << selector.specificity() << ", pseudoClass:"<< selector.pseudoClass() << ", pseudoElement:" << selector.pseudoElement();

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

QBrush PaletteExtended::brush(PaletteExtended::BrushName name, quint64 type, const QBrush &defaultBrush) const
{
    const QPair<BrushName, quint64> &key = qMakePair(name, type);

    if (m_brushCache.contains(key))
        return m_brushCache.value(key);

    QMetaEnum metaEnum = QMetaEnum::fromType<PaletteExtended::BrushName>();
    QString colorName = metaEnum.valueToKey(name);

    const QStringList &path = colorName.split("_");

    foreach (const QCss::StyleRule &rule, m_brushScheme->nameIndex.values(path.first())) {
        bool eligible = false;

        foreach (const QCss::Selector &selector, rule.selectors) {
            if (selector.pseudoClass() == type) {
                eligible = true;
                break;
            }
        }

        if (!eligible)
            continue;

        foreach (const QCss::Declaration &declaration, rule.declarations) {
            if (declaration.d->property == path.last()) {
                QBrush brush = declaration.brushValue();

                if (brush.style() == Qt::NoBrush) {
                    const QString &uri = declaration.uriValue();

                    if (!uri.isEmpty())
                        brush.setTexture(QPixmap(uri));
                }

                m_brushCache[key] = brush;

                return brush;
            }
        }
    }

    m_brushCache[key] = defaultBrush;

    return defaultBrush;
}

QBrush PaletteExtended::brush(PaletteExtended::BrushName name, bool enabled, bool mouseOver, bool hasFocus, bool sunken, bool flat, const QBrush &defaultBrush) const
{
    PaletteExtended::PseudoClassType extraType = flat ? PaletteExtended::PseudoClass_Flat : PaletteExtended::PseudoClass_Unknown;
    const QBrush &normal = brush(name, flat ? extraType : PaletteExtended::PseudoClass_Unspecified, defaultBrush);

    if (!enabled)
        return brush(name, PaletteExtended::PseudoClass_Disabled | extraType, normal);
    else if (sunken)
        return brush(name, PaletteExtended::PseudoClass_Pressed | extraType, normal);
    else if (mouseOver)
        return brush(name, PaletteExtended::PseudoClass_Hover | extraType, normal);
    else if (hasFocus)
        return brush(name, PaletteExtended::PseudoClass_Focus | extraType, normal);

    return normal;
}

void PaletteExtended::setType(StyleType type)
{
    QFile file;

    if (type == StyleType::StyleDark) {
        file.setFileName(":/brushschemes/ddark.css");
    } else if (type == StyleType::StyleLight) {
        file.setFileName(":/brushschemes/dlight.css");
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
    p.setBrush(QPalette::Button, brush(QPalette_Button));
    p.setBrush(QPalette::ButtonText, brush(QPalette_ButtonText));

    p.setBrush(QPalette::Disabled, QPalette::Base, brush(QPalette_Base, false));
    p.setBrush(QPalette::Disabled, QPalette::Text, brush(QPalette_Text, false));
    p.setBrush(QPalette::Disabled, QPalette::Window, brush(QPalette_Window, false));
    p.setBrush(QPalette::Disabled, QPalette::WindowText, brush(QPalette_WindowText, false));
    p.setBrush(QPalette::Disabled, QPalette::Highlight, brush(QPalette_Highlight, false));
    p.setBrush(QPalette::Disabled, QPalette::HighlightedText, brush(QPalette_HighlightedText, false));
    p.setBrush(QPalette::Disabled, QPalette::Button, brush(QPalette_Button, false));
    p.setBrush(QPalette::Disabled, QPalette::ButtonText, brush(QPalette_ButtonText, false));

    qDebug() << p.brush(QPalette::Disabled, QPalette::Text);
}

}
