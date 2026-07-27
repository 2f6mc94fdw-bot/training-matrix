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
    applyTheme(Light);  // Default to light theme for executive readability
    Logger::instance().info("StyleManager", "Style manager initialized with light theme");
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
    const QMap<QString, QColor> colors = getThemeColors(theme);
    return QString(
        "QWidget {"
        "    font-family: 'Manrope', 'Source Sans 3', 'Segoe UI', 'Helvetica Neue', Arial, sans-serif;"
        "    font-size: 14px;"
        "    color: %1;"
        "    background-color: %2;"
        "}"
        "QMainWindow { background-color: %3; }"
        "QLabel { color: %1; background: transparent; }"
        "QMenuBar { background-color: %4; color: %5; border-bottom: 1px solid %6; padding: 6px; font-weight: 600; }"
        "QMenuBar::item { padding: 8px 12px; border-radius: 6px; }"
        "QMenuBar::item:selected { background-color: %7; }"
        "QMenu { background-color: %8; color: %5; border: 1px solid %6; border-radius: 8px; padding: 6px; }"
        "QMenu::item { padding: 8px 16px; border-radius: 6px; }"
        "QMenu::item:selected { background-color: %9; color: white; }"
        "QStatusBar { background-color: %4; color: %10; border-top: 1px solid %6; }"
        "QPushButton {"
        "    background-color: %9;"
        "    color: white;"
        "    border: 1px solid %9;"
        "    border-radius: 8px;"
        "    padding: 8px 16px;"
        "    font-weight: 700;"
        "    min-height: 20px;"
        "}"
        "QPushButton:hover { background-color: %11; border-color: %11; }"
        "QPushButton:pressed { background-color: %12; border-color: %12; }"
        "QPushButton:disabled { background-color: %13; border-color: %13; color: %14; }"
        "QLineEdit, QTextEdit, QPlainTextEdit, QComboBox, QSpinBox, QDoubleSpinBox {"
        "    background-color: %8;"
        "    color: %1;"
        "    border: 1px solid %6;"
        "    border-radius: 8px;"
        "    padding: 8px 10px;"
        "    selection-background-color: %9;"
        "    selection-color: white;"
        "}"
        "QLineEdit:focus, QTextEdit:focus, QPlainTextEdit:focus, QComboBox:focus, QSpinBox:focus, QDoubleSpinBox:focus {"
        "    border: 2px solid %9;"
        "    padding: 7px 9px;"
        "}"
        "QComboBox::drop-down { border: none; width: 24px; }"
        "QComboBox::down-arrow { image: none; border-left: 5px solid transparent; border-right: 5px solid transparent; border-top: 6px solid %1; margin-right: 8px; }"
        "QComboBox QAbstractItemView { background-color: %8; color: %1; border: 1px solid %6; selection-background-color: %9; selection-color: white; }"
        "QGroupBox {"
        "    background-color: %8;"
        "    border: 1px solid %6;"
        "    border-radius: 12px;"
        "    margin-top: 10px;"
        "    padding: 16px;"
        "    padding-top: 24px;"
        "}"
        "QGroupBox::title {"
        "    subcontrol-origin: margin;"
        "    subcontrol-position: top left;"
        "    padding: 0 8px;"
        "    color: %15;"
        "    font-weight: 700;"
        "    text-transform: uppercase;"
        "    font-size: 12px;"
        "}"
        "QTableWidget, QTableView, QTreeWidget, QListWidget {"
        "    background-color: %8;"
        "    color: %1;"
        "    border: 1px solid %6;"
        "    border-radius: 10px;"
        "    gridline-color: %6;"
        "    alternate-background-color: %16;"
        "    selection-background-color: %9;"
        "    selection-color: white;"
        "}"
        "QTableWidget::item, QTreeWidget::item, QListWidget::item { padding: 6px; min-height: 28px; }"
        "QListWidget#navigationList {"
        "    background-color: %4;"
        "    color: %5;"
        "    border: none;"
        "    border-right: 1px solid %6;"
        "    padding: 8px;"
        "    outline: 0;"
        "}"
        "QListWidget#navigationList::item {"
        "    padding: 10px 12px;"
        "    border-radius: 8px;"
        "    margin-bottom: 4px;"
        "    color: %10;"
        "    font-weight: 600;"
        "}"
        "QListWidget#navigationList::item:selected {"
        "    background-color: %9;"
        "    color: white;"
        "}"
        "QListWidget#navigationList::item:hover:!selected {"
        "    background-color: %7;"
        "    color: %5;"
        "}"
        "QHeaderView::section {"
        "    background-color: %17;"
        "    color: %5;"
        "    border: none;"
        "    border-bottom: 1px solid %6;"
        "    padding: 10px 8px;"
        "    font-weight: 700;"
        "    font-size: 13px;"
        "}"
        "QTabWidget::pane { border: 1px solid %6; border-radius: 10px; background-color: %8; margin-top: 6px; }"
        "QTabBar::tab { background-color: %17; color: %10; border: 1px solid %6; border-bottom: none; border-top-left-radius: 8px; border-top-right-radius: 8px; padding: 8px 14px; margin-right: 4px; font-weight: 600; }"
        "QTabBar::tab:selected { background-color: %8; color: %5; border-bottom: 3px solid %9; }"
        "QScrollBar:vertical { background: %2; width: 12px; margin: 2px; }"
        "QScrollBar::handle:vertical { background: %18; border-radius: 6px; min-height: 24px; }"
        "QScrollBar::handle:vertical:hover { background: %10; }"
        "QScrollBar:horizontal { background: %2; height: 12px; margin: 2px; }"
        "QScrollBar::handle:horizontal { background: %18; border-radius: 6px; min-width: 24px; }"
        "QScrollBar::handle:horizontal:hover { background: %10; }"
        "QScrollBar::add-line, QScrollBar::sub-line { background: transparent; border: none; }"
    ).arg(
        colors["text"].name(),                 // 1
        colors["background"].name(),           // 2
        colors["windowBackground"].name(),     // 3
        colors["menuBackground"].name(),       // 4
        colors["menuText"].name(),             // 5
        colors["border"].name(),               // 6
        colors["hoverBackground"].name(),      // 7
        colors["cardBackground"].name(),       // 8
        colors["primary"].name(),              // 9
        colors["textSecondary"].name(),        // 10
        colors["primary"].lighter(108).name(), // 11
        colors["primary"].darker(112).name(),  // 12
        colors["disabledBackground"].name(),   // 13
        colors["disabledText"].name(),         // 14
        colors["accent"].name(),               // 15
        colors["alternateBackground"].name(),  // 16
        colors["headerBackground"].name(),     // 17
        colors["scrollBarHandle"].name()       // 18
    );
}

void StyleManager::initializeColors()
{
    // Clinical Executive Light Theme
    lightColors_["primary"] = QColor("#1D4ED8");
    lightColors_["secondary"] = QColor("#0EA5A4");
    lightColors_["accent"] = QColor("#0F172A");
    lightColors_["danger"] = QColor("#DC2626");
    lightColors_["warning"] = QColor("#F59E0B");
    lightColors_["success"] = QColor("#16A34A");
    lightColors_["background"] = QColor("#F8FAFC");
    lightColors_["windowBackground"] = QColor("#EEF2F7");
    lightColors_["cardBackground"] = QColor("#FFFFFF");
    lightColors_["text"] = QColor("#0F172A");
    lightColors_["textSecondary"] = QColor("#475569");
    lightColors_["border"] = QColor("#CBD5E1");
    lightColors_["alternateBackground"] = QColor("#F1F5F9");
    lightColors_["hoverBackground"] = QColor("#E2E8F0");
    lightColors_["headerBackground"] = QColor("#E2E8F0");
    lightColors_["headerText"] = QColor("#0F172A");
    lightColors_["inputBackground"] = QColor("#FFFFFF");
    lightColors_["disabledBackground"] = QColor("#E2E8F0");
    lightColors_["disabledText"] = QColor("#94A3B8");
    lightColors_["menuBackground"] = QColor("#EAF1FB");
    lightColors_["menuText"] = QColor("#0F172A");
    lightColors_["toolbarBackground"] = QColor("#FFFFFF");
    lightColors_["statusBarBackground"] = QColor("#E2E8F0");
    lightColors_["statusBarText"] = QColor("#334155");
    lightColors_["progressBackground"] = QColor("#CBD5E1");
    lightColors_["scrollBarBackground"] = QColor("#E2E8F0");
    lightColors_["scrollBarHandle"] = QColor("#94A3B8");
    lightColors_["tabBackground"] = QColor("#FFFFFF");
    lightColors_["tabText"] = QColor("#475569");

    // Professional Dark Variant
    darkColors_["primary"] = QColor("#3B82F6");
    darkColors_["secondary"] = QColor("#14B8A6");
    darkColors_["accent"] = QColor("#F8FAFC");
    darkColors_["danger"] = QColor("#EF4444");
    darkColors_["warning"] = QColor("#F59E0B");
    darkColors_["success"] = QColor("#22C55E");
    darkColors_["background"] = QColor("#0B1220");
    darkColors_["windowBackground"] = QColor("#0F172A");
    darkColors_["cardBackground"] = QColor("#111827");
    darkColors_["text"] = QColor("#E2E8F0");
    darkColors_["textSecondary"] = QColor("#94A3B8");
    darkColors_["border"] = QColor("#334155");
    darkColors_["alternateBackground"] = QColor("#172033");
    darkColors_["hoverBackground"] = QColor("#1E293B");
    darkColors_["headerBackground"] = QColor("#1E293B");
    darkColors_["headerText"] = QColor("#E2E8F0");
    darkColors_["inputBackground"] = QColor("#0F172A");
    darkColors_["disabledBackground"] = QColor("#1E293B");
    darkColors_["disabledText"] = QColor("#64748B");
    darkColors_["menuBackground"] = QColor("#0F172A");
    darkColors_["menuText"] = QColor("#E2E8F0");
    darkColors_["toolbarBackground"] = QColor("#111827");
    darkColors_["statusBarBackground"] = QColor("#0F172A");
    darkColors_["statusBarText"] = QColor("#94A3B8");
    darkColors_["progressBackground"] = QColor("#334155");
    darkColors_["scrollBarBackground"] = QColor("#1E293B");
    darkColors_["scrollBarHandle"] = QColor("#475569");
    darkColors_["tabBackground"] = QColor("#111827");
    darkColors_["tabText"] = QColor("#94A3B8");
}

QMap<QString, QColor> StyleManager::getThemeColors(Theme theme) const
{
    return (theme == Light) ? lightColors_ : darkColors_;
}
