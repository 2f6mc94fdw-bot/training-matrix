#include "MainWindow.h"
#include "../core/Application.h"
#include "../core/Constants.h"
#include "../utils/Logger.h"

// Widget includes
#include "DashboardWidget.h"
#include "EngineersWidget.h"
#include "ProductionAreasWidget.h"
#include "AssessmentWidget.h"
#include "CoreSkillsWidget.h"
#include "ReportsWidget.h"
#include "AnalyticsWidget.h"
#include "CertificationsWidget.h"
#include "SnapshotsWidget.h"
#include "AuditLogWidget.h"
#include "ImportExportDialog.h"

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
    fileMenu->addAction("&Logout", this, &MainWindow::onLogoutClicked);
    fileMenu->addSeparator();
    fileMenu->addAction("E&xit", this, &MainWindow::close);

    // View menu
    QMenu* viewMenu = menuBar->addMenu("&View");
    viewMenu->addAction("Toggle &Theme", this, &MainWindow::onThemeToggled);

    // Help menu
    QMenu* helpMenu = menuBar->addMenu("&Help");
    helpMenu->addAction("&About", []() {
        QMessageBox::about(nullptr, "About Skill Matrix",
            QString("Skill Matrix v%1\n\nA comprehensive training and competency management system.")
            .arg(Constants::APP_VERSION));
    });
}

void MainWindow::setupNavigationSidebar()
{
    navigationList_ = new QListWidget(this);
    navigationList_->setMaximumWidth(Constants::SIDEBAR_WIDTH);
    navigationList_->setMinimumWidth(Constants::SIDEBAR_WIDTH);

    // Add navigation items
    navigationList_->addItem("Dashboard");
    navigationList_->addItem("Engineers");
    navigationList_->addItem("Production Areas");
    navigationList_->addItem("Assessments");
    navigationList_->addItem("Core Skills");
    navigationList_->addItem("Reports");
    navigationList_->addItem("Analytics");
    navigationList_->addItem("Certifications");
    navigationList_->addItem("Snapshots");
    navigationList_->addItem("Audit Log");
    navigationList_->addItem("Import/Export");

    connect(navigationList_, &QListWidget::currentRowChanged, this, &MainWindow::onNavigationItemClicked);
}

void MainWindow::setupCentralWidget()
{
    QWidget* centralWidget = new QWidget(this);
    QHBoxLayout* mainLayout = new QHBoxLayout(centralWidget);

    // Setup navigation
    setupNavigationSidebar();

    // Setup content stack
    contentStack_ = new QStackedWidget(this);

    // Create real widgets
    dashboardWidget_ = new DashboardWidget(this);
    engineersWidget_ = new EngineersWidget(this);
    productionAreasWidget_ = new ProductionAreasWidget(this);
    assessmentWidget_ = new AssessmentWidget(this);
    coreSkillsWidget_ = new CoreSkillsWidget(this);
    reportsWidget_ = new ReportsWidget(this);
    analyticsWidget_ = new AnalyticsWidget(this);
    certificationsWidget_ = new CertificationsWidget(this);
    snapshotsWidget_ = new SnapshotsWidget(this);
    auditLogWidget_ = new AuditLogWidget(this);
    importExportWidget_ = new ImportExportDialog(this);

    // Add widgets to stack
    contentStack_->addWidget(dashboardWidget_);
    contentStack_->addWidget(engineersWidget_);
    contentStack_->addWidget(productionAreasWidget_);
    contentStack_->addWidget(assessmentWidget_);
    contentStack_->addWidget(coreSkillsWidget_);
    contentStack_->addWidget(reportsWidget_);
    contentStack_->addWidget(analyticsWidget_);
    contentStack_->addWidget(certificationsWidget_);
    contentStack_->addWidget(snapshotsWidget_);
    contentStack_->addWidget(auditLogWidget_);
    contentStack_->addWidget(importExportWidget_);

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
    // TODO: Implement theme toggle
    Logger::instance().debug("MainWindow", "Theme toggle requested");
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
    }
}

void MainWindow::saveSettings()
{
    QSettings* settings = Application::instance().settings();
    if (settings) {
        settings->setValue(Constants::SETTING_WINDOW_GEOMETRY, saveGeometry());
        settings->setValue(Constants::SETTING_WINDOW_STATE, saveState());
        settings->sync();
    }
}
