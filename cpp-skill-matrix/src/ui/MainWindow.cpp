#include "MainWindow.h"
#include "../core/Application.h"
#include "../core/DataCache.h"
#include "../database/DatabaseManager.h"
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
#include "CoreSkillsManagementWidget.h"
#include "MyCoreSkillsWidget.h"
#include "MyAssessmentsWidget.h"
#include "MyDashboardWidget.h"
#include "MyCertificationsWidget.h"
#include "MyProgressWidget.h"
#include "ReportsWidget.h"
#include "AnalyticsWidget.h"
#include "ManagerReviewWidget.h"
#include "NotificationsWidget.h"
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
#include <QTabWidget>

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
    navigationList_->setObjectName("navigationList");
    navigationList_->setMaximumWidth(Constants::SIDEBAR_WIDTH);
    navigationList_->setMinimumWidth(Constants::SIDEBAR_WIDTH);

    // Get current user's role from session
    Session* session = Application::instance().session();
    bool isAdmin = session && session->isAdmin();

    if (isAdmin) {
        navigationList_->addItem("Overview");
        navigationList_->addItem("Team");
        navigationList_->addItem("Assessments");
        navigationList_->addItem("Approval Queue");
        navigationList_->addItem("Skills Structure");
        navigationList_->addItem("Development & Insights");
        navigationList_->addItem("Compliance");
        navigationList_->addItem("Action Inbox");
        navigationList_->addItem("System Administration");
    } else {
        navigationList_->addItem("Home");
        navigationList_->addItem("My Assessments");
        navigationList_->addItem("My Development");
        navigationList_->addItem("My Qualifications");
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
        // PERFORMANCE: Use lazy initialization - create widgets only when first accessed
        // Initialize all widget pointers to nullptr
        engineersWidget_ = nullptr;
        usersWidget_ = nullptr;
        productionAreasWidget_ = nullptr;
        assessmentWidget_ = nullptr;
        coreSkillsWidget_ = nullptr;
        coreSkillsManagementWidget_ = nullptr;
        reportsWidget_ = nullptr;
        analyticsWidget_ = nullptr;
        managerReviewWidget_ = nullptr;
        notificationsWidget_ = nullptr;
        certificationsWidget_ = nullptr;
        snapshotsWidget_ = nullptr;
        auditLogWidget_ = nullptr;
        importExportWidget_ = nullptr;

        // Create only the overview initially. The remaining workflow pages are lazy-loaded.
        dashboardWidget_ = new DashboardWidget(this);

        contentStack_->addWidget(dashboardWidget_);   // 0 - Overview
        contentStack_->addWidget(new QWidget(this));  // 1 - Team
        contentStack_->addWidget(new QWidget(this));  // 2 - Assessments
        contentStack_->addWidget(new QWidget(this));  // 3 - Approval Queue
        contentStack_->addWidget(new QWidget(this));  // 4 - Skills Structure
        contentStack_->addWidget(new QWidget(this));  // 5 - Development & Insights
        contentStack_->addWidget(new QWidget(this));  // 6 - Compliance
        contentStack_->addWidget(new QWidget(this));  // 7 - Action Inbox
        contentStack_->addWidget(new QWidget(this));  // 8 - System Administration
    } else {
        // Engineer workspaces are compact enough to create together and share one engineer scope.
        QString engineerId = session->engineerId();

        MyDashboardWidget* myDashboard = new MyDashboardWidget(engineerId, this);
        contentStack_->addWidget(myDashboard);

        QTabWidget* assessmentsWorkspace = new QTabWidget(this);
        assessmentsWorkspace->setDocumentMode(true);
        MyAssessmentsWidget* myAssessments = new MyAssessmentsWidget(engineerId, this);
        MyCoreSkillsWidget* myCoreSkills = new MyCoreSkillsWidget(engineerId, this);
        assessmentsWorkspace->addTab(myAssessments, "Production Skills");
        assessmentsWorkspace->addTab(myCoreSkills, "Core Skills");
        contentStack_->addWidget(assessmentsWorkspace);

        MyProgressWidget* myProgress = new MyProgressWidget(engineerId, this);
        contentStack_->addWidget(myProgress);

        MyCertificationsWidget* myCerts = new MyCertificationsWidget(engineerId, this);
        contentStack_->addWidget(myCerts);
    }

    // Layout
    mainLayout->addWidget(navigationList_);
    mainLayout->addWidget(contentStack_, 1);

    setCentralWidget(centralWidget);
    navigationList_->setCurrentRow(0);
}

void MainWindow::setupStatusBar()
{
    QLabel* statusLabel = new QLabel("Ready", this);
    statusBar()->addWidget(statusLabel);

    // Add database connection status indicator
    QLabel* dbStatusLabel = new QLabel(this);

    // Check database connection status
    bool isConnected = DatabaseManager::instance().isConnected();
    if (isConnected) {
        dbStatusLabel->setText("Database: Connected");
        dbStatusLabel->setStyleSheet("QLabel { color: green; font-weight: bold; }");
    } else {
        dbStatusLabel->setText("Database: Disconnected");
        dbStatusLabel->setStyleSheet("QLabel { color: red; font-weight: bold; }");
    }

    statusBar()->addPermanentWidget(dbStatusLabel);

    // Connect to database connection changes
    connect(&DatabaseManager::instance(), &DatabaseManager::connectionChanged,
            this, [dbStatusLabel](bool connected) {
        if (connected) {
            dbStatusLabel->setText("Database: Connected");
            dbStatusLabel->setStyleSheet("QLabel { color: green; font-weight: bold; }");
        } else {
            dbStatusLabel->setText("Database: Disconnected");
            dbStatusLabel->setStyleSheet("QLabel { color: red; font-weight: bold; }");
        }
    });
}

void MainWindow::onNavigationItemClicked(int index)
{
    // PERFORMANCE: Lazy load widgets on first access
    Session* session = Application::instance().session();
    bool isAdmin = session && session->isAdmin();

    if (isAdmin) {
        // Check if we need to create the widget for this index
        QWidget* currentWidget = contentStack_->widget(index);

        // If it's a placeholder QWidget (not a specialized widget), create the real widget
        if (currentWidget && QString(currentWidget->metaObject()->className()) == "QWidget") {
            QWidget* newWidget = nullptr;

            switch (index) {
                case 1: { // Team
                    QTabWidget* tabs = new QTabWidget(this);
                    tabs->setDocumentMode(true);
                    engineersWidget_ = new EngineersWidget(tabs);
                    usersWidget_ = new UsersWidget(tabs);
                    tabs->addTab(engineersWidget_, "Engineers");
                    tabs->addTab(usersWidget_, "User Accounts");
                    newWidget = tabs;
                    Logger::instance().debug("MainWindow", "Lazy-loaded Team workspace");
                    break;
                }
                case 2: { // Assessments
                    QTabWidget* tabs = new QTabWidget(this);
                    tabs->setDocumentMode(true);
                    assessmentWidget_ = new AssessmentWidget(tabs);
                    coreSkillsWidget_ = new CoreSkillsWidget(tabs);
                    tabs->addTab(assessmentWidget_, "Production Skills");
                    tabs->addTab(coreSkillsWidget_, "Core Skills");
                    newWidget = tabs;
                    Logger::instance().debug("MainWindow", "Lazy-loaded Assessments workspace");
                    break;
                }
                case 3: // Approval Queue
                    managerReviewWidget_ = new ManagerReviewWidget(this);
                    newWidget = managerReviewWidget_;
                    Logger::instance().debug("MainWindow", "Lazy-loaded Approval Queue");
                    break;
                case 4: { // Skills Structure
                    QTabWidget* tabs = new QTabWidget(this);
                    tabs->setDocumentMode(true);
                    productionAreasWidget_ = new ProductionAreasWidget(tabs);
                    coreSkillsManagementWidget_ = new CoreSkillsManagementWidget(tabs);
                    tabs->addTab(productionAreasWidget_, "Production Structure");
                    tabs->addTab(coreSkillsManagementWidget_, "Core Skills Structure");
                    newWidget = tabs;
                    Logger::instance().debug("MainWindow", "Lazy-loaded Skills Structure workspace");
                    break;
                }
                case 5: { // Development & Insights
                    QTabWidget* tabs = new QTabWidget(this);
                    tabs->setDocumentMode(true);
                    analyticsWidget_ = new AnalyticsWidget(tabs);
                    reportsWidget_ = new ReportsWidget(tabs);
                    tabs->addTab(analyticsWidget_, "Coverage & Development");
                    tabs->addTab(reportsWidget_, "Reports & Export");
                    newWidget = tabs;
                    Logger::instance().debug("MainWindow", "Lazy-loaded Development & Insights workspace");
                    break;
                }
                case 6: // Compliance
                    certificationsWidget_ = new CertificationsWidget(this);
                    newWidget = certificationsWidget_;
                    Logger::instance().debug("MainWindow", "Lazy-loaded Compliance workspace");
                    break;
                case 7: // Action Inbox
                    notificationsWidget_ = new NotificationsWidget(this);
                    newWidget = notificationsWidget_;
                    Logger::instance().debug("MainWindow", "Lazy-loaded Action Inbox");
                    break;
                case 8: { // System Administration
                    QTabWidget* tabs = new QTabWidget(this);
                    tabs->setDocumentMode(true);
                    auditLogWidget_ = new AuditLogWidget(tabs);
                    snapshotsWidget_ = new SnapshotsWidget(tabs);
                    tabs->addTab(auditLogWidget_, "Audit Log");
                    tabs->addTab(snapshotsWidget_, "Snapshots");
                    newWidget = tabs;
                    Logger::instance().debug("MainWindow", "Lazy-loaded System Administration workspace");
                    break;
                }
            }

            // Replace placeholder with real widget
            if (newWidget) {
                contentStack_->removeWidget(currentWidget);
                delete currentWidget;
                contentStack_->insertWidget(index, newWidget);
            }
        }
    }

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
