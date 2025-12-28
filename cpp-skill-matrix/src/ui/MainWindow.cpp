#include "MainWindow.h"
#include "../core/Application.h"
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
#include "MyProgressWidget.h"
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
        navigationList_->addItem("Core Skills Management");
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
        certificationsWidget_ = nullptr;
        snapshotsWidget_ = nullptr;
        auditLogWidget_ = nullptr;
        importExportWidget_ = nullptr;

        // Create only the dashboard widget initially (default view)
        dashboardWidget_ = new DashboardWidget(this);

        // Add placeholder widgets to stack (will be replaced with real widgets on first access)
        contentStack_->addWidget(dashboardWidget_);  // 0 - Dashboard
        contentStack_->addWidget(new QWidget(this)); // 1 - Engineers (lazy)
        contentStack_->addWidget(new QWidget(this)); // 2 - Users (lazy)
        contentStack_->addWidget(new QWidget(this)); // 3 - Production Areas (lazy)
        contentStack_->addWidget(new QWidget(this)); // 4 - Assessments (lazy)
        contentStack_->addWidget(new QWidget(this)); // 5 - Core Skills (lazy)
        contentStack_->addWidget(new QWidget(this)); // 6 - Core Skills Management (lazy)
        contentStack_->addWidget(new QWidget(this)); // 7 - Reports (lazy)
        contentStack_->addWidget(new QWidget(this)); // 8 - Analytics (lazy)
        contentStack_->addWidget(new QWidget(this)); // 9 - Certifications (lazy)
        contentStack_->addWidget(new QWidget(this)); // 10 - Snapshots (lazy)
        contentStack_->addWidget(new QWidget(this)); // 11 - Audit Log (lazy)
        contentStack_->addWidget(new QWidget(this)); // 12 - Import/Export (lazy)
    } else {
        // Engineer widgets - personal view filtered by engineerId
        // For now, create placeholder widgets - will be replaced with engineer-specific widgets
        QString engineerId = session->engineerId();

        // My Dashboard - shows personal overview
        MyDashboardWidget* myDashboard = new MyDashboardWidget(engineerId, this);
        contentStack_->addWidget(myDashboard);

        // My Assessments - shows only their assessments
        MyAssessmentsWidget* myAssessments = new MyAssessmentsWidget(engineerId, this);
        contentStack_->addWidget(myAssessments);

        // My Core Skills - shows only their core skills
        MyCoreSkillsWidget* myCoreSkills = new MyCoreSkillsWidget(engineerId, this);
        contentStack_->addWidget(myCoreSkills);

        // My Certifications - shows only their certifications
        QWidget* myCerts = new QLabel("Your certifications will appear here.", this);
        contentStack_->addWidget(myCerts);

        // My Progress - shows progress over time with targets
        MyProgressWidget* myProgress = new MyProgressWidget(engineerId, this);
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
                case 1: // Engineers
                    if (!engineersWidget_) {
                        engineersWidget_ = new EngineersWidget(this);
                        newWidget = engineersWidget_;
                        Logger::instance().debug("MainWindow", "Lazy-loaded Engineers widget");
                    }
                    break;
                case 2: // Users
                    if (!usersWidget_) {
                        usersWidget_ = new UsersWidget(this);
                        newWidget = usersWidget_;
                        Logger::instance().debug("MainWindow", "Lazy-loaded Users widget");
                    }
                    break;
                case 3: // Production Areas
                    if (!productionAreasWidget_) {
                        productionAreasWidget_ = new ProductionAreasWidget(this);
                        newWidget = productionAreasWidget_;
                        Logger::instance().debug("MainWindow", "Lazy-loaded Production Areas widget");
                    }
                    break;
                case 4: // Assessments
                    if (!assessmentWidget_) {
                        assessmentWidget_ = new AssessmentWidget(this);
                        newWidget = assessmentWidget_;
                        Logger::instance().debug("MainWindow", "Lazy-loaded Assessment widget");
                    }
                    break;
                case 5: // Core Skills
                    if (!coreSkillsWidget_) {
                        coreSkillsWidget_ = new CoreSkillsWidget(this);
                        newWidget = coreSkillsWidget_;
                        Logger::instance().debug("MainWindow", "Lazy-loaded Core Skills widget");
                    }
                    break;
                case 6: // Core Skills Management
                    if (!coreSkillsManagementWidget_) {
                        coreSkillsManagementWidget_ = new CoreSkillsManagementWidget(this);
                        newWidget = coreSkillsManagementWidget_;
                        Logger::instance().debug("MainWindow", "Lazy-loaded Core Skills Management widget");
                    }
                    break;
                case 7: // Reports
                    if (!reportsWidget_) {
                        reportsWidget_ = new ReportsWidget(this);
                        newWidget = reportsWidget_;
                        Logger::instance().debug("MainWindow", "Lazy-loaded Reports widget");
                    }
                    break;
                case 8: // Analytics
                    if (!analyticsWidget_) {
                        analyticsWidget_ = new AnalyticsWidget(this);

                        // Connect to import/export data change signal if import/export widget exists
                        if (importExportWidget_) {
                            connect(importExportWidget_, &ImportExportDialog::dataChanged,
                                    analyticsWidget_, &AnalyticsWidget::refresh);
                        }

                        newWidget = analyticsWidget_;
                        Logger::instance().debug("MainWindow", "Lazy-loaded Analytics widget");
                    }
                    break;
                case 9: // Certifications
                    if (!certificationsWidget_) {
                        certificationsWidget_ = new CertificationsWidget(this);
                        newWidget = certificationsWidget_;
                        Logger::instance().debug("MainWindow", "Lazy-loaded Certifications widget");
                    }
                    break;
                case 10: // Snapshots
                    if (!snapshotsWidget_) {
                        snapshotsWidget_ = new SnapshotsWidget(this);
                        newWidget = snapshotsWidget_;
                        Logger::instance().debug("MainWindow", "Lazy-loaded Snapshots widget");
                    }
                    break;
                case 11: // Audit Log
                    if (!auditLogWidget_) {
                        auditLogWidget_ = new AuditLogWidget(this);
                        newWidget = auditLogWidget_;
                        Logger::instance().debug("MainWindow", "Lazy-loaded Audit Log widget");
                    }
                    break;
                case 12: // Import/Export
                    if (!importExportWidget_) {
                        importExportWidget_ = new ImportExportDialog(this);

                        // Connect to dashboard and analytics if they exist
                        if (dashboardWidget_) {
                            connect(importExportWidget_, &ImportExportDialog::dataChanged,
                                    dashboardWidget_, &DashboardWidget::refresh);
                        }
                        if (analyticsWidget_) {
                            connect(importExportWidget_, &ImportExportDialog::dataChanged,
                                    analyticsWidget_, &AnalyticsWidget::refresh);
                        }

                        newWidget = importExportWidget_;
                        Logger::instance().debug("MainWindow", "Lazy-loaded Import/Export widget");
                    }
                    break;
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
