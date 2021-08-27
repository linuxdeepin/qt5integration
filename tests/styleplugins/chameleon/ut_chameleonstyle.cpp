#include <gtest/gtest.h>

#include <DTabBar>
#include <DStyleOptionButton>
#include <QToolButton>
#include <QTableView>
#include <QMenu>
#include <QCalendarWidget>
#include <QTreeView>
#include <QListView>
#include <QLineEdit>
#include <DSpinBox>
#include <QRadioButton>
#include <QCheckBox>

#include "chameleonstyle.h"

DWIDGET_USE_NAMESPACE;
using namespace chameleon;

class TestForStyleHintParam : public ::testing::TestWithParam<int>
{
protected:
    void SetUp() override;
    void TearDown() override;

    ChameleonStyle *style;
    QWidget *testWidget;
};

void TestForStyleHintParam::SetUp()
{
    style = new ChameleonStyle;
    testWidget = new QWidget;
}

void TestForStyleHintParam::TearDown()
{
    delete style;
    delete testWidget;
}

INSTANTIATE_TEST_CASE_P(ChameleonStyle, TestForStyleHintParam, ::testing::Range(int(ChameleonStyle::SH_EtchDisabledText), int(ChameleonStyle::SH_SpinBox_ButtonsInsideFrame)));

TEST_P(TestForStyleHintParam, styleHint)
{
    QStyleOption opt;
    opt.init(testWidget);

    testWidget->setStyle(style);

    int param = GetParam();
    // 测试在调用默认参数时是否会崩溃
    style->styleHint(ChameleonStyle::StyleHint(param));
    // 测试调用有参时是否会崩溃
    style->styleHint(ChameleonStyle::StyleHint(param), &opt, testWidget);
}

class TestForPixelMetricParam : public ::testing::TestWithParam<quint64>
{
protected:
    void SetUp() override;
    void TearDown() override;

    ChameleonStyle *style;
    QWidget *testWidget;
};

void TestForPixelMetricParam::SetUp()
{
    style = new ChameleonStyle;
    testWidget = new QWidget;
}

void TestForPixelMetricParam::TearDown()
{
    delete style;
    delete testWidget;
}

INSTANTIATE_TEST_CASE_P(ChameleonStyle, TestForPixelMetricParam, ::testing::Range(quint64(ChameleonStyle::PM_FocusBorderWidth), quint64(ChameleonStyle::PM_ButtonMinimizedSize)));

TEST_P(TestForPixelMetricParam, pixelMetric)
{
    QStyleOption opt;
    opt.init(testWidget);

    testWidget->setStyle(style);

    quint64 param = GetParam();
    // 测试在调用默认参数时是否有潜在崩溃风险
    style->pixelMetric(QStyle::PixelMetric(param));
    // 测试调用有参时是否有潜在崩溃风险
    style->pixelMetric(QStyle::PixelMetric(param), &opt, testWidget);
}

class TestForQtDrawControlParam : public ::testing::TestWithParam<int>
{
protected:
    void SetUp() override;
    void TearDown() override;

    ChameleonStyle *style;
    QWidget *testWidget;
};

INSTANTIATE_TEST_CASE_P(ChameleonStyle, TestForQtDrawControlParam, ::testing::Range(int(QStyle::CE_PushButton), int(QStyle::CE_ShapedFrame)));

TEST_P(TestForQtDrawControlParam, drawControl)
{
    QStyleOption opt;
    opt.init(testWidget);

    // 测试Qt原生drawControl参数调用时是否有潜在崩溃风险
    QPixmap surface(QSize(200, 200));
    QPainter painter(&surface);
    int param = GetParam();

    // DStyle中存在的元素值在DTKWIDGET中使用 这里不再测试调用
    style->drawControl(QStyle::ControlElement(param), &opt, &painter);
}

void TestForQtDrawControlParam::SetUp()
{
    style = new ChameleonStyle;
    testWidget = new QWidget;
}

void TestForQtDrawControlParam::TearDown()
{
    delete style;
    delete testWidget;
}

struct DrawComplexControl_Param {
    explicit DrawComplexControl_Param(QStyle::ComplexControl cc, QStyleOptionComplex *opt)
        : cc(cc)
        , opt(opt)
    {
    }

    QStyle::ComplexControl cc;
    QSharedPointer<QStyleOptionComplex> opt;
};

class TestForQtDrawComplexControlParam : public ::testing::TestWithParam<DrawComplexControl_Param>
{
protected:
    void SetUp() override;
    void TearDown() override;

    ChameleonStyle *style;
    QWidget *testWidget;
};

INSTANTIATE_TEST_CASE_P(ChameleonStyle, TestForQtDrawComplexControlParam, ::testing::Values(DrawComplexControl_Param(QStyle::CC_SpinBox, new QStyleOptionSpinBox), DrawComplexControl_Param(QStyle::CC_ToolButton, new QStyleOptionToolButton), DrawComplexControl_Param(QStyle::CC_Slider, new QStyleOptionSlider), DrawComplexControl_Param(QStyle::CC_ComboBox, new QStyleOptionComboBox)));

TEST_P(TestForQtDrawComplexControlParam, drawComplexControl)
{
    DrawComplexControl_Param param = GetParam();

    QPixmap surface(QSize(200, 200));
    QPainter p(&surface);

    param.opt->init(testWidget);
    // 测试调用默认参数时函数是否会崩溃
    style->drawComplexControl(param.cc, param.opt.data(), &p);
}

void TestForQtDrawComplexControlParam::SetUp()
{
    style = new ChameleonStyle;
    testWidget = new QWidget;
}

void TestForQtDrawComplexControlParam::TearDown()
{
    delete style;
    delete testWidget;
}

class TestForQtStandardPixmapParam : public ::testing::TestWithParam<int>
{
};

INSTANTIATE_TEST_CASE_P(ChameleonStyle, TestForQtStandardPixmapParam, ::testing::Range(int(QStyle::SP_TitleBarMenuButton), int(QStyle::SP_LineEditClearButton)));

TEST_P(TestForQtStandardPixmapParam, StandardPixmap)
{
    ChameleonStyle *style = new ChameleonStyle;
    int param = GetParam();

    qDebug() << "QStyle::standardIcon:" << param << " isNull: " << (style->standardIcon(QStyle::StandardPixmap(param)).isNull());
    qDebug() << "QStyle::standardPixmap:" << param << " isNull: " << (style->standardPixmap(QStyle::StandardPixmap(param)).isNull());
    delete style;
}

class TestForDrawUtil : public ::testing::Test
{
protected:
    void SetUp();
    void TearDown();
    bool testPixmapHasData();

    ChameleonStyle *style;
    QWidget *testWidget;
    QPixmap pixmap;
};

void TestForDrawUtil::SetUp()
{
    style = new ChameleonStyle;
    testWidget = new QWidget;

    pixmap = QPixmap(QSize(200, 200));
    // 以异色作为初始颜色防止默认随机颜色的干扰
    pixmap.fill(Qt::green);
}

void TestForDrawUtil::TearDown()
{
    delete style;
    delete testWidget;
}

bool TestForDrawUtil::testPixmapHasData()
{
    QImage image = pixmap.toImage();

    image.reinterpretAsFormat(QImage::Format_RGB32);
    const QRgb *bits = reinterpret_cast<const QRgb *>(image.constBits());
    const QRgb *end = bits + image.byteCount() / sizeof(QRgb);
    return !std::all_of(bits, end, [](QRgb r) { return r == QColor(Qt::green).rgb(); });
}

TEST_F(TestForDrawUtil, drawShadow)
{
    QPainter p(&pixmap);

    // 测试调用是否存在异常崩溃并且测试函数的绘制操作是否有效
    style->drawShadow(&p, QRect(0, 0, 20, 20), Qt::black);
    ASSERT_FALSE(pixmap.isNull());
    ASSERT_TRUE(testPixmapHasData());
}

TEST_F(TestForDrawUtil, drawBorder)
{
    QPainter p(&pixmap);
    QStyleOption opt;
    opt.init(testWidget);

    // 测试调用是否存在异常崩溃并且测试函数的绘制操作是否有效
    style->drawBorder(&p, &opt, testWidget);
    ASSERT_FALSE(pixmap.isNull());
    ASSERT_TRUE(testPixmapHasData());
}

TEST_F(TestForDrawUtil, drawCalenderEllipse)
{
    QPainter p(&pixmap);

    // 测试绘制操作是否存在数据
    style->drawCalenderEllipse(&p, QRect(0, 0, 50, 50), 5);
    ASSERT_FALSE(pixmap.isNull());
    ASSERT_TRUE(testPixmapHasData());
}

TEST_F(TestForDrawUtil, drawButtonBoxButton)
{
    // 多场景触发绘制 测试数据是否存在 并 异于初始数据
    QPainter p(&pixmap);
    DStyleOptionButtonBoxButton button1;
    button1.init(testWidget);
    button1.orientation = Qt::Horizontal;
    button1.position = DStyleOptionButtonBoxButton::Beginning;

    style->drawButtonBoxButton(&button1, &p, QRect(0, 0, 50, 50), 5);
    ASSERT_FALSE(pixmap.isNull());
    ASSERT_TRUE(testPixmapHasData());

    pixmap.fill(Qt::green);

    DStyleOptionButtonBoxButton button2;
    button2.init(testWidget);
    button2.orientation = Qt::Horizontal;
    button2.position = DStyleOptionButtonBoxButton::Middle;
    style->drawButtonBoxButton(&button2, &p, QRect(0, 0, 50, 50), 5);
    ASSERT_TRUE(testPixmapHasData());

    pixmap.fill(Qt::green);

    DStyleOptionButtonBoxButton button3;
    button3.init(testWidget);
    button3.orientation = Qt::Horizontal;
    button3.position = DStyleOptionButtonBoxButton::End;
    style->drawButtonBoxButton(&button3, &p, QRect(0, 0, 50, 50), 5);
    ASSERT_TRUE(testPixmapHasData());

    pixmap.fill(Qt::green);

    DStyleOptionButtonBoxButton button4;
    button4.init(testWidget);
    button4.orientation = Qt::Horizontal;
    button4.position = DStyleOptionButtonBoxButton::OnlyOne;
    style->drawButtonBoxButton(&button4, &p, QRect(0, 0, 50, 50), 5);
    ASSERT_TRUE(testPixmapHasData());

    pixmap.fill(Qt::green);

    DStyleOptionButtonBoxButton button5;
    button5.init(testWidget);
    button5.orientation = Qt::Vertical;
    button5.position = DStyleOptionButtonBoxButton::Beginning;
    style->drawButtonBoxButton(&button5, &p, QRect(0, 0, 50, 50), 5);
    ASSERT_TRUE(testPixmapHasData());
}

TEST_F(TestForDrawUtil, drawButtonDownArrow)
{
    testWidget->resize(200, 200);

    QPainter p(&pixmap);
    QStyleOptionButton opt;
    opt.init(testWidget);

    style->drawButtonDownArrow(&opt, &p, testWidget);
    ASSERT_TRUE(testPixmapHasData());
}

TEST_F(TestForDrawUtil, drawSpinBox)
{
    testWidget->resize(200, 200);
    testWidget->setProperty("_d_dtk_spinBox", true);
    QPainter p(&pixmap);

    QStyleOptionSpinBox opt;
    opt.init(testWidget);
    opt.buttonSymbols = QAbstractSpinBox::PlusMinus;

    style->drawSpinBox(&opt, &p, testWidget);
    ASSERT_FALSE(pixmap.isNull());
    ASSERT_TRUE(testPixmapHasData());

    pixmap.fill(Qt::green);

    QStyleOptionSpinBox opt1;
    opt.init(testWidget);
    opt.buttonSymbols = QAbstractSpinBox::UpDownArrows;

    style->drawSpinBox(&opt1, &p, testWidget);
    ASSERT_FALSE(pixmap.isNull());
    ASSERT_TRUE(testPixmapHasData());
}

TEST_F(TestForDrawUtil, drawMenuBarItem)
{
    testWidget->resize(200, 200);
    QPainter p(&pixmap);

    QStyleOptionMenuItem opt;
    opt.init(testWidget);
    opt.menuItemType = QStyleOptionMenuItem::Margin;
    opt.checkType = QStyleOptionMenuItem::Exclusive;
    opt.text = "测试1";
    opt.icon = style->standardIcon(QStyle::SP_DirIcon);
    opt.menuRect = QRect(20, 20, 60, 40);
    opt.maxIconWidth = 20;
    opt.tabWidth = 2;

    QRect ret;
    style->drawMenuBarItem(&opt, ret, &p, testWidget);
    ASSERT_FALSE(pixmap.isNull());
    ASSERT_TRUE(testPixmapHasData());

    pixmap.fill(Qt::green);

    QStyleOptionMenuItem opt1;
    opt1.init(testWidget);
    opt1.menuItemType = QStyleOptionMenuItem::Normal;
    opt1.checkType = QStyleOptionMenuItem::Exclusive;
    opt1.text = "测试2";
    opt1.icon = style->standardIcon(QStyle::SP_DirIcon);
    opt1.menuRect = QRect(20, 20, 60, 40);
    opt1.maxIconWidth = 20;
    opt1.tabWidth = 2;
    opt1.state |= QStyle::State_MouseOver;

    style->drawMenuBarItem(&opt1, ret, &p, testWidget);
    ASSERT_FALSE(pixmap.isNull());
    ASSERT_TRUE(testPixmapHasData());
}

TEST_F(TestForDrawUtil, drawMenuItemBackground)
{
    testWidget->resize(200, 200);
    QPainter p(&pixmap);

    QStyleOption opt;
    opt.init(testWidget);
    opt.state |= QStyle::State_Selected;

    style->drawMenuItemBackground(&opt, &p, QStyleOptionMenuItem::Normal);
    ASSERT_FALSE(pixmap.isNull());
    ASSERT_TRUE(testPixmapHasData());

    pixmap.fill(Qt::green);

    QStyleOption opt1;
    opt1.init(testWidget);
    opt1.state |= QStyle::State_MouseOver;

    style->drawMenuItemBackground(&opt, &p, QStyleOptionMenuItem::DefaultItem);
    ASSERT_FALSE(pixmap.isNull());
    ASSERT_TRUE(testPixmapHasData());

    pixmap.fill(Qt::green);

    QStyleOption opt2;
    opt2.init(testWidget);
    opt2.state |= QStyle::State_Sunken;

    style->drawMenuItemBackground(&opt, &p, QStyleOptionMenuItem::SubMenu);
    ASSERT_FALSE(pixmap.isNull());
    ASSERT_TRUE(testPixmapHasData());
}

TEST_F(TestForDrawUtil, drawMenuItem)
{
    testWidget->resize(200, 200);

    QPainter p(&pixmap);

    QStyleOptionMenuItem opt;
    opt.init(testWidget);
    opt.menuItemType = QStyleOptionMenuItem::Normal;

    style->drawMenuItem(&opt, &p, testWidget);
    ASSERT_FALSE(pixmap.isNull());
    ASSERT_TRUE(testPixmapHasData());

    pixmap.fill(Qt::green);

    QStyleOptionMenuItem opt1;
    opt1.init(testWidget);
    opt1.menuItemType = QStyleOptionMenuItem::Separator;

    style->drawMenuItem(&opt1, &p, testWidget);
    ASSERT_FALSE(pixmap.isNull());
    ASSERT_TRUE(testPixmapHasData());

    pixmap.fill(Qt::green);

    QStyleOptionMenuItem opt2;
    opt2.init(testWidget);
    opt2.menuItemType = QStyleOptionMenuItem::SubMenu;

    style->drawMenuItem(&opt2, &p, testWidget);
    ASSERT_FALSE(pixmap.isNull());
    ASSERT_TRUE(testPixmapHasData());
}

TEST_F(TestForDrawUtil, drawTabBar)
{
    QTabBar *tabbar = new QTabBar;
    tabbar->resize(200, 200);

    testWidget->resize(200, 200);
    QPainter p(&pixmap);

    QStyleOptionTab tab;
    tab.init(tabbar);
    tab.state |= (QStyle::State_Selected | QStyle::State_Enabled);
    tab.shape = QTabBar::RoundedEast;
    tab.position = QStyleOptionTab::OnlyOneTab;
    tab.selectedPosition = QStyleOptionTab::NotAdjacent;
    tab.cornerWidgets = QStyleOptionTab::NoCornerWidgets;
    tab.text = "测试1";

    style->drawTabBar(&p, &tab, tabbar);
    ASSERT_FALSE(pixmap.isNull());
    ASSERT_TRUE(testPixmapHasData());

    pixmap.fill(Qt::green);

    QStyleOptionTab tab1;
    tab1.init(testWidget);
    tab1.state |= QStyle::State_Enabled;
    tab1.shape = QTabBar::TriangularNorth;
    tab1.position = QStyleOptionTab::Middle;
    tab1.selectedPosition = QStyleOptionTab::NextIsSelected;
    tab1.cornerWidgets = QStyleOptionTab::RightCornerWidget;
    tab1.text = "测试2";
    tab1.icon = style->standardIcon(QStyle::SP_DirIcon);
    tab1.iconSize = QSize(16, 16);

    style->drawTabBar(&p, &tab, tabbar);
    ASSERT_FALSE(pixmap.isNull());
    ASSERT_TRUE(testPixmapHasData());

    pixmap.fill(Qt::green);

    QStyleOptionTab tab2;
    tab2.init(testWidget);
    tab2.state |= QStyle::State_Enabled;
    tab.state |= QStyle::State_Enabled;
    tab2.shape = QTabBar::RoundedWest;
    tab2.position = QStyleOptionTab::Beginning;
    tab2.selectedPosition = QStyleOptionTab::PreviousIsSelected;
    tab2.cornerWidgets = QStyleOptionTab::NoCornerWidgets;
    tab2.text = "测试3";
    tab2.icon = style->standardIcon(QStyle::SP_DirIcon);
    tab2.iconSize = QSize(16, 16);

    style->drawTabBar(&p, &tab, tabbar);
    ASSERT_FALSE(pixmap.isNull());
    ASSERT_TRUE(testPixmapHasData());
}

TEST_F(TestForDrawUtil, drawTabBarLabel)
{
    testWidget->resize(200, 200);
    testWidget->setProperty("_d_dtk_tabbartab_type", true);

    QPainter p(&pixmap);

    QStyleOptionTab tab;
    tab.init(testWidget);
    tab.state |= (QStyle::State_Selected | QStyle::State_Enabled);
    tab.shape = QTabBar::RoundedEast;
    tab.position = QStyleOptionTab::OnlyOneTab;
    tab.selectedPosition = QStyleOptionTab::NotAdjacent;
    tab.cornerWidgets = QStyleOptionTab::NoCornerWidgets;
    tab.text = "测试1";

    style->drawTabBarLabel(&p, &tab, testWidget);
    ASSERT_FALSE(pixmap.isNull());
    ASSERT_TRUE(testPixmapHasData());

    pixmap.fill(Qt::green);

    QStyleOptionTab tab1;
    tab1.init(testWidget);
    tab1.state |= QStyle::State_Enabled;
    tab1.shape = QTabBar::TriangularNorth;
    tab1.text = "测试2";

    style->drawTabBarLabel(&p, &tab, testWidget);
    ASSERT_FALSE(pixmap.isNull());
    ASSERT_TRUE(testPixmapHasData());

    pixmap.fill(Qt::green);

    DTabBar *dtabbar = new DTabBar;
    dtabbar->setProperty("_d_dtk_tabbartab_type", true);
    dtabbar->setProperty("_d_dtk_tabbar_alignment", 0);

    QStyleOptionTab tab2;
    tab2.init(testWidget);
    tab2.state |= (QStyle::State_Selected | QStyle::State_Enabled);
    tab2.shape = QTabBar::TriangularNorth;
    tab2.position = QStyleOptionTab::OnlyOneTab;
    tab2.selectedPosition = QStyleOptionTab::NotAdjacent;
    tab2.cornerWidgets = QStyleOptionTab::NoCornerWidgets;
    tab2.text = "测试3";

    style->drawTabBarLabel(&p, &tab, testWidget);
    ASSERT_FALSE(pixmap.isNull());
    ASSERT_TRUE(testPixmapHasData());

    dtabbar->setProperty("_d_dtk_tabbar_alignment", 1);

    QStyleOptionTab tab3;
    tab2.init(testWidget);
    tab2.state |= (QStyle::State_Selected | QStyle::State_Enabled);
    tab2.shape = QTabBar::TriangularWest;
    tab2.position = QStyleOptionTab::OnlyOneTab;
    tab2.selectedPosition = QStyleOptionTab::NotAdjacent;
    tab2.cornerWidgets = QStyleOptionTab::NoCornerWidgets;
    tab2.text = "测试4";

    style->drawTabBarLabel(&p, &tab, testWidget);
    ASSERT_FALSE(pixmap.isNull());
    ASSERT_TRUE(testPixmapHasData());
}

TEST_F(TestForDrawUtil, drawTabBarCloseButton)
{
    QPainter p(&pixmap);

    QTabBar *tabbar = new QTabBar;
    tabbar->addTab("测试1");
    tabbar->addTab("测试2");
    tabbar->addTab("测试3");
    tabbar->resize(200, 200);
    QWidget *btn1 = new QWidget(tabbar);
    btn1->resize(20, 20);

    QWidget *btn2 = new QWidget(tabbar);
    btn2->resize(20, 20);

    tabbar->setTabButton(0, QTabBar::LeftSide, btn1);
    tabbar->setTabButton(1, QTabBar::RightSide, btn2);

    QStyleOption opt1;
    opt1.init(btn1);
    opt1.state = QStyle::State_Enabled | QStyle::State_Selected;

    style->drawTabBarCloseButton(&p, &opt1, btn1);
    ASSERT_FALSE(pixmap.isNull());
    ASSERT_TRUE(testPixmapHasData());

    style->drawTabBarCloseButton(&p, &opt1, btn2);
    ASSERT_FALSE(pixmap.isNull());
    ASSERT_TRUE(testPixmapHasData());
}

TEST_F(TestForDrawUtil, drawTabBarScrollButton)
{
    QPainter p(&pixmap);

    QTabBar *tabbar = new QTabBar;
    QToolButton *toolButton = new QToolButton(tabbar);
    toolButton->setArrowType(Qt::DownArrow);

    QStyleOptionToolButton opt;
    opt.init(toolButton);
    opt.text = "测试1";
    opt.arrowType = Qt::DownArrow;

    style->drawTabBarScrollButton(&p, &opt, toolButton);
    ASSERT_FALSE(pixmap.isNull());
    ASSERT_TRUE(testPixmapHasData());

    pixmap.fill(Qt::green);

    toolButton->setArrowType(Qt::UpArrow);
    opt.arrowType = Qt::UpArrow;

    tabbar->setShape(QTabBar::TriangularEast);
    style->drawTabBarScrollButton(&p, &opt, toolButton);
    ASSERT_FALSE(pixmap.isNull());
    ASSERT_TRUE(testPixmapHasData());

    pixmap.fill(Qt::green);

    toolButton->setArrowType(Qt::LeftArrow);
    opt.arrowType = Qt::LeftArrow;

    tabbar->setShape(QTabBar::TriangularEast);
    style->drawTabBarScrollButton(&p, &opt, toolButton);
    ASSERT_FALSE(pixmap.isNull());
    ASSERT_TRUE(testPixmapHasData());

    pixmap.fill(Qt::green);

    toolButton->setArrowType(Qt::RightArrow);
    opt.arrowType = Qt::RightArrow;

    tabbar->setShape(QTabBar::TriangularEast);
    style->drawTabBarScrollButton(&p, &opt, toolButton);
    ASSERT_FALSE(pixmap.isNull());
    ASSERT_TRUE(testPixmapHasData());
}

TEST_F(TestForDrawUtil, drawComboBox)
{
    testWidget->resize(200, 200);

    QPainter p(&pixmap);

    QStyleOptionComboBox opt;
    opt.init(testWidget);
    opt.editable = true;
    opt.popupRect = QRect(0, 0, 30, 30);
    opt.frame = true;
    opt.currentText = "测试1";
    opt.currentIcon = style->standardIcon(QStyle::SP_DirIcon);
    opt.iconSize = QSize(16, 16);
    opt.state = QStyle::State_Enabled | QStyle::State_MouseOver;

    style->drawComboBox(&p, &opt, testWidget);
    ASSERT_FALSE(pixmap.isNull());
    ASSERT_TRUE(testPixmapHasData());

    pixmap.fill(Qt::green);

    opt.editable = false;
    opt.frame = false;
    opt.currentText = "测试2";
    opt.currentIcon = style->standardIcon(QStyle::SP_DirIcon);
    opt.iconSize = QSize(16, 16);
    opt.subControls |= QStyle::SC_ComboBoxArrow | QStyle::SC_ComboBoxFrame | QStyle::SC_ComboBoxEditField | QStyle::SC_ComboBoxListBoxPopup;
    style->drawComboBox(&p, &opt, testWidget);
    ASSERT_FALSE(pixmap.isNull());
    ASSERT_TRUE(testPixmapHasData());

    pixmap.fill(Qt::green);

    opt.currentText = "测试3";
    opt.currentIcon = style->standardIcon(QStyle::SP_DirIcon);
    opt.iconSize = QSize(16, 16);
    opt.subControls |= QStyle::SC_ComboBoxArrow | QStyle::SC_ComboBoxFrame;
    style->drawComboBox(&p, &opt, testWidget);
    ASSERT_FALSE(pixmap.isNull());
    ASSERT_TRUE(testPixmapHasData());

    pixmap.fill(Qt::green);

    opt.currentText = "测试3";
    opt.currentIcon = style->standardIcon(QStyle::SP_DirIcon);
    opt.iconSize = QSize(16, 16);
    opt.state = QStyle::State_Enabled | QStyle::State_Selected | QStyle::State_HasFocus;
    opt.subControls |= QStyle::SC_ComboBoxEditField | QStyle::SC_ComboBoxListBoxPopup;

    style->drawComboBox(&p, &opt, testWidget);
    ASSERT_FALSE(pixmap.isNull());
    ASSERT_TRUE(testPixmapHasData());
}

TEST_F(TestForDrawUtil, drawComboBoxLabel)
{
    testWidget->resize(200, 200);

    QPainter p(&pixmap);

    QStyleOptionComboBox opt;
    opt.init(testWidget);
    opt.editable = true;
    opt.popupRect = QRect(0, 0, 30, 30);
    opt.frame = true;
    opt.currentText = "测试1";
    opt.currentIcon = style->standardIcon(QStyle::SP_DirIcon);
    opt.iconSize = QSize(16, 16);
    opt.state = QStyle::State_Enabled | QStyle::State_MouseOver;

    style->drawComboBoxLabel(&p, &opt, testWidget);
    ASSERT_FALSE(pixmap.isNull());
    ASSERT_TRUE(testPixmapHasData());

    pixmap.fill(Qt::green);

    opt.editable = false;
    opt.frame = false;
    opt.currentText = "测试2";
    opt.subControls |= QStyle::SC_ComboBoxArrow | QStyle::SC_ComboBoxFrame | QStyle::SC_ComboBoxEditField | QStyle::SC_ComboBoxListBoxPopup;
    style->drawComboBoxLabel(&p, &opt, testWidget);
    ASSERT_FALSE(pixmap.isNull());
    ASSERT_TRUE(testPixmapHasData());

    pixmap.fill(Qt::green);

    opt.rect = QRect(20, 20, 100, 50);
    opt.currentText = "测试3";
    opt.currentIcon = {};
    opt.iconSize = {};
    opt.subControls |= QStyle::SC_ComboBoxArrow | QStyle::SC_ComboBoxFrame;
    opt.state = QStyle::State_Enabled | QStyle::State_Sunken;
    style->drawComboBoxLabel(&p, &opt, testWidget);
    ASSERT_FALSE(pixmap.isNull());
    ASSERT_TRUE(testPixmapHasData());

    pixmap.fill(Qt::green);

    opt.currentText = "测试3";
    opt.currentIcon = style->standardIcon(QStyle::SP_DirIcon);
    opt.iconSize = QSize(16, 16);
    opt.state = QStyle::State_Enabled | QStyle::State_Selected | QStyle::State_HasFocus;
    opt.subControls |= QStyle::SC_ComboBoxEditField | QStyle::SC_ComboBoxListBoxPopup;

    style->drawComboBoxLabel(&p, &opt, testWidget);
    ASSERT_FALSE(pixmap.isNull());
    ASSERT_TRUE(testPixmapHasData());
}

TEST_F(TestForDrawUtil, drawSliderHandle)
{
    testWidget->resize(200, 200);

    QPainter p(&pixmap);
    QRectF rect(0, 0, 50, 50);

    QStyleOptionSlider slider1;
    slider1.init(testWidget);
    slider1.tickPosition = QSlider::NoTicks;

    style->drawSliderHandle(&slider1, rect, &p, testWidget);
    ASSERT_FALSE(pixmap.isNull());
    ASSERT_TRUE(testPixmapHasData());
    pixmap.fill(Qt::green);

    QStyleOptionSlider slider2;
    slider2.init(testWidget);
    slider2.orientation = Qt::Horizontal;
    slider2.tickPosition = QSlider::TicksLeft;

    style->drawSliderHandle(&slider2, rect, &p, testWidget);
    ASSERT_FALSE(pixmap.isNull());
    ASSERT_TRUE(testPixmapHasData());
    pixmap.fill(Qt::green);

    QStyleOptionSlider slider3;
    slider3.init(testWidget);
    slider2.orientation = Qt::Horizontal;
    slider3.tickPosition = QSlider::TicksRight;

    style->drawSliderHandle(&slider3, rect, &p, testWidget);
    ASSERT_FALSE(pixmap.isNull());
    ASSERT_TRUE(testPixmapHasData());
    pixmap.fill(Qt::green);

    QStyleOptionSlider slider4;
    slider2.init(testWidget);
    slider2.orientation = Qt::Vertical;
    slider2.tickPosition = QSlider::TicksLeft;

    style->drawSliderHandle(&slider2, rect, &p, testWidget);
    ASSERT_FALSE(pixmap.isNull());
    ASSERT_TRUE(testPixmapHasData());
    pixmap.fill(Qt::green);

    QStyleOptionSlider slider5;
    slider3.init(testWidget);
    slider2.orientation = Qt::Vertical;
    slider3.tickPosition = QSlider::TicksRight;

    style->drawSliderHandle(&slider3, rect, &p, testWidget);
    ASSERT_FALSE(pixmap.isNull());
    ASSERT_TRUE(testPixmapHasData());
}

TEST_F(TestForDrawUtil, drawSliderHandleFocus)
{
    testWidget->resize(100, 100);

    QPainter p(&pixmap);
    QRectF rect(0, 0, 20, 20);

    QStyleOptionSlider opt;
    opt.init(testWidget);
    opt.orientation = Qt::Horizontal;
    opt.tickPosition = QSlider::NoTicks;
    opt.minimum = 0;
    opt.maximum = 100;

    style->drawSliderHandleFocus(&opt, rect, &p, nullptr);
    ASSERT_FALSE(pixmap.isNull());
    ASSERT_TRUE(testPixmapHasData());
    pixmap.fill(Qt::green);

    opt.tickPosition = QSlider::TicksLeft;
    style->drawSliderHandleFocus(&opt, rect, &p, nullptr);
    ASSERT_FALSE(pixmap.isNull());
    ASSERT_TRUE(testPixmapHasData());
    pixmap.fill(Qt::green);

    opt.tickPosition = QSlider::TicksRight;
    style->drawSliderHandleFocus(&opt, rect, &p, nullptr);
    ASSERT_FALSE(pixmap.isNull());
    ASSERT_TRUE(testPixmapHasData());
    pixmap.fill(Qt::green);

    opt.orientation = Qt::Vertical;
    opt.tickPosition = QSlider::TicksLeft;
    style->drawSliderHandleFocus(&opt, rect, &p, nullptr);
    ASSERT_FALSE(pixmap.isNull());
    ASSERT_TRUE(testPixmapHasData());
    pixmap.fill(Qt::green);

    opt.tickPosition = QSlider::TicksRight;
    style->drawSliderHandleFocus(&opt, rect, &p, nullptr);
    ASSERT_FALSE(pixmap.isNull());
    ASSERT_TRUE(testPixmapHasData());
    pixmap.fill(Qt::green);
}

TEST_F(TestForDrawUtil, drawIcon)
{
    QStyleOption opt;
    opt.state |= QStyle::State_Enabled;
    opt.state |= QStyle::State_Selected;

    QPainter p(&pixmap);
    QRect rect(0, 0, 10, 10);

    style->drawIcon(&opt, &p, rect, style->standardIcon(QStyle::SP_DirIcon), true);
    ASSERT_FALSE(pixmap.isNull());
    ASSERT_TRUE(testPixmapHasData());
}

TEST_F(TestForDrawUtil, tabLayout)
{
    testWidget->resize(200, 200);

    QStyleOptionTab tab;
    tab.init(testWidget);
    tab.iconSize = QSize(32, 32);
    tab.shape = QTabBar::RoundedEast;
    tab.icon = style->standardIcon(QStyle::SP_DirIcon);

    QRect textRect, iconRect;
    style->tabLayout(&tab, testWidget, &textRect, &iconRect);
    ASSERT_TRUE(textRect.isValid());
    ASSERT_TRUE(iconRect.isValid());
}

TEST_F(TestForDrawUtil, drawTableViewItem)
{
    QTableView *view = new QTableView;
    view->resize(200, 200);

    QPainter p(&pixmap);

    QStyleOptionViewItem opt;
    opt.init(testWidget);
    opt.backgroundBrush = Qt::darkCyan;

    style->drawTableViewItem(QStyle::PE_PanelItemViewItem, &opt, &p, view);
    ASSERT_FALSE(pixmap.isNull());
    ASSERT_TRUE(testPixmapHasData());

    pixmap.fill(Qt::green);

    opt.state &= ~QStyle::State_Selected;
    view->setProperty("_d_dtk_enable_tableviewitem_radius", true);
    style->drawTableViewItem(QStyle::PE_PanelItemViewItem, &opt, &p, view);
    ASSERT_FALSE(pixmap.isNull());
    ASSERT_TRUE(testPixmapHasData());

    delete view;
}

TEST_F(TestForDrawUtil, drawMenuItemRedPoint)
{
    QMenu *menu = new QMenu;
    menu->resize(100, 200);

    QAction *action = new QAction(menu);
    action->setText("测试1");
    action->setProperty("_d_menu_item_redpoint", true);
    menu->addAction(action);

    QStyleOptionMenuItem opt;
    opt.init(menu);
    opt.menuItemType = QStyleOptionMenuItem::Normal;
    opt.rect = menu->actionGeometry(action);

    QPainter p(&pixmap);

    style->drawMenuItemRedPoint(&opt, &p, menu);
    ASSERT_FALSE(pixmap.isNull());
    ASSERT_TRUE(testPixmapHasData());

    pixmap.fill(Qt::green);
    QAction *actionMenu = new QAction(menu);
    QMenu *submenu = new QMenu("测试菜单");
    submenu->setProperty("_d_menu_item_redpoint", true);

    actionMenu->setMenu(submenu);
    menu->addAction(actionMenu);

    opt.menuItemType = QStyleOptionMenuItem::SubMenu;
    opt.rect = menu->actionGeometry(actionMenu);

    style->drawMenuItemRedPoint(&opt, &p, menu);
    ASSERT_FALSE(pixmap.isNull());
    ASSERT_TRUE(testPixmapHasData());

    delete menu;
}

#define INIT_DEFAULTWIDGETSIZE(W) \
    W.setGeometry(0, 0, 100, 100)

TEST_F(TestForDrawUtil, drawPrimitivePanelItemViewItem)
{
    QCalendarWidget calWidget;
    INIT_DEFAULTWIDGETSIZE(calWidget);
    QStyleOption opt;
    opt.initFrom(&calWidget);
    opt.state |= QStyle::State_Selected;

    QPainter p;
    p.begin(&pixmap);
    style->drawPrimitive(QStyle::PE_PanelItemViewItem, &opt, &p, calWidget.findChild<QTableView *>("qt_calendar_calendarview"));
    p.end();

    ASSERT_TRUE(testPixmapHasData());
    pixmap.fill(Qt::green);

    QTreeView tableView;
    INIT_DEFAULTWIDGETSIZE(tableView);

    QStyleOptionViewItem tableviewOption;
    tableviewOption.initFrom(&tableView);
    tableviewOption.showDecorationSelected = true;
    tableviewOption.state |= (QStyle::State_Selected | QStyle::State_MouseOver);

    tableviewOption.viewItemPosition = QStyleOptionViewItem::Beginning;
    p.begin(&pixmap);
    style->drawPrimitive(QStyle::PE_PanelItemViewItem, &tableviewOption, &p, &tableView);
    p.end();

    ASSERT_TRUE(testPixmapHasData());
    pixmap.fill(Qt::green);

    tableviewOption.viewItemPosition = QStyleOptionViewItem::End;
    p.begin(&pixmap);
    style->drawPrimitive(QStyle::PE_PanelItemViewItem, &tableviewOption, &p, &tableView);
    p.end();

    ASSERT_TRUE(testPixmapHasData());
    pixmap.fill(Qt::green);

    tableviewOption.showDecorationSelected = false;
    tableviewOption.backgroundBrush = Qt::darkRed;
    p.begin(&pixmap);
    style->drawPrimitive(QStyle::PE_PanelItemViewItem, &tableviewOption, &p, &tableView);
    p.end();

    ASSERT_TRUE(testPixmapHasData());
    pixmap.fill(Qt::green);
    QListView listView;
    INIT_DEFAULTWIDGETSIZE(listView);

    QStyleOptionViewItem listViewOption;
    listViewOption.initFrom(&listView);

    listViewOption.state |= QStyle::State_Selected;
    listViewOption.showDecorationSelected = true;
    p.begin(&pixmap);
    style->drawPrimitive(QStyle::PE_PanelItemViewItem, &listViewOption, &p, &listView);
    p.end();

    ASSERT_TRUE(testPixmapHasData());
}

TEST_F(TestForDrawUtil, drawPrimitivePanelLineEdit)
{
    QLineEdit lineEdit;
    INIT_DEFAULTWIDGETSIZE(lineEdit);

    QStyleOptionFrame frameOption;
    frameOption.initFrom(&lineEdit);

    frameOption.features |= QStyleOptionFrame::Flat;
    frameOption.state |= QStyle::State_HasFocus;

    QPainter p;
    p.begin(&pixmap);
    style->drawPrimitive(QStyle::PE_PanelLineEdit, &frameOption, &p, &lineEdit);
    p.end();

    ASSERT_TRUE(testPixmapHasData());
    pixmap.fill(Qt::green);

    frameOption.features |= QStyleOptionFrame::Rounded;
    frameOption.lineWidth = 5;

    p.begin(&pixmap);
    style->drawPrimitive(QStyle::PE_PanelLineEdit, &frameOption, &p, &lineEdit);
    p.end();

    ASSERT_TRUE(testPixmapHasData());
    pixmap.fill(Qt::green);

    lineEdit.setProperty("_d_dtk_lineedit_opacity", true);

    p.begin(&pixmap);
    style->drawPrimitive(QStyle::PE_PanelLineEdit, &frameOption, &p, &lineEdit);
    p.end();

    ASSERT_TRUE(testPixmapHasData());
    pixmap.fill(Qt::green);

    DSpinBox spinbox;
    INIT_DEFAULTWIDGETSIZE(spinbox);
    lineEdit.setParent(&spinbox);
    spinbox.setProperty("_d_dtk_lineedit_opacity", true);

    p.begin(&pixmap);
    style->drawPrimitive(QStyle::PE_PanelLineEdit, &frameOption, &p, &lineEdit);
    p.end();

    ASSERT_TRUE(testPixmapHasData());
    lineEdit.setParent(nullptr);
}

TEST_F(TestForDrawUtil, drawPrimitiveIndicatorRadioButton)
{
    QRadioButton radioButton;
    INIT_DEFAULTWIDGETSIZE(radioButton);

    QStyleOption radioButtonOption;
    radioButtonOption.initFrom(&radioButton);

    radioButtonOption.state |= QStyle::State_On;

    QPainter p;
    p.begin(&pixmap);
    style->drawPrimitive(QStyle::PE_IndicatorRadioButton, &radioButtonOption, &p, &radioButton);
    p.end();

    ASSERT_TRUE(testPixmapHasData());
    pixmap.fill(Qt::green);

    radioButtonOption.state &= ~QStyle::State_On;
    radioButtonOption.state |= QStyle::State_Off;
    p.begin(&pixmap);
    style->drawPrimitive(QStyle::PE_IndicatorRadioButton, &radioButtonOption, &p, &radioButton);
    p.end();

    ASSERT_TRUE(testPixmapHasData());
}

TEST_F(TestForDrawUtil, drawPrimitiveIndicatorCheckBox)
{
    QCheckBox checkBox;
    INIT_DEFAULTWIDGETSIZE(checkBox);

    QStyleOption checkBoxOption;
    checkBoxOption.initFrom(&checkBox);
    checkBoxOption.state |= QStyle::State_NoChange;

    QPainter p;
    p.begin(&pixmap);
    style->drawPrimitive(QStyle::PE_IndicatorCheckBox, &checkBoxOption, &p, &checkBox);
    p.end();

    ASSERT_TRUE(testPixmapHasData());
    pixmap.fill(Qt::green);

    checkBoxOption.state &= ~QStyle::State_NoChange;
    p.begin(&pixmap);
    style->drawPrimitive(QStyle::PE_IndicatorCheckBox, &checkBoxOption, &p, &checkBox);
    p.end();

    ASSERT_TRUE(testPixmapHasData());
    pixmap.fill(Qt::green);
}
