#include "StyleManager.h"
#include "../utils/Logger.h"
#include <QApplication>

StyleManager& StyleManager::instance()
{
    static StyleManager instance;
    return instance;
}

StyleManager::StyleManager(QObject* parent)
    : QObject(parent)
    , currentTheme_(Light)
{
}

StyleManager::~StyleManager()
{
}

void StyleManager::initialize()
{
    initializeColors();
    applyTheme(Dark);  // Default to dark theme
    Logger::instance().info("StyleManager", "Style manager initialized with dark theme");
}

void StyleManager::applyTheme(Theme theme)
{
    currentTheme_ = theme;
    currentStylesheet_ = buildStylesheet(theme);

    if (qApp) {
        qApp->setStyleSheet(currentStylesheet_);
    }

    emit themeChanged(theme);
    Logger::instance().info("StyleManager",
        QString("Applied theme: %1").arg(theme == Light ? "Light" : "Dark"));
}

QString StyleManager::getStylesheet() const
{
    return currentStylesheet_;
}

QColor StyleManager::getColor(const QString& colorName) const
{
    const QMap<QString, QColor>& colors = (currentTheme_ == Light) ? lightColors_ : darkColors_;
    return colors.value(colorName, QColor(128, 128, 128));
}

QString StyleManager::getButtonStyle(const QString& type) const
{
    QColor bgColor, textColor, hoverColor, pressedColor;

    if (type == "primary") {
        bgColor = getColor("primary");
        textColor = QColor(255, 255, 255);
        hoverColor = bgColor.lighter(110);
        pressedColor = bgColor.darker(110);
    } else if (type == "danger") {
        bgColor = getColor("danger");
        textColor = QColor(255, 255, 255);
        hoverColor = bgColor.lighter(110);
        pressedColor = bgColor.darker(110);
    } else if (type == "success") {
        bgColor = getColor("accent");
        textColor = QColor(255, 255, 255);
        hoverColor = bgColor.lighter(110);
        pressedColor = bgColor.darker(110);
    } else { // secondary
        bgColor = getColor("secondary");
        textColor = QColor(255, 255, 255);
        hoverColor = bgColor.lighter(110);
        pressedColor = bgColor.darker(110);
    }

    return QString(
        "background-color: %1; "
        "color: %2; "
        "border: none; "
        "border-radius: 4px; "
        "padding: 8px 16px; "
        "font-weight: bold; "
        "min-width: 80px; "
    ).arg(bgColor.name(), textColor.name());
}

QString StyleManager::getTableStyle() const
{
    return QString(
        "QTableWidget {"
        "    gridline-color: %1; "
        "    background-color: %2; "
        "    alternate-background-color: %3; "
        "    selection-background-color: %4; "
        "    selection-color: %5; "
        "    border: 1px solid %6; "
        "    border-radius: 4px; "
        "} "
        "QTableWidget::item {"
        "    padding: 4px; "
        "} "
        "QHeaderView::section {"
        "    background-color: %7; "
        "    color: %8; "
        "    padding: 8px; "
        "    border: none; "
        "    border-bottom: 2px solid %9; "
        "    font-weight: bold; "
        "} "
    ).arg(
        getColor("border").name(),
        getColor("background").name(),
        getColor("alternateBackground").name(),
        getColor("primary").name(),
        QColor(255, 255, 255).name(),
        getColor("border").name(),
        getColor("headerBackground").name(),
        getColor("headerText").name(),
        getColor("primary").name()
    );
}

QString StyleManager::getCardStyle() const
{
    return QString(
        "QGroupBox {"
        "    background-color: %1; "
        "    border: 1px solid %2; "
        "    border-radius: 8px; "
        "    margin-top: 12px; "
        "    padding: 16px; "
        "    font-weight: bold; "
        "} "
        "QGroupBox::title {"
        "    subcontrol-origin: margin; "
        "    subcontrol-position: top left; "
        "    padding: 4px 8px; "
        "    color: %3; "
        "} "
    ).arg(
        getColor("cardBackground").name(),
        getColor("border").name(),
        getColor("primary").name()
    );
}

QString StyleManager::getInputStyle() const
{
    return QString(
        "QLineEdit, QTextEdit, QPlainTextEdit, QComboBox {"
        "    background-color: %1; "
        "    border: 1px solid %2; "
        "    border-radius: 4px; "
        "    padding: 6px; "
        "    color: %3; "
        "} "
        "QLineEdit:focus, QTextEdit:focus, QPlainTextEdit:focus, QComboBox:focus {"
        "    border: 2px solid %4; "
        "} "
        "QLineEdit:disabled, QTextEdit:disabled, QPlainTextEdit:disabled, QComboBox:disabled {"
        "    background-color: %5; "
        "    color: %6; "
        "} "
    ).arg(
        getColor("inputBackground").name(),
        getColor("border").name(),
        getColor("text").name(),
        getColor("primary").name(),
        getColor("disabledBackground").name(),
        getColor("disabledText").name()
    );
}

QString StyleManager::buildStylesheet(Theme theme) const
{
    QMap<QString, QColor> colors = getThemeColors(theme);

    QString stylesheet = QString(
        "/* Global Styles - Improved spacing and typography */ "
        "QWidget {"
        "    font-family: 'Segoe UI', 'San Francisco', 'Helvetica Neue', Arial, sans-serif; "
        "    font-size: 13pt; "  // Increased from 10pt
        "    color: %1; "
        "    background-color: %2; "
        "} "
        ""
        "/* Main Window */ "
        "QMainWindow {"
        "    background-color: %3; "
        "} "
        ""
        "/* Buttons - Primary */ "
        "QPushButton {"
        "    background-color: %4; "
        "    color: white; "
        "    border: none; "
        "    border-radius: 8px; "  // Rounded-lg style
        "    padding: 12px 24px; "
        "    font-weight: 600; "
        "    font-size: 13pt; "
        "    min-width: 100px; "
        "    min-height: 36px; "
        "} "
        "QPushButton:hover {"
        "    opacity: 0.9; "  // Subtle fade on hover
        "    background-color: %5; "
        "} "
        "QPushButton:pressed {"
        "    background-color: %6; "
        "} "
        "QPushButton:disabled {"
        "    background-color: %7; "
        "    color: %8; "
        "} "
        ""
        "/* Labels */ "
        "QLabel {"
        "    font-size: 13pt; "
        "    padding: 2px; "
        "} "
        ""
        "/* Tables */ "
        "QTableWidget {"
        "    gridline-color: %9; "
        "    background-color: %2; "  // Use background color
        "    alternate-background-color: %10; "
        "    selection-background-color: %11; "
        "    selection-color: white; "
        "    border: 1px solid %12; "
        "    border-radius: 12px; "  // Rounded-xl style
        "    font-size: 13pt; "
        "} "
        "QTableWidget::item {"
        "    padding: 16px; "  // p-4 table cells
        "    min-height: 32px; "
        "} "
        "QTableWidget::item:hover {"
        "    background-color: %13; "
        "} "
        "QHeaderView::section {"
        "    background-color: %14; "
        "    color: %15; "
        "    padding: 14px 12px; "  // Increased from 10px
        "    border: none; "
        "    border-bottom: 2px solid %16; "
        "    font-weight: 600; "
        "    font-size: 13pt; "
        "    text-align: left; "
        "    min-height: 40px; "  // Added minimum height
        "} "
        "QHeaderView::section:hover {"
        "    background-color: %17; "
        "} "
        ""
        "/* Group Boxes / Cards */ "
        "QGroupBox {"
        "    background-color: %18; "
        "    border: 1px solid %19; "
        "    border-radius: 12px; "  // Rounded-xl style
        "    margin: 0px; "  // Remove margins - let layout spacing control this
        "    padding: 24px; "
        "    font-weight: 600; "
        "    font-size: 15pt; "
        "} "
        "QGroupBox::title {"
        "    subcontrol-origin: margin; "
        "    subcontrol-position: top left; "
        "    padding: 4px 12px; "
        "    color: %20; "
        "    background-color: transparent; "
        "} "
        ""
        "/* Input Fields */ "
        "QLineEdit, QTextEdit, QPlainTextEdit {"
        "    background-color: %2; "  // Use darker background (slate-900)
        "    border: 1px solid %21; "
        "    border-radius: 8px; "  // Rounded-lg style
        "    padding: 10px 12px; "
        "    color: %22; "
        "    font-size: 13pt; "
        "    min-height: 32px; "
        "    selection-background-color: %23; "
        "    selection-color: white; "
        "} "
        "QLineEdit:focus, QTextEdit:focus, QPlainTextEdit:focus {"
        "    border: 2px solid #06b6d4; "  // Cyan-500 focus ring
        "    padding: 9px 11px; "
        "} "
        "QLineEdit:disabled, QTextEdit:disabled, QPlainTextEdit:disabled {"
        "    background-color: %25; "
        "    color: %26; "
        "} "
        ""
        "/* Combo Boxes */ "
        "QComboBox {"
        "    background-color: %2; "  // Use darker background (slate-900)
        "    border: 1px solid %27; "
        "    border-radius: 8px; "  // Rounded-lg style
        "    padding: 10px 12px; "
        "    font-size: 13pt; "
        "    min-width: 120px; "
        "    min-height: 32px; "
        "    color: %1; "
        "} "
        "QComboBox:hover {"
        "    border: 1px solid %28; "
        "} "
        "QComboBox:focus {"
        "    border: 2px solid #06b6d4; "  // Cyan-500 focus ring
        "    padding: 9px 11px; "
        "} "
        "QComboBox::drop-down {"
        "    border: none; "
        "    width: 24px; "  // Slightly wider
        "} "
        "QComboBox::down-arrow {"
        "    image: none; "
        "    border-left: 5px solid transparent; "
        "    border-right: 5px solid transparent; "
        "    border-top: 7px solid %30; "
        "    margin-right: 8px; "
        "} "
        "QComboBox QAbstractItemView {"
        "    background-color: %18; "  // Use card background (slate-800)
        "    border: 1px solid %31; "
        "    border-radius: 8px; "
        "    selection-background-color: %32; "
        "    selection-color: white; "
        "    padding: 6px; "
        "    color: %1; "
        "} "
        "QComboBox QAbstractItemView::item {"
        "    padding: 8px 12px; "
        "    min-height: 28px; "
        "} "
        ""
        "/* Spin Boxes */ "
        "QSpinBox, QDoubleSpinBox {"
        "    background-color: %2; "  // Use darker background (slate-900)
        "    border: 1px solid %33; "
        "    border-radius: 8px; "  // Rounded-lg style
        "    padding: 10px 12px; "
        "    font-size: 13pt; "
        "    min-height: 32px; "
        "    color: %1; "
        "} "
        "QSpinBox:focus, QDoubleSpinBox:focus {"
        "    border: 2px solid #06b6d4; "  // Cyan-500 focus ring
        "} "
        ""
        "/* Labels */ "
        "QLabel {"
        "    background-color: transparent; "
        "    color: %34; "
        "} "
        ""
        "/* Menu Bar */ "
        "QMenuBar {"
        "    background-color: %35; "
        "    color: %36; "
        "    border-bottom: 1px solid %37; "
        "    padding: 6px; "  // Increased from 4px
        "    font-size: 13pt; "
        "} "
        "QMenuBar::item {"
        "    background-color: transparent; "
        "    padding: 8px 16px; "  // Increased from 6px 12px
        "    border-radius: 6px; "  // Increased from 4px
        "    margin: 2px; "
        "} "
        "QMenuBar::item:selected {"
        "    background-color: %38; "
        "} "
        "QMenuBar::item:pressed {"
        "    background-color: %39; "
        "} "
        ""
        "/* Menus */ "
        "QMenu {"
        "    background-color: %18; "  // Use card background (slate-800)
        "    border: 1px solid %40; "
        "    border-radius: 12px; "  // Rounded-xl style
        "    padding: 8px; "
        "    font-size: 13pt; "
        "    color: %1; "
        "} "
        "QMenu::item {"
        "    padding: 10px 32px; "  // Increased from 6px 24px
        "    border-radius: 6px; "  // Increased from 4px
        "    min-height: 28px; "  // Added minimum height
        "} "
        "QMenu::item:selected {"
        "    background-color: %41; "
        "    color: white; "
        "} "
        "QMenu::separator {"
        "    height: 1px; "
        "    background-color: %40; "
        "    margin: 6px 0px; "
        "} "
        ""
        "/* Tool Bar */ "
        "QToolBar {"
        "    background-color: %42; "
        "    border: none; "
        "    spacing: 8px; "  // Increased from 4px
        "    padding: 8px; "  // Increased from 4px
        "} "
        "QToolBar::separator {"
        "    background-color: %43; "
        "    width: 1px; "
        "    margin: 6px 12px; "  // Increased from 4px 8px
        "} "
        ""
        "/* Tool Buttons */ "
        "QToolButton {"
        "    background-color: transparent; "
        "    border: none; "
        "    border-radius: 6px; "  // Increased from 4px
        "    padding: 10px; "  // Increased from 6px
        "    min-width: 32px; "
        "    min-height: 32px; "
        "} "
        "QToolButton:hover {"
        "    background-color: %44; "
        "} "
        "QToolButton:pressed {"
        "    background-color: %45; "
        "} "
        ""
        "/* Status Bar */ "
        "QStatusBar {"
        "    background-color: %46; "
        "    color: %47; "
        "    border-top: 1px solid %48; "
        "    padding: 6px 12px; "  // Added padding
        "    font-size: 12pt; "  // Slightly smaller for status
        "} "
        ""
        "/* Progress Bar */ "
        "QProgressBar {"
        "    border: 1px solid %49; "
        "    border-radius: 6px; "  // Increased from 4px
        "    text-align: center; "
        "    background-color: %50; "
        "    min-height: 24px; "  // Added minimum height
        "    font-size: 12pt; "
        "} "
        "QProgressBar::chunk {"
        "    background-color: %51; "
        "    border-radius: 5px; "  // Increased from 3px
        "} "
        ""
        "/* Scroll Bars */ "
        "QScrollBar:vertical {"
        "    background-color: %52; "
        "    width: 14px; "  // Increased from 12px
        "    border-radius: 7px; "
        "    margin: 0px; "
        "} "
        "QScrollBar::handle:vertical {"
        "    background-color: %53; "
        "    border-radius: 7px; "
        "    min-height: 30px; "  // Increased from 20px
        "    margin: 2px; "
        "} "
        "QScrollBar::handle:vertical:hover {"
        "    background-color: %54; "
        "} "
        "QScrollBar:horizontal {"
        "    background-color: %55; "
        "    height: 14px; "  // Increased from 12px
        "    border-radius: 7px; "
        "    margin: 0px; "
        "} "
        "QScrollBar::handle:horizontal {"
        "    background-color: %56; "
        "    border-radius: 7px; "
        "    min-width: 30px; "  // Increased from 20px
        "    margin: 2px; "
        "} "
        "QScrollBar::handle:horizontal:hover {"
        "    background-color: %57; "
        "} "
        "QScrollBar::add-line, QScrollBar::sub-line {"
        "    background: none; "
        "    border: none; "
        "} "
        ""
        "/* Tab Widget */ "
        "QTabWidget::pane {"
        "    border: 1px solid %58; "
        "    border-radius: 12px; "  // Rounded-xl style
        "    background-color: %18; "  // Use card background (slate-800)
        "    padding: 8px; "
        "} "
        "QTabBar::tab {"
        "    background-color: %59; "
        "    color: %60; "
        "    padding: 12px 24px; "  // Increased from 8px 16px
        "    border: 1px solid %61; "
        "    border-bottom: none; "
        "    border-top-left-radius: 8px; "  // Increased from 4px
        "    border-top-right-radius: 8px; "  // Increased from 4px
        "    margin-right: 4px; "  // Increased from 2px
        "    min-width: 80px; "  // Added minimum width
        "    font-size: 13pt; "
        "    font-weight: 500; "
        "} "
        "QTabBar::tab:selected {"
        "    background-color: %18; "  // Use card background (slate-800)
        "    color: %62; "
        "    border-bottom: 3px solid %63; "
        "    font-weight: 600; "
        "} "
        "QTabBar::tab:hover:!selected {"
        "    background-color: %64; "
        "} "
        ""
        "/* Splitter */ "
        "QSplitter::handle {"
        "    background-color: %65; "
        "    width: 2px; "  // Added explicit width
        "} "
        "QSplitter::handle:hover {"
        "    background-color: %66; "
        "} "
        ""
        "/* Tree Widget */ "
        "QTreeWidget {"
        "    background-color: %2; "  // Use main background (slate-900)
        "    border: 1px solid %67; "
        "    border-radius: 12px; "  // Rounded-xl style
        "    alternate-background-color: %68; "
        "    selection-background-color: %69; "
        "    selection-color: white; "
        "    font-size: 13pt; "
        "    color: %1; "
        "} "
        "QTreeWidget::item {"
        "    padding: 10px 8px; "  // Increased from 4px
        "    min-height: 32px; "  // Added minimum height
        "} "
        "QTreeWidget::item:hover {"
        "    background-color: %70; "
        "} "
        "QTreeWidget::branch {"
        "    background-color: transparent; "
        "} "
        ""
        "/* List Widget */ "
        "QListWidget {"
        "    background-color: %2; "  // Use main background (slate-900)
        "    border: 1px solid %67; "
        "    border-radius: 12px; "  // Rounded-xl style
        "    alternate-background-color: %68; "
        "    selection-background-color: %69; "
        "    selection-color: white; "
        "    font-size: 13pt; "
        "    color: %1; "
        "} "
        "QListWidget::item {"
        "    padding: 10px 12px; "  // Good spacing for list items
        "    min-height: 32px; "
        "} "
        "QListWidget::item:hover {"
        "    background-color: %70; "
        "} "
        ""
        "/* Check Box */ "
        "QCheckBox {"
        "    spacing: 10px; "  // Increased from 8px
        "    font-size: 13pt; "
        "    padding: 4px; "
        "} "
        "QCheckBox::indicator {"
        "    width: 20px; "  // Increased from 18px
        "    height: 20px; "  // Increased from 18px
        "    border: 2px solid %71; "
        "    border-radius: 4px; "  // Increased from 3px
        "    background-color: white; "
        "} "
        "QCheckBox::indicator:checked {"
        "    background-color: %72; "
        "    border-color: %73; "
        "} "
        "QCheckBox::indicator:hover {"
        "    border-color: %72; "
        "} "
        ""
        "/* Radio Button */ "
        "QRadioButton {"
        "    spacing: 10px; "  // Increased from 8px
        "    font-size: 13pt; "
        "    padding: 4px; "
        "} "
        "QRadioButton::indicator {"
        "    width: 20px; "  // Increased from 18px
        "    height: 20px; "  // Increased from 18px
        "    border: 2px solid %74; "
        "    border-radius: 10px; "  // Increased from 9px
        "    background-color: white; "
        "} "
        "QRadioButton::indicator:checked {"
        "    background-color: %75; "
        "    border-color: %76; "
        "} "
        "QRadioButton::indicator:hover {"
        "    border-color: %75; "
        "} "
        ""
        "/* Tooltips */ "
        "QToolTip {"
        "    background-color: %72; "
        "    color: white; "
        "    border: none; "
        "    border-radius: 6px; "
        "    padding: 8px 12px; "
        "    font-size: 12pt; "
        "} "
    ).arg(
        colors["text"].name(),                      // 1
        colors["background"].name(),                // 2
        colors["windowBackground"].name(),          // 3
        colors["primary"].name(),                   // 4
        colors["primary"].lighter(110).name(),      // 5
        colors["primary"].darker(110).name(),       // 6
        colors["disabledBackground"].name(),        // 7
        colors["disabledText"].name(),              // 8
        colors["border"].name(),                    // 9
        colors["alternateBackground"].name(),       // 10
        colors["primary"].name(),                   // 11
        colors["border"].name(),                    // 12
        colors["hoverBackground"].name(),           // 13
        colors["headerBackground"].name(),          // 14
        colors["headerText"].name(),                // 15
        colors["primary"].name(),                   // 16
        colors["headerBackground"].lighter(105).name(), // 17
        colors["cardBackground"].name(),            // 18
        colors["border"].name(),                    // 19
        colors["primary"].name(),                   // 20
        colors["border"].name(),                    // 21
        colors["text"].name(),                      // 22
        colors["primary"].name(),                   // 23
        colors["primary"].name(),                   // 24
        colors["disabledBackground"].name(),        // 25
        colors["disabledText"].name(),              // 26
        colors["border"].name(),                    // 27
        colors["secondary"].name(),                 // 28
        colors["primary"].name(),                   // 29
        colors["text"].name(),                      // 30
        colors["border"].name(),                    // 31
        colors["primary"].name(),                   // 32
        colors["border"].name(),                    // 33
        colors["text"].name(),                      // 34
        colors["menuBackground"].name(),            // 35
        colors["menuText"].name(),                  // 36
        colors["border"].name(),                    // 37
        colors["hoverBackground"].name(),           // 38
        colors["primary"].lighter(120).name(),      // 39
        colors["border"].name(),                    // 40
        colors["primary"].name(),                   // 41
        colors["toolbarBackground"].name(),         // 42
        colors["border"].name(),                    // 43
        colors["hoverBackground"].name(),           // 44
        colors["primary"].lighter(120).name(),      // 45
        colors["statusBarBackground"].name(),       // 46
        colors["statusBarText"].name(),             // 47
        colors["border"].name(),                    // 48
        colors["border"].name(),                    // 49
        colors["progressBackground"].name(),        // 50
        colors["primary"].name(),                   // 51
        colors["scrollBarBackground"].name(),       // 52
        colors["scrollBarHandle"].name(),           // 53
        colors["scrollBarHandle"].lighter(110).name(), // 54
        colors["scrollBarBackground"].name(),       // 55
        colors["scrollBarHandle"].name(),           // 56
        colors["scrollBarHandle"].lighter(110).name(), // 57
        colors["border"].name(),                    // 58
        colors["tabBackground"].name(),             // 59
        colors["tabText"].name(),                   // 60
        colors["border"].name(),                    // 61
        colors["primary"].name(),                   // 62
        colors["primary"].name(),                   // 63
        colors["hoverBackground"].name(),           // 64
        colors["border"].name(),                    // 65
        colors["secondary"].name(),                 // 66
        colors["border"].name(),                    // 67
        colors["alternateBackground"].name(),       // 68
        colors["primary"].name(),                   // 69
        colors["hoverBackground"].name(),           // 70
        colors["border"].name(),                    // 71
        colors["primary"].name(),                   // 72
        colors["primary"].name(),                   // 73
        colors["border"].name(),                    // 74
        colors["primary"].name(),                   // 75
        colors["primary"].name()                    // 76
    );

    return stylesheet;
}

void StyleManager::initializeColors()
{
    // Light Theme Colors
    lightColors_["primary"] = QColor("#2196F3");          // Blue
    lightColors_["secondary"] = QColor("#757575");        // Gray
    lightColors_["accent"] = QColor("#4CAF50");           // Green
    lightColors_["danger"] = QColor("#F44336");           // Red
    lightColors_["warning"] = QColor("#FF9800");          // Orange
    lightColors_["success"] = QColor("#4CAF50");          // Green
    lightColors_["background"] = QColor("#FAFAFA");       // Very light gray
    lightColors_["windowBackground"] = QColor("#F5F5F5"); // Light gray
    lightColors_["cardBackground"] = QColor("#FFFFFF");   // White
    lightColors_["text"] = QColor("#212121");             // Dark gray
    lightColors_["textSecondary"] = QColor("#757575");    // Medium gray
    lightColors_["border"] = QColor("#E0E0E0");           // Light border
    lightColors_["alternateBackground"] = QColor("#F5F5F5"); // Alternate row
    lightColors_["hoverBackground"] = QColor("#E3F2FD");  // Light blue hover
    lightColors_["headerBackground"] = QColor("#FAFAFA"); // Header background
    lightColors_["headerText"] = QColor("#424242");       // Header text
    lightColors_["inputBackground"] = QColor("#FFFFFF");  // Input bg
    lightColors_["disabledBackground"] = QColor("#F5F5F5"); // Disabled bg
    lightColors_["disabledText"] = QColor("#BDBDBD");     // Disabled text
    lightColors_["menuBackground"] = QColor("#FFFFFF");   // Menu bg
    lightColors_["menuText"] = QColor("#212121");         // Menu text
    lightColors_["toolbarBackground"] = QColor("#FAFAFA"); // Toolbar bg
    lightColors_["statusBarBackground"] = QColor("#F5F5F5"); // Status bar bg
    lightColors_["statusBarText"] = QColor("#616161");    // Status bar text
    lightColors_["progressBackground"] = QColor("#E0E0E0"); // Progress bg
    lightColors_["scrollBarBackground"] = QColor("#F5F5F5"); // Scrollbar bg
    lightColors_["scrollBarHandle"] = QColor("#BDBDBD"); // Scrollbar handle
    lightColors_["tabBackground"] = QColor("#F5F5F5");    // Tab bg
    lightColors_["tabText"] = QColor("#757575");          // Tab text

    // Dark Theme Colors - Slate Professional Design System
    // Inspired by Tailwind Slate palette with Cyan accents

    // Accent colors (buttons, highlights, interactive elements)
    darkColors_["primary"] = QColor("#0891b2");           // Cyan-600 - primary buttons/accents
    darkColors_["accent"] = QColor("#22d3ee");            // Cyan-400 - accent text/highlights
    darkColors_["success"] = QColor("#059669");           // Emerald-600 - success states
    darkColors_["danger"] = QColor("#dc2626");            // Red-600 - danger/error states
    darkColors_["warning"] = QColor("#d97706");           // Amber-600 - warning states
    darkColors_["secondary"] = QColor("#64748b");         // Slate-500 - secondary elements

    // Background colors (slate palette)
    darkColors_["background"] = QColor("#0f172a");        // Slate-900 - main background
    darkColors_["windowBackground"] = QColor("#0f172a");  // Slate-900 - window bg
    darkColors_["cardBackground"] = QColor("#1e293b");    // Slate-800 - cards/panels
    darkColors_["inputBackground"] = QColor("#0f172a");   // Slate-900 - input fields
    darkColors_["alternateBackground"] = QColor("#1e293b"); // Slate-800 - alternate rows
    darkColors_["headerBackground"] = QColor("#1e293b");  // Slate-800 - headers
    darkColors_["hoverBackground"] = QColor("#334155");   // Slate-700 - hover state
    darkColors_["disabledBackground"] = QColor("#1e293b"); // Slate-800 - disabled bg
    darkColors_["toolbarBackground"] = QColor("#1e293b"); // Slate-800 - toolbar
    darkColors_["statusBarBackground"] = QColor("#0f172a"); // Slate-900 - status bar
    darkColors_["menuBackground"] = QColor("#1e293b");    // Slate-800 - menus
    darkColors_["tabBackground"] = QColor("#1e293b");     // Slate-800 - tabs

    // Text colors
    darkColors_["text"] = QColor("#f1f5f9");              // Slate-100 - primary text
    darkColors_["textSecondary"] = QColor("#94a3b8");     // Slate-400 - muted text
    darkColors_["headerText"] = QColor("#f1f5f9");        // Slate-100 - header text
    darkColors_["menuText"] = QColor("#f1f5f9");          // Slate-100 - menu text
    darkColors_["statusBarText"] = QColor("#94a3b8");     // Slate-400 - status text
    darkColors_["tabText"] = QColor("#94a3b8");           // Slate-400 - tab text
    darkColors_["disabledText"] = QColor("#64748b");      // Slate-500 - disabled text

    // Borders and separators
    darkColors_["border"] = QColor("#334155");            // Slate-700 - borders

    // UI elements
    darkColors_["progressBackground"] = QColor("#334155"); // Slate-700 - progress bg
    darkColors_["scrollBarBackground"] = QColor("#1e293b"); // Slate-800 - scrollbar bg
    darkColors_["scrollBarHandle"] = QColor("#475569");   // Slate-600 - scrollbar handle
}

QMap<QString, QColor> StyleManager::getThemeColors(Theme theme) const
{
    return (theme == Light) ? lightColors_ : darkColors_;
}
