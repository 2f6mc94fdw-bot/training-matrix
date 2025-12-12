#include "Application.h"
#include "Constants.h"
#include "Session.h"
#include "../ui/MainWindow.h"
#include "../ui/LoginDialog.h"
#include "../ui/StyleManager.h"
#include "../database/DatabaseManager.h"
#include "../utils/Logger.h"
#include "../utils/Config.h"
#include "../utils/IconProvider.h"

#include <QMessageBox>
#include <QFile>
#include <QTextStream>
#include <QDir>
#include <QStandardPaths>

Application& Application::instance()
{
    static Application instance;
    return instance;
}

Application::Application(QObject* parent)
    : QObject(parent)
    , qApp_(nullptr)
    , session_(nullptr)
    , mainWindow_(nullptr)
    , settings_(nullptr)
    , debugMode_(false)
    , currentTheme_(Constants::THEME_LIGHT)
{
}

Application::~Application()
{
    shutdown();
}

bool Application::initialize(int argc, char* argv[])
{
    // Create Qt application
    qApp_ = new QApplication(argc, argv);

    // Add Qt plugin paths for SQL drivers
    // This is needed when app is launched from Dock/Finder
    QCoreApplication::addLibraryPath("/opt/homebrew/Cellar/qtbase/6.9.3_1/share/qt/plugins");
    QCoreApplication::addLibraryPath("/opt/homebrew/opt/qtbase/share/qt/plugins");

    Logger::instance().info("Application", QString("Qt plugin paths: %1").arg(
        QCoreApplication::libraryPaths().join(", ")));

    // Setup application metadata
    setupApplication();

    // Create settings
    settings_ = new QSettings(
        QSettings::IniFormat,
        QSettings::UserScope,
        Constants::APP_ORGANIZATION,
        Constants::APP_NAME
    );

    // Create session
    session_ = new Session(this);

    // Load settings
    loadSettings();

    // Initialize logger
    Logger::instance().initialize();
    Logger::instance().info("Application", "Starting Skill Matrix v" + version());

    // Check for debug mode
    for (int i = 1; i < argc; ++i) {
        QString arg = QString::fromUtf8(argv[i]);
        if (arg == "--debug" || arg == "-d") {
            debugMode_ = true;
            Logger::instance().setLevel(Logger::Debug);
            Logger::instance().debug("Application", "Debug mode enabled");
        }
    }

    // Initialize StyleManager and IconProvider
    StyleManager::instance().initialize();
    IconProvider::instance().initialize();

    // Apply theme through StyleManager
    StyleManager::Theme theme = (currentTheme_ == Constants::THEME_DARK) ?
        StyleManager::Dark : StyleManager::Light;
    StyleManager::instance().applyTheme(theme);

    // Initialize database
    DatabaseManager& dbManager = DatabaseManager::instance();

    // Load database configuration from config file
    Config& config = Config::instance();
    config.load();

    // Connect to database using config
    if (config.has("database.server")) {
        QString server = config.databaseServer();
        QString database = config.databaseName();
        QString user = config.databaseUser();
        QString password = config.databasePassword();
        int port = config.databasePort();

        if (!dbManager.connect(server, database, user, password, port)) {
            Logger::instance().warning("Application", "Failed to connect to database on startup");
        }
    } else {
        Logger::instance().warning("Application", "No database configuration found");
    }

    Logger::instance().info("Application", "Initialization complete");
    return true;
}

int Application::run()
{
    // Show login dialog
    LoginDialog loginDialog;
    if (loginDialog.exec() != QDialog::Accepted) {
        Logger::instance().info("Application", "Login cancelled - exiting");
        return 0;
    }

    // Create and show main window
    mainWindow_ = new MainWindow();
    mainWindow_->show();

    Logger::instance().info("Application", "Main window displayed");

    // Run event loop
    return qApp_->exec();
}

void Application::shutdown()
{
    Logger::instance().info("Application", "Shutting down");

    // Save settings
    saveSettings();

    // Clean up
    if (mainWindow_) {
        delete mainWindow_;
        mainWindow_ = nullptr;
    }

    // Disconnect from database
    DatabaseManager::instance().disconnect();

    // Close logger
    Logger::instance().close();

    if (settings_) {
        delete settings_;
        settings_ = nullptr;
    }

    if (session_) {
        delete session_;
        session_ = nullptr;
    }

    if (qApp_) {
        qApp_->quit();
        delete qApp_;
        qApp_ = nullptr;
    }
}

QString Application::version() const
{
    return QString(Constants::APP_VERSION);
}

QString Application::applicationName() const
{
    return QString(Constants::APP_NAME);
}

void Application::showError(const QString& title, const QString& message)
{
    Logger::instance().error("Application", title + ": " + message);
    QMessageBox::critical(mainWindow_, title, message);
}

void Application::showInfo(const QString& title, const QString& message)
{
    Logger::instance().info("Application", title + ": " + message);
    QMessageBox::information(mainWindow_, title, message);
}

void Application::showWarning(const QString& title, const QString& message)
{
    Logger::instance().warning("Application", title + ": " + message);
    QMessageBox::warning(mainWindow_, title, message);
}

bool Application::askQuestion(const QString& title, const QString& message)
{
    QMessageBox::StandardButton reply = QMessageBox::question(
        mainWindow_,
        title,
        message,
        QMessageBox::Yes | QMessageBox::No
    );
    return reply == QMessageBox::Yes;
}

void Application::onUserLogin(const QString& userId, const QString& username, const QString& role)
{
    session_->setUserId(userId);
    session_->setUsername(username);
    session_->setRole(role);
    session_->setLoggedIn(true);

    Logger::instance().info("Application", "User logged in: " + username + " (" + role + ")");
    emit userLoggedIn();
}

void Application::onUserLogout()
{
    QString username = session_->username();

    session_->clear();

    Logger::instance().info("Application", "User logged out: " + username);
    emit userLoggedOut();

    // Close main window and show login again
    if (mainWindow_) {
        mainWindow_->close();
    }

    // Show login dialog
    LoginDialog loginDialog;
    if (loginDialog.exec() == QDialog::Accepted) {
        // Create new main window
        mainWindow_ = new MainWindow();
        mainWindow_->show();
    } else {
        // Exit application
        qApp_->quit();
    }
}

void Application::applyTheme(const QString& theme)
{
    currentTheme_ = theme;

    // Use StyleManager for theming
    StyleManager::Theme styleTheme = (theme == Constants::THEME_DARK) ?
        StyleManager::Dark : StyleManager::Light;
    StyleManager::instance().applyTheme(styleTheme);

    emit themeChanged(theme);
}

void Application::loadSettings()
{
    // Load theme
    currentTheme_ = settings_->value(Constants::SETTING_THEME, Constants::THEME_LIGHT).toString();

    // Load other settings as needed
    bool autoSave = settings_->value(Constants::SETTING_AUTO_SAVE, true).toBool();
    Q_UNUSED(autoSave); // Will be used later

    Logger::instance().debug("Application", "Settings loaded");
}

void Application::saveSettings()
{
    if (!settings_) {
        Logger::instance().warning("Application", "Cannot save settings - settings object is null");
        return;
    }

    // Save theme
    settings_->setValue(Constants::SETTING_THEME, currentTheme_);

    // Save window geometry and state
    if (mainWindow_) {
        settings_->setValue(Constants::SETTING_WINDOW_GEOMETRY, mainWindow_->saveGeometry());
        settings_->setValue(Constants::SETTING_WINDOW_STATE, mainWindow_->saveState());
    }

    settings_->sync();
    Logger::instance().debug("Application", "Settings saved");
}

void Application::setupApplication()
{
    QCoreApplication::setOrganizationName(Constants::APP_ORGANIZATION);
    QCoreApplication::setOrganizationDomain(Constants::APP_DOMAIN);
    QCoreApplication::setApplicationName(Constants::APP_NAME);
    QCoreApplication::setApplicationVersion(Constants::APP_VERSION);
}

void Application::loadStylesheet(const QString& theme)
{
    QString stylesheetPath;

    if (theme == Constants::THEME_DARK) {
        stylesheetPath = ":/styles/dark.qss";
    } else {
        stylesheetPath = ":/styles/light.qss";
    }

    QFile file(stylesheetPath);
    if (file.open(QFile::ReadOnly | QFile::Text)) {
        QTextStream stream(&file);
        QString stylesheet = stream.readAll();
        qApp_->setStyleSheet(stylesheet);
        file.close();
        Logger::instance().debug("Application", "Loaded stylesheet: " + theme);
    } else {
        // Fallback to default Qt style
        Logger::instance().warning("Application", "Failed to load stylesheet: " + stylesheetPath);
    }
}
