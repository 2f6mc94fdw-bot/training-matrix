#ifndef APPLICATION_H
#define APPLICATION_H

#include <QObject>
#include <QApplication>
#include <QString>
#include <QSettings>
#include "Session.h"

class DatabaseManager;
class MainWindow;

/**
 * @brief Application singleton class
 *
 * Manages the application lifecycle, global settings, and provides
 * access to key application components.
 */
class Application : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief Get the singleton instance
     * @return Reference to the Application instance
     */
    static Application& instance();

    /**
     * @brief Initialize the application
     * @param argc Command line argument count
     * @param argv Command line arguments
     * @return true if initialization succeeded
     */
    bool initialize(int argc, char* argv[]);

    /**
     * @brief Run the application event loop
     * @return Application exit code
     */
    int run();

    /**
     * @brief Shutdown the application
     */
    void shutdown();

    /**
     * @brief Get the Qt application instance
     * @return Pointer to QApplication
     */
    QApplication* qApplication() { return qApp_; }

    /**
     * @brief Get the current user session
     * @return Pointer to Session
     */
    Session* session() { return session_; }

    /**
     * @brief Get the main window
     * @return Pointer to MainWindow
     */
    MainWindow* mainWindow() { return mainWindow_; }

    /**
     * @brief Get application settings
     * @return Pointer to QSettings
     */
    QSettings* settings() { return settings_; }

    /**
     * @brief Check if application is running in debug mode
     * @return true if debug mode
     */
    bool isDebugMode() const { return debugMode_; }

    /**
     * @brief Get application version
     * @return Version string
     */
    QString version() const;

    /**
     * @brief Get application name
     * @return Application name
     */
    QString applicationName() const;

    /**
     * @brief Show error message dialog
     * @param title Dialog title
     * @param message Error message
     */
    void showError(const QString& title, const QString& message);

    /**
     * @brief Show information message dialog
     * @param title Dialog title
     * @param message Information message
     */
    void showInfo(const QString& title, const QString& message);

    /**
     * @brief Show warning message dialog
     * @param title Dialog title
     * @param message Warning message
     */
    void showWarning(const QString& title, const QString& message);

    /**
     * @brief Show question dialog
     * @param title Dialog title
     * @param message Question message
     * @return true if user clicked Yes/OK
     */
    bool askQuestion(const QString& title, const QString& message);

signals:
    /**
     * @brief Emitted when user logs in
     */
    void userLoggedIn();

    /**
     * @brief Emitted when user logs out
     */
    void userLoggedOut();

    /**
     * @brief Emitted when theme changes
     * @param theme New theme name
     */
    void themeChanged(const QString& theme);

    /**
     * @brief Emitted when database connection status changes
     * @param connected true if connected
     */
    void databaseConnectionChanged(bool connected);

public slots:
    /**
     * @brief Handle user login
     * @param userId User ID
     * @param username Username
     * @param role User role
     * @param engineerId Engineer ID (for engineer users)
     */
    void onUserLogin(const QString& userId, const QString& username, const QString& role, const QString& engineerId = QString());

    /**
     * @brief Handle user logout
     */
    void onUserLogout();

    /**
     * @brief Apply theme
     * @param theme Theme name ("light" or "dark")
     */
    void applyTheme(const QString& theme);

private:
    // Singleton - private constructor
    explicit Application(QObject* parent = nullptr);
    ~Application();

    // Delete copy constructor and assignment operator
    Application(const Application&) = delete;
    Application& operator=(const Application&) = delete;

    /**
     * @brief Load application settings
     */
    void loadSettings();

    /**
     * @brief Save application settings
     */
    void saveSettings();

    /**
     * @brief Setup application metadata
     */
    void setupApplication();

    /**
     * @brief Load stylesheet
     * @param theme Theme name
     */
    void loadStylesheet(const QString& theme);

private:
    QApplication* qApp_;
    Session* session_;
    MainWindow* mainWindow_;
    QSettings* settings_;
    bool debugMode_;
    QString currentTheme_;
};

#endif // APPLICATION_H
