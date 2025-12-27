#include "ImportExportDialog.h"
#include "../controllers/DataController.h"
#include "../utils/Logger.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QMessageBox>
#include <QFileDialog>

ImportExportDialog::ImportExportDialog(QWidget* parent)
    : QWidget(parent)
    , exportCSVButton_(nullptr)
    , exportJSONButton_(nullptr)
    , importCSVButton_(nullptr)
    , importJSONButton_(nullptr)
    , backupButton_(nullptr)
    , restoreButton_(nullptr)
    , statusDisplay_(nullptr)
{
    setupUI();
    Logger::instance().info("ImportExportDialog", "Import/Export widget initialized");
}

ImportExportDialog::~ImportExportDialog()
{
}

void ImportExportDialog::setupUI()
{
    QVBoxLayout* mainLayout = new QVBoxLayout(this);

    QLabel* titleLabel = new QLabel("Data Import/Export", this);
    QFont titleFont = titleLabel->font();
    titleFont.setPointSize(16);
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);
    mainLayout->addWidget(titleLabel);

    // Export Group
    QGroupBox* exportGroup = new QGroupBox("Export Data", this);
    QGridLayout* exportLayout = new QGridLayout(exportGroup);

    exportCSVButton_ = new QPushButton("Export to CSV", this);
    exportJSONButton_ = new QPushButton("Export to JSON", this);

    connect(exportCSVButton_, &QPushButton::clicked, this, &ImportExportDialog::onExportCSVClicked);
    connect(exportJSONButton_, &QPushButton::clicked, this, &ImportExportDialog::onExportJSONClicked);

    exportLayout->addWidget(exportCSVButton_, 0, 0);
    exportLayout->addWidget(exportJSONButton_, 0, 1);

    mainLayout->addWidget(exportGroup);

    // Import Group
    QGroupBox* importGroup = new QGroupBox("Import Data", this);
    QGridLayout* importLayout = new QGridLayout(importGroup);

    importCSVButton_ = new QPushButton("Import from CSV", this);
    importJSONButton_ = new QPushButton("Import from JSON", this);

    connect(importCSVButton_, &QPushButton::clicked, this, &ImportExportDialog::onImportCSVClicked);
    connect(importJSONButton_, &QPushButton::clicked, this, &ImportExportDialog::onImportJSONClicked);

    importLayout->addWidget(importCSVButton_, 0, 0);
    importLayout->addWidget(importJSONButton_, 0, 1);

    mainLayout->addWidget(importGroup);

    // Backup/Restore Group
    QGroupBox* backupGroup = new QGroupBox("Backup & Restore", this);
    QGridLayout* backupLayout = new QGridLayout(backupGroup);

    backupButton_ = new QPushButton("Create Backup", this);
    restoreButton_ = new QPushButton("Restore from Backup", this);

    connect(backupButton_, &QPushButton::clicked, this, &ImportExportDialog::onBackupClicked);
    connect(restoreButton_, &QPushButton::clicked, this, &ImportExportDialog::onRestoreClicked);

    backupLayout->addWidget(backupButton_, 0, 0);
    backupLayout->addWidget(restoreButton_, 0, 1);

    mainLayout->addWidget(backupGroup);

    // Test Data Group
    QGroupBox* testDataGroup = new QGroupBox("Test Data", this);
    QGridLayout* testDataLayout = new QGridLayout(testDataGroup);

    QPushButton* generateTestDataButton = new QPushButton("Generate Random Assessments", this);
    generateTestDataButton->setToolTip("Creates random assessment scores for all engineers to test analytics");

    connect(generateTestDataButton, &QPushButton::clicked, this, &ImportExportDialog::onGenerateTestDataClicked);

    testDataLayout->addWidget(generateTestDataButton, 0, 0);

    mainLayout->addWidget(testDataGroup);

    // Status Display
    QLabel* statusLabel = new QLabel("Operation Status:", this);
    mainLayout->addWidget(statusLabel);

    statusDisplay_ = new QTextEdit(this);
    statusDisplay_->setReadOnly(true);
    statusDisplay_->setMaximumHeight(150);
    statusDisplay_->setPlainText("Ready to import or export data...");

    mainLayout->addWidget(statusDisplay_);
    mainLayout->addStretch();

    setLayout(mainLayout);
}

void ImportExportDialog::onExportCSVClicked()
{
    QString fileName = QFileDialog::getSaveFileName(this,
        "Export to CSV", "", "CSV Files (*.csv);;All Files (*)");

    if (!fileName.isEmpty()) {
        statusDisplay_->setPlainText(QString("Exporting data to CSV: %1\n\n[Implementation placeholder - use ExcelExporter utility]").arg(fileName));
        Logger::instance().info("ImportExportDialog", "Exporting to CSV: " + fileName);
        QMessageBox::information(this, "Export", "CSV export functionality coming soon!");
    }
}

void ImportExportDialog::onExportJSONClicked()
{
    QString fileName = QFileDialog::getSaveFileName(this,
        "Export to JSON", "", "JSON Files (*.json);;All Files (*)");

    if (!fileName.isEmpty()) {
        statusDisplay_->setPlainText(QString("Exporting data to JSON: %1\n\n[Implementation placeholder - use JsonHelper utility]").arg(fileName));
        Logger::instance().info("ImportExportDialog", "Exporting to JSON: " + fileName);
        QMessageBox::information(this, "Export", "JSON export functionality coming soon!");
    }
}

void ImportExportDialog::onImportCSVClicked()
{
    QString fileName = QFileDialog::getOpenFileName(this,
        "Import from CSV", "", "CSV Files (*.csv);;All Files (*)");

    if (!fileName.isEmpty()) {
        statusDisplay_->setPlainText(QString("Importing data from CSV: %1\n\n[Implementation placeholder - use ExcelImporter utility]").arg(fileName));
        Logger::instance().info("ImportExportDialog", "Importing from CSV: " + fileName);
        QMessageBox::information(this, "Import", "CSV import functionality coming soon!");
    }
}

void ImportExportDialog::onImportJSONClicked()
{
    QString fileName = QFileDialog::getOpenFileName(this,
        "Import from JSON", "", "JSON Files (*.json);;All Files (*)");

    if (!fileName.isEmpty()) {
        statusDisplay_->setPlainText(QString("Importing data from JSON: %1\n\n[Implementation placeholder - use JsonHelper utility]").arg(fileName));
        Logger::instance().info("ImportExportDialog", "Importing from JSON: " + fileName);
        QMessageBox::information(this, "Import", "JSON import functionality coming soon!");
    }
}

void ImportExportDialog::onBackupClicked()
{
    QString fileName = QFileDialog::getSaveFileName(this,
        "Create Backup", "", "Backup Files (*.bak);;All Files (*)");

    if (!fileName.isEmpty()) {
        statusDisplay_->setPlainText(QString("Creating database backup: %1\n\n[Implementation placeholder - copy database file]").arg(fileName));
        Logger::instance().info("ImportExportDialog", "Creating backup: " + fileName);
        QMessageBox::information(this, "Backup", "Database backup functionality coming soon!");
    }
}

void ImportExportDialog::onRestoreClicked()
{
    QString fileName = QFileDialog::getOpenFileName(this,
        "Restore from Backup", "", "Backup Files (*.bak);;All Files (*)");

    if (!fileName.isEmpty()) {
        QMessageBox::StandardButton reply = QMessageBox::warning(
            this,
            "Restore Warning",
            "Restoring from backup will overwrite the current database. Are you sure?",
            QMessageBox::Yes | QMessageBox::No
        );

        if (reply == QMessageBox::Yes) {
            statusDisplay_->setPlainText(QString("Restoring database from: %1\n\n[Implementation placeholder - restore database file]").arg(fileName));
            Logger::instance().info("ImportExportDialog", "Restoring from backup: " + fileName);
            QMessageBox::information(this, "Restore", "Database restore functionality coming soon!");
        }
    }
}

void ImportExportDialog::onGenerateTestDataClicked()
{
    QMessageBox::StandardButton reply = QMessageBox::question(
        this,
        "Generate Test Data",
        "This will create random assessment scores for all engineers.\n\n"
        "Approximately 70% of competencies will be filled with random scores (0-3).\n"
        "Existing assessments will not be overwritten.\n\n"
        "Continue?",
        QMessageBox::Yes | QMessageBox::No
    );

    if (reply == QMessageBox::Yes) {
        statusDisplay_->setPlainText("Generating random assessments...\n\n");

        QString result = DataController::generateRandomAssessments(70);

        statusDisplay_->setPlainText(result);
        Logger::instance().info("ImportExportDialog", "Generated test data: " + result);

        // Emit signal to refresh all data-dependent widgets
        emit dataChanged();

        QMessageBox::information(this, "Success", result + "\n\nThe Dashboard and Analytics will now refresh automatically.");
    }
}
