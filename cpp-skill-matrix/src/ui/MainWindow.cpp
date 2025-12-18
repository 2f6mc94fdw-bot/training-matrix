#include "MainWindow.h"
#include "../core/Application.h"
#include "../core/Constants.h"
#include "../utils/Logger.h"
#include "StyleManager.h"

// Widget includes
#include "DashboardWidget.h"
#include "EngineersWidget.h"
#include "UsersWidget.h"
#include "ProductionAreasWidget.h"
#include "AssessmentWidget.h"
#include "CoreSkillsWidget.h"
#include "ReportsWidget.h"
#include "AnalyticsWidget.h"
#include "CertificationsWidget.h"
#include "SnapshotsWidget.h"
#include "AuditLogWidget.h"
#include "ImportExportDialog.h"
#include "ChangePasswordDialog.h"

#include "../controllers/AuthController.h"

#include <QMenuBar>
#include <QStatusBar>
#include <QToolBar>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QCloseEvent>
#include <QSettings>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , navigationList_(nullptr)
    , contentStack_(nullptr)
    , dashboardWidget_(nullptr)
{
    setupUI();
    restoreSettings();

    Logger::instance().info("MainWindow", "Main window created");
}

MainWindow::~MainWindow()
{
    saveSettings();
}

void MainWindow::setupUI()
{
    setWindowTitle(Constants::APP_NAME);
    resize(Constants::DEFAULT_WINDOW_WIDTH, Constants::DEFAULT_WINDOW_HEIGHT);
    setMinimumSize(Constants::MIN_WINDOW_WIDTH, Constants::MIN_WINDOW_HEIGHT);

    setupMenuBar();
    setupCentralWidget();
    setupStatusBar();
}

void MainWindow::setupMenuBar()
{
    QMenuBar* menuBar = new QMenuBar(this);
    setMenuBar(menuBar);

    // File menu
    QMenu* fileMenu = menuBar->addMenu("&File");
    fileMenu->addAction("Change &Password", this, &MainWindow::onChangePasswordClicked);
    fileMenu->addSeparator();
    fileMenu->addAction("&Logout", this, &MainWindow::onLogoutClicked);
    fileMenu->addSeparator();
    fileMenu->addAction("E&xit", this, &MainWindow::close);

    // View menu
    QMenu* viewMenu = menuBar->addMenu("&View");
    viewMenu->addAction("Toggle &Theme", this, &MainWindow::onThemeToggled);

    // Help menu
    QMenu* helpMenu = menuBar->addMenu("&Help");
    helpMenu->addAction("&About", []() {
        QMessageBox::about(nullptr, "About Aptitude",
            QString("Aptitude v%1\n\nA comprehensive training and competency management system.\n\nEmpowering organizations to track, develop, and optimize workforce skills.")
            .arg(Constants::APP_VERSION));
    });
}

void MainWindow::setupNavigationSidebar()
{
    navigationList_ = new QListWidget(this);
    navigationList_->setMaximumWidth(Constants::SIDEBAR_WIDTH);
    navigationList_->setMinimumWidth(Constants::SIDEBAR_WIDTH);

    // Get current user's role from session
    Session* session = Application::instance().session();
    bool isAdmin = session && session->isAdmin();

    if (isAdmin) {
        // Admin navigation - full access to all features
        navigationList_->addItem("Dashboard");
        navigationList_->addItem("Engineers");
        navigationList_->addItem("Users");
        navigationList_->addItem("Production Areas");
        navigationList_->addItem("Assessments");
        navigationList_->addItem("Core Skills");
        navigationList_->addItem("Reports");
        navigationList_->addItem("Analytics");
        navigationList_->addItem("Certifications");
        navigationList_->addItem("Snapshots");
        navigationList_->addItem("Audit Log");
        navigationList_->addItem("Import/Export");
    } else {
        // Engineer navigation - personal view only
        navigationList_->addItem("My Dashboard");
        navigationList_->addItem("My Assessments");
        navigationList_->addItem("My Core Skills");
        navigationList_->addItem("My Certifications");
        navigationList_->addItem("My Progress");
    }

    connect(navigationList_, &QListWidget::currentRowChanged, this, &MainWindow::onNavigationItemClicked);
}

void MainWindow::setupCentralWidget()
{
    QWidget* centralWidget = new QWidget(this);
    QHBoxLayout* mainLayout = new QHBoxLayout(centralWidget);
    mainLayout->setSpacing(0);  // No spacing between sidebar and content (they have their own padding)
    mainLayout->setContentsMargins(0, 0, 0, 0);  // No margins on main layout - widgets control their own spacing

    // Setup navigation
    setupNavigationSidebar();

    // Setup content stack
    contentStack_ = new QStackedWidget(this);

    // Get current user's role from session
    Session* session = Application::instance().session();
    bool isAdmin = session && session->isAdmin();

    if (isAdmin) {
        // Admin widgets - full featured management interface
        dashboardWidget_ = new DashboardWidget(this);
        engineersWidget_ = new EngineersWidget(this);
        usersWidget_ = new UsersWidget(this);
        productionAreasWidget_ = new ProductionAreasWidget(this);
        assessmentWidget_ = new AssessmentWidget(this);
        coreSkillsWidget_ = new CoreSkillsWidget(this);
        reportsWidget_ = new ReportsWidget(this);
        analyticsWidget_ = new AnalyticsWidget(this);
        certificationsWidget_ = new CertificationsWidget(this);
        snapshotsWidget_ = new SnapshotsWidget(this);
        auditLogWidget_ = new AuditLogWidget(this);
        importExportWidget_ = new ImportExportDialog(this);

        // Add admin widgets to stack
        contentStack_->addWidget(dashboardWidget_);
        contentStack_->addWidget(engineersWidget_);
        contentStack_->addWidget(usersWidget_);
        contentStack_->addWidget(productionAreasWidget_);
        contentStack_->addWidget(assessmentWidget_);
        contentStack_->addWidget(coreSkillsWidget_);
        contentStack_->addWidget(reportsWidget_);
        contentStack_->addWidget(analyticsWidget_);
        contentStack_->addWidget(certificationsWidget_);
        contentStack_->addWidget(snapshotsWidget_);
        contentStack_->addWidget(auditLogWidget_);
        contentStack_->addWidget(importExportWidget_);
    } else {
        // Engineer widgets - personal view filtered by engineerId
        // For now, create placeholder widgets - will be replaced with engineer-specific widgets
        QString engineerId = session->engineerId();

        // My Dashboard - shows personal overview
        QWidget* myDashboard = new QLabel(QString("Welcome! Your Engineer ID: %1\n\nYour personal dashboard will show your skills, assessments, and targets here.").arg(engineerId), this);
        contentStack_->addWidget(myDashboard);

        // My Assessments - shows only their assessments
        QWidget* myAssessments = new QLabel("Your machine competency assessments will appear here.", this);
        contentStack_->addWidget(myAssessments);

        // My Core Skills - shows only their core skills
        QWidget* myCoreSkills = new QLabel("Your core skill assessments will appear here.", this);
        contentStack_->addWidget(myCoreSkills);

        // My Certifications - shows only their certifications
        QWidget* myCerts = new QLabel("Your certifications will appear here.", this);
        contentStack_->addWidget(myCerts);

        // My Progress - shows progress over time with targets
        QWidget* myProgress = new QLabel("Your progress history and targets will appear here.", this);
        contentStack_->addWidget(myProgress);
    }

    // Layout
    mainLayout->addWidget(navigationList_);
    mainLayout->addWidget(contentStack_, 1);

    setCentralWidget(centralWidget);
}

void MainWindow::setupStatusBar()
{
    QLabel* statusLabel = new QLabel("Ready", this);
    statusBar()->addWidget(statusLabel);

    // TODO: Add database connection status indicator
}

void MainWindow::onNavigationItemClicked(int index)
{
    contentStack_->setCurrentIndex(index);
    Logger::instance().debug("MainWindow", QString("Navigation changed to index %1").arg(index));
}

void MainWindow::onThemeToggled()
{
    StyleManager& styleManager = StyleManager::instance();

    // Toggle between Light and Dark themes
    StyleManager::Theme newTheme = (styleManager.currentTheme() == StyleManager::Light)
        ? StyleManager::Dark
        : StyleManager::Light;

    styleManager.applyTheme(newTheme);

    QString themeName = (newTheme == StyleManager::Light) ? "Light" : "Dark";
    Logger::instance().info("MainWindow", QString("Theme switched to: %1").arg(themeName));

    // Save theme preference immediately
    QSettings* settings = Application::instance().settings();
    if (settings) {
        QString themeSetting = (newTheme == StyleManager::Light)
            ? Constants::THEME_LIGHT
            : Constants::THEME_DARK;
        settings->setValue(Constants::SETTING_THEME, themeSetting);
        settings->sync();
    }

    // Show a brief message in the status bar
    statusBar()->showMessage(QString("Theme changed to %1 mode").arg(themeName), 3000);
}

void MainWindow::onChangePasswordClicked()
{
    ChangePasswordDialog dialog(this);

    if (dialog.exec() == QDialog::Accepted) {
        QString oldPassword = dialog.oldPassword();
        QString newPassword = dialog.newPassword();

        AuthController authController;
        bool success = authController.changePassword(oldPassword, newPassword);

        if (success) {
            QMessageBox::information(this, "Success",
                "Your password has been changed successfully.");
            Logger::instance().info("MainWindow", "Password changed via menu");
        } else {
            QMessageBox::critical(this, "Error",
                "Failed to change password. Please ensure your current password is correct.");
            Logger::instance().warning("MainWindow", "Password change failed via menu");
        }
    }
}

void MainWindow::onLogoutClicked()
{
    QMessageBox::StandardButton reply = QMessageBox::question(
        this,
        "Logout",
        "Are you sure you want to logout?",
        QMessageBox::Yes | QMessageBox::No
    );

    if (reply == QMessageBox::Yes) {
        Application::instance().onUserLogout();
    }
}

void MainWindow::closeEvent(QCloseEvent* event)
{
    QMessageBox::StandardButton reply = QMessageBox::question(
        this,
        "Exit",
        "Are you sure you want to exit?",
        QMessageBox::Yes | QMessageBox::No
    );

    if (reply == QMessageBox::Yes) {
        saveSettings();
        event->accept();
    } else {
        event->ignore();
    }
}

void MainWindow::restoreSettings()
{
    QSettings* settings = Application::instance().settings();
    if (settings) {
        restoreGeometry(settings->value(Constants::SETTING_WINDOW_GEOMETRY).toByteArray());
        restoreState(settings->value(Constants::SETTING_WINDOW_STATE).toByteArray());

        // Note: Theme is already restored and applied by Application::initialize()
    }
}

void MainWindow::saveSettings()
{
    QSettings* settings = Application::instance().settings();
    if (settings) {
        settings->setValue(Constants::SETTING_WINDOW_GEOMETRY, saveGeometry());
        settings->setValue(Constants::SETTING_WINDOW_STATE, saveState());

        // Save theme preference
        QString themeSetting = (StyleManager::instance().currentTheme() == StyleManager::Light)
            ? Constants::THEME_LIGHT
            : Constants::THEME_DARK;
        settings->setValue(Constants::SETTING_THEME, themeSetting);

        settings->sync();
        Logger::instance().debug("MainWindow", QString("Saved theme: %1").arg(themeSetting));
    }
}
