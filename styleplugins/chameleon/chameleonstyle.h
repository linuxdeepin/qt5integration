/*
 * SPDX-FileCopyrightText: 2017 - 2024 UnionTech Software Technology Co., Ltd.
 * SPDX-License-Identifier: LGPL-3.0-or-later
 */
#ifndef CHAMELEONSTYLE_H
#define CHAMELEONSTYLE_H

#include <DStyle>
#include <DDciIconPlayer>
#include <QVariantAnimation>

DWIDGET_USE_NAMESPACE

namespace dstyle {
class DStyleAnimation;
}

DWIDGET_BEGIN_NAMESPACE
class DStyleOptionButtonBoxButton;
DWIDGET_END_NAMESPACE

namespace chameleon {
class ChameleonMovementAnimation : public QVariantAnimation
{
    Q_OBJECT

public:
    explicit ChameleonMovementAnimation(QWidget *targetWidget);

    inline QRect currentRect() const {
        return m_currentRect;
    }

    inline bool isRuning() const {
        return state() == QVariantAnimation::Running;
    }

    inline float progress() const {
        return float(currentLoopTime()) / duration();
    }

    QWidget *targetWidget() const;
    void setTargetRect(const QRect &rect);
    void setCurrentRect(const QRect &rect);

private:
    QRect m_currentRect;
    QRect m_targetRect;
    QRect m_lastTargetRect;
};

class ChameleonStyle : public DStyle
{
public:
    explicit ChameleonStyle();

    void drawPrimitive(QStyle::PrimitiveElement pe, const QStyleOption *opt, QPainter *p,
                       const QWidget *w = nullptr) const override;
    void drawControl(QStyle::ControlElement element, const QStyleOption *opt, QPainter *p,
                     const QWidget *w = nullptr) const override;
    QRect subElementRect(QStyle::SubElement r, const QStyleOption *opt, const QWidget *widget = nullptr) const override;
    void drawComplexControl(ComplexControl cc, const QStyleOptionComplex *opt, QPainter *p,
                            const QWidget *w = nullptr) const override;
    SubControl hitTestComplexControl(ComplexControl cc, const QStyleOptionComplex *opt,
                                     const QPoint &pt, const QWidget *w = nullptr) const override;
    QRect subControlRect(ComplexControl cc, const QStyleOptionComplex *opt, SubControl sc,
                         const QWidget *w = nullptr) const override;
    QSize sizeFromContents(QStyle::ContentsType ct, const QStyleOption *opt,
                           const QSize &contentsSize, const QWidget *widget = nullptr) const override;

    int pixelMetric(QStyle::PixelMetric m, const QStyleOption *opt = nullptr, const QWidget *widget = nullptr) const override;

    int styleHint(StyleHint sh, const QStyleOption *opt = nullptr, const QWidget *w = nullptr,
                  QStyleHintReturn *shret = nullptr) const override;

    void polish(QWidget *w) override;
    void unpolish(QWidget *w) override;
    void unpolish(QApplication *application) override;

    QBrush generatedBrush(StateFlags flags, const QBrush &base, QPalette::ColorGroup cg, QPalette::ColorRole role, const QStyleOption *option) const;
    QBrush generatedBrush(StateFlags flags, const QBrush &base, DPalette::ColorGroup cg = DPalette::Normal, DPalette::ColorType type = DPalette::ItemBackground, const QStyleOption *option = nullptr) const;

private:
    void drawShadow(QPainter *p, const QRect &rect, const QColor &color) const;
    void drawBorder(QPainter *p, const QStyleOption *opt, const QWidget *w) const;
    void drawCalenderEllipse(QPainter *p, const QRect &rect, int offset) const;
    void drawButtonBoxButton(const DStyleOptionButtonBoxButton *btnopt, QPainter *p, const QRect &rect, int radius) const;
    void drawRoundedShadow(QPainter *p, const QRect &rect, const QColor &color,int frame_radius) const;
    bool isNoticks(const QStyleOptionSlider *slider, QPainter *p, const QWidget *w) const;

    QColor getColor(const QStyleOption *option, QPalette::ColorRole role) const;
    QColor getColor(const QStyleOption *option, DPalette::ColorType type, const QWidget *widget) const;
    QBrush getBrush(const QStyleOption *option, DPalette::ColorRole type) const;
    QMargins frameExtentMargins() const;
    QRect drawButtonDownArrow(const QStyleOption *opt, QPainter *p, const QWidget *w) const;

    bool drawSpinBox(const QStyleOptionSpinBox *opt, QPainter *p, const QWidget *w) const;
    void updateSpinBoxButtonState(const QStyleOptionSpinBox *opt, QStyleOptionButton& button, bool isActive, bool isEnabled) const;
    bool drawMenuBarItem(const QStyleOptionMenuItem *option, QRect &rect, QPainter *painter, const QWidget *widget) const;
    ChameleonMovementAnimation *drawMenuItemBackground(const QStyleOption *option, QPainter *painter, QStyleOptionMenuItem::MenuItemType type) const;
    bool drawMenuItem(const QStyleOptionMenuItem *option, QPainter *painter, const QWidget *widget) const;
    bool drawTabBar(QPainter *painter ,const QStyleOptionTab *tab, const QWidget *widget) const;
    bool drawTabBarLabel(QPainter *painter ,const QStyleOptionTab *tab, const QWidget *widget) const;
    bool drawTabBarCloseButton(QPainter *painter,const QStyleOption *tab , const QWidget *widget) const;
    bool drawTabBarScrollButton(QPainter *painter, const QStyleOption *opt, const QWidget *buttonWidget) const;
    bool drawComboBox(QPainter *painter, const QStyleOptionComboBox *comboBox, const QWidget *widget) const;
    bool drawComboBoxLabel(QPainter *painter, const QStyleOptionComboBox *comboBox, const QWidget *widget) const;
    void drawSliderHandle(const QStyleOptionComplex *opt, QRectF& rectHandle, QPainter *p, const QWidget *w) const;
    void drawSliderHandleFocus(const QStyleOptionComplex *opt, QRectF& rectHandle, QPainter *p, const QWidget *w) const;
    void drawIcon(const QStyleOption *opt, QPainter *p, QRect &rect, const QIcon &icon, bool checked = false) const;
    void tabLayout(const QStyleOptionTab *opt, const QWidget *widget, QRect *textRect, QRect *iconRect) const;
    bool drawTableViewItem(QStyle::PrimitiveElement pe, const QStyleOption *opt, QPainter *p,
                       const QWidget *w = nullptr) const;
    bool hideScrollBarByAnimation(const QStyleOptionSlider *scrollBar, QPainter *p) const;

public:
#ifndef QT_NO_ANIMATION
    dstyle::DStyleAnimation *animation(const QObject *target) const;
    void startAnimation(dstyle::DStyleAnimation *animation, int delay = 0) const;
    void _q_removeAnimation();
#endif

protected:
    void drawMenuItemRedPoint(const QStyleOptionMenuItem *option, QPainter *painter, const QWidget *widget) const;
    void resetAttribute(QWidget *w, bool polish);

private:
    mutable QHash<const QObject*, dstyle::DStyleAnimation*> animations;
    mutable QHash<const QObject*, DDciIconPlayer* > dciIconPlayers;

    bool eventFilter(QObject *watched, QEvent *event) override;
};
} // namespace chameleon

#endif // CHAMELEONSTYLE_H
