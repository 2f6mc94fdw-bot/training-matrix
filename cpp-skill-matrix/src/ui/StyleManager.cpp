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
        "/* Global Styles */ "
        "QWidget {"
        "    font-family: 'Segoe UI', 'San Francisco', 'Helvetica Neue', Arial, sans-serif; "
        "    font-size: 10pt; "
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
        "    border-radius: 4px; "
        "    padding: 8px 16px; "
        "    font-weight: bold; "
        "    min-width: 80px; "
        "} "
        "QPushButton:hover {"
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
        "/* Tables */ "
        "QTableWidget {"
        "    gridline-color: %9; "
        "    background-color: white; "
        "    alternate-background-color: %10; "
        "    selection-background-color: %11; "
        "    selection-color: white; "
        "    border: 1px solid %12; "
        "    border-radius: 4px; "
        "} "
        "QTableWidget::item {"
        "    padding: 6px; "
        "} "
        "QTableWidget::item:hover {"
        "    background-color: %13; "
        "} "
        "QHeaderView::section {"
        "    background-color: %14; "
        "    color: %15; "
        "    padding: 10px; "
        "    border: none; "
        "    border-bottom: 2px solid %16; "
        "    font-weight: bold; "
        "    text-align: left; "
        "} "
        "QHeaderView::section:hover {"
        "    background-color: %17; "
        "} "
        ""
        "/* Group Boxes / Cards */ "
        "QGroupBox {"
        "    background-color: %18; "
        "    border: 1px solid %19; "
        "    border-radius: 8px; "
        "    margin-top: 16px; "
        "    padding: 16px; "
        "    font-weight: bold; "
        "    font-size: 11pt; "
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
        "    background-color: white; "
        "    border: 1px solid %21; "
        "    border-radius: 4px; "
        "    padding: 8px; "
        "    color: %22; "
        "    selection-background-color: %23; "
        "    selection-color: white; "
        "} "
        "QLineEdit:focus, QTextEdit:focus, QPlainTextEdit:focus {"
        "    border: 2px solid %24; "
        "} "
        "QLineEdit:disabled, QTextEdit:disabled, QPlainTextEdit:disabled {"
        "    background-color: %25; "
        "    color: %26; "
        "} "
        ""
        "/* Combo Boxes */ "
        "QComboBox {"
        "    background-color: white; "
        "    border: 1px solid %27; "
        "    border-radius: 4px; "
        "    padding: 6px 12px; "
        "    min-width: 100px; "
        "} "
        "QComboBox:hover {"
        "    border: 1px solid %28; "
        "} "
        "QComboBox:focus {"
        "    border: 2px solid %29; "
        "} "
        "QComboBox::drop-down {"
        "    border: none; "
        "    width: 20px; "
        "} "
        "QComboBox::down-arrow {"
        "    image: none; "
        "    border-left: 4px solid transparent; "
        "    border-right: 4px solid transparent; "
        "    border-top: 6px solid %30; "
        "    margin-right: 6px; "
        "} "
        "QComboBox QAbstractItemView {"
        "    background-color: white; "
        "    border: 1px solid %31; "
        "    selection-background-color: %32; "
        "    selection-color: white; "
        "    padding: 4px; "
        "} "
        ""
        "/* Spin Boxes */ "
        "QSpinBox, QDoubleSpinBox {"
        "    background-color: white; "
        "    border: 1px solid %33; "
        "    border-radius: 4px; "
        "    padding: 6px; "
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
        "    padding: 4px; "
        "} "
        "QMenuBar::item {"
        "    background-color: transparent; "
        "    padding: 6px 12px; "
        "    border-radius: 4px; "
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
        "    background-color: white; "
        "    border: 1px solid %40; "
        "    padding: 4px; "
        "} "
        "QMenu::item {"
        "    padding: 6px 24px; "
        "    border-radius: 4px; "
        "} "
        "QMenu::item:selected {"
        "    background-color: %41; "
        "    color: white; "
        "} "
        ""
        "/* Tool Bar */ "
        "QToolBar {"
        "    background-color: %42; "
        "    border: none; "
        "    spacing: 4px; "
        "    padding: 4px; "
        "} "
        "QToolBar::separator {"
        "    background-color: %43; "
        "    width: 1px; "
        "    margin: 4px 8px; "
        "} "
        ""
        "/* Tool Buttons */ "
        "QToolButton {"
        "    background-color: transparent; "
        "    border: none; "
        "    border-radius: 4px; "
        "    padding: 6px; "
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
        "} "
        ""
        "/* Progress Bar */ "
        "QProgressBar {"
        "    border: 1px solid %49; "
        "    border-radius: 4px; "
        "    text-align: center; "
        "    background-color: %50; "
        "} "
        "QProgressBar::chunk {"
        "    background-color: %51; "
        "    border-radius: 3px; "
        "} "
        ""
        "/* Scroll Bars */ "
        "QScrollBar:vertical {"
        "    background-color: %52; "
        "    width: 12px; "
        "    border-radius: 6px; "
        "} "
        "QScrollBar::handle:vertical {"
        "    background-color: %53; "
        "    border-radius: 6px; "
        "    min-height: 20px; "
        "} "
        "QScrollBar::handle:vertical:hover {"
        "    background-color: %54; "
        "} "
        "QScrollBar:horizontal {"
        "    background-color: %55; "
        "    height: 12px; "
        "    border-radius: 6px; "
        "} "
        "QScrollBar::handle:horizontal {"
        "    background-color: %56; "
        "    border-radius: 6px; "
        "    min-width: 20px; "
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
        "    border-radius: 4px; "
        "    background-color: white; "
        "} "
        "QTabBar::tab {"
        "    background-color: %59; "
        "    color: %60; "
        "    padding: 8px 16px; "
        "    border: 1px solid %61; "
        "    border-bottom: none; "
        "    border-top-left-radius: 4px; "
        "    border-top-right-radius: 4px; "
        "    margin-right: 2px; "
        "} "
        "QTabBar::tab:selected {"
        "    background-color: white; "
        "    color: %62; "
        "    border-bottom: 2px solid %63; "
        "} "
        "QTabBar::tab:hover:!selected {"
        "    background-color: %64; "
        "} "
        ""
        "/* Splitter */ "
        "QSplitter::handle {"
        "    background-color: %65; "
        "} "
        "QSplitter::handle:hover {"
        "    background-color: %66; "
        "} "
        ""
        "/* Tree Widget */ "
        "QTreeWidget {"
        "    background-color: white; "
        "    border: 1px solid %67; "
        "    border-radius: 4px; "
        "    alternate-background-color: %68; "
        "    selection-background-color: %69; "
        "    selection-color: white; "
        "} "
        "QTreeWidget::item {"
        "    padding: 4px; "
        "} "
        "QTreeWidget::item:hover {"
        "    background-color: %70; "
        "} "
        ""
        "/* Check Box */ "
        "QCheckBox {"
        "    spacing: 8px; "
        "} "
        "QCheckBox::indicator {"
        "    width: 18px; "
        "    height: 18px; "
        "    border: 2px solid %71; "
        "    border-radius: 3px; "
        "    background-color: white; "
        "} "
        "QCheckBox::indicator:checked {"
        "    background-color: %72; "
        "    border-color: %73; "
        "} "
        ""
        "/* Radio Button */ "
        "QRadioButton {"
        "    spacing: 8px; "
        "} "
        "QRadioButton::indicator {"
        "    width: 18px; "
        "    height: 18px; "
        "    border: 2px solid %74; "
        "    border-radius: 9px; "
        "    background-color: white; "
        "} "
        "QRadioButton::indicator:checked {"
        "    background-color: %75; "
        "    border-color: %76; "
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

    // Dark Theme Colors - Following 6-3-1 Rule
    // 60% - Dominant: Deep navy background
    // 30% - Secondary: Medium panels and cards
    // 10% - Accent: Bright highlights

    // Accent colors (10% - highlights, CTAs, important elements)
    darkColors_["primary"] = QColor("#7AA2F7");           // Bright blue accent
    darkColors_["accent"] = QColor("#2AC3DE");            // Cyan accent
    darkColors_["success"] = QColor("#9ECE6A");           // Green accent
    darkColors_["danger"] = QColor("#F7768E");            // Red accent
    darkColors_["warning"] = QColor("#E0AF68");           // Orange accent
    darkColors_["secondary"] = QColor("#565F89");         // Muted purple

    // Dominant colors (60% - main backgrounds)
    darkColors_["background"] = QColor("#1A1B26");        // Deep navy - main bg
    darkColors_["windowBackground"] = QColor("#16161E");  // Darker navy - window bg
    darkColors_["toolbarBackground"] = QColor("#1A1B26"); // Toolbar matches main
    darkColors_["statusBarBackground"] = QColor("#16161E"); // Status bar darker
    darkColors_["menuBackground"] = QColor("#1A1B26");    // Menu matches main
    darkColors_["tabBackground"] = QColor("#1A1B26");     // Tab inactive

    // Secondary colors (30% - panels, cards, elevated surfaces)
    darkColors_["cardBackground"] = QColor("#24283B");    // Medium navy - cards
    darkColors_["headerBackground"] = QColor("#1F2335");  // Header slightly lighter
    darkColors_["inputBackground"] = QColor("#1F2335");   // Input fields
    darkColors_["alternateBackground"] = QColor("#1F2335"); // Alternate row
    darkColors_["hoverBackground"] = QColor("#2D3450");   // Hover state
    darkColors_["disabledBackground"] = QColor("#1F2335"); // Disabled bg

    // Text colors
    darkColors_["text"] = QColor("#C0CAF5");              // Soft white
    darkColors_["textSecondary"] = QColor("#9AA5CE");     // Muted blue-gray
    darkColors_["headerText"] = QColor("#C0CAF5");        // Header text
    darkColors_["menuText"] = QColor("#C0CAF5");          // Menu text
    darkColors_["statusBarText"] = QColor("#9AA5CE");     // Status bar text
    darkColors_["tabText"] = QColor("#9AA5CE");           // Tab text
    darkColors_["disabledText"] = QColor("#565F89");      // Disabled text

    // Borders and separators
    darkColors_["border"] = QColor("#292E42");            // Subtle borders

    // UI elements
    darkColors_["progressBackground"] = QColor("#1F2335"); // Progress bg
    darkColors_["scrollBarBackground"] = QColor("#16161E"); // Scrollbar bg
    darkColors_["scrollBarHandle"] = QColor("#3B4261");   // Scrollbar handle
    darkColors_["toolbarBackground"] = QColor("#2C2C2C"); // Toolbar bg
    darkColors_["statusBarBackground"] = QColor("#1E1E1E"); // Status bar bg
    darkColors_["statusBarText"] = QColor("#B0B0B0");     // Status bar text
    darkColors_["progressBackground"] = QColor("#424242"); // Progress bg
    darkColors_["scrollBarBackground"] = QColor("#2C2C2C"); // Scrollbar bg
    darkColors_["scrollBarHandle"] = QColor("#616161");   // Scrollbar handle
    darkColors_["tabBackground"] = QColor("#2C2C2C");     // Tab bg
    darkColors_["tabText"] = QColor("#B0B0B0");           // Tab text
}

QMap<QString, QColor> StyleManager::getThemeColors(Theme theme) const
{
    return (theme == Light) ? lightColors_ : darkColors_;
}
