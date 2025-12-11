#include "MainWindow.h"
#include "../core/Application.h"
#include "../core/Constants.h"
#include "../utils/Logger.h"

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

    // Create placeholder widgets
    dashboardWidget_ = new QLabel("Dashboard (Coming Soon)", this);
    engineersWidget_ = new QLabel("Engineers Management (Coming Soon)", this);
    productionAreasWidget_ = new QLabel("Production Areas (Coming Soon)", this);
    assessmentWidget_ = new QLabel("Assessments (Coming Soon)", this);
    coreSkillsWidget_ = new QLabel("Core Skills (Coming Soon)", this);
    reportsWidget_ = new QLabel("Reports (Coming Soon)", this);
    analyticsWidget_ = new QLabel("Analytics (Coming Soon)", this);
    certificationsWidget_ = new QLabel("Certifications (Coming Soon)", this);
    snapshotsWidget_ = new QLabel("Snapshots (Coming Soon)", this);
    auditLogWidget_ = new QLabel("Audit Log (Coming Soon)", this);
    importExportWidget_ = new QLabel("Import/Export (Coming Soon)", this);

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
