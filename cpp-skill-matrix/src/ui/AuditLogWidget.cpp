#include "AuditLogWidget.h"
#include "../utils/Logger.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QLineEdit>
#include <QFileDialog>
#include <QFile>
#include <QDir>
#include <QTextStream>
#include <QMessageBox>

AuditLogWidget::AuditLogWidget(QWidget* parent)
    : QWidget(parent)
    , auditTable_(nullptr)
    , refreshButton_(nullptr)
    , exportCSVButton_(nullptr)
    , userFilterEdit_(nullptr)
    , actionFilterEdit_(nullptr)
    , searchFilterEdit_(nullptr)
{
    setupUI();
    loadAuditLogs();
    Logger::instance().info("AuditLogWidget", "Audit Log widget initialized");
}

AuditLogWidget::~AuditLogWidget()
{
}

void AuditLogWidget::setupUI()
{
    QVBoxLayout* mainLayout = new QVBoxLayout(this);

    QLabel* titleLabel = new QLabel("Audit Log Viewer", this);
    QFont titleFont = titleLabel->font();
    titleFont.setPointSize(16);
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);
    mainLayout->addWidget(titleLabel);

    QLabel* infoLabel = new QLabel("View all system activities and changes (read-only)", this);
    mainLayout->addWidget(infoLabel);

    QHBoxLayout* filterLayout = new QHBoxLayout();
    userFilterEdit_ = new QLineEdit(this);
    userFilterEdit_->setPlaceholderText("Filter user...");
    actionFilterEdit_ = new QLineEdit(this);
    actionFilterEdit_->setPlaceholderText("Filter action...");
    searchFilterEdit_ = new QLineEdit(this);
    searchFilterEdit_->setPlaceholderText("Search details...");
    filterLayout->addWidget(userFilterEdit_);
    filterLayout->addWidget(actionFilterEdit_);
    filterLayout->addWidget(searchFilterEdit_, 1);
    mainLayout->addLayout(filterLayout);

    auditTable_ = new QTableWidget(this);
    auditTable_->setColumnCount(4);
    auditTable_->setHorizontalHeaderLabels({"Timestamp", "User", "Action", "Details"});
    auditTable_->horizontalHeader()->setStretchLastSection(true);
    auditTable_->setSelectionBehavior(QAbstractItemView::SelectRows);
    auditTable_->setEditTriggers(QAbstractItemView::NoEditTriggers);
    auditTable_->setAlternatingRowColors(true);

    mainLayout->addWidget(auditTable_);

    QHBoxLayout* buttonLayout = new QHBoxLayout();

    refreshButton_ = new QPushButton("Refresh", this);
    exportCSVButton_ = new QPushButton("Export CSV", this);
    connect(refreshButton_, &QPushButton::clicked, this, &AuditLogWidget::onRefreshClicked);
    connect(exportCSVButton_, &QPushButton::clicked, this, &AuditLogWidget::onExportCSVClicked);
    connect(userFilterEdit_, &QLineEdit::textChanged, this, &AuditLogWidget::onFiltersChanged);
    connect(actionFilterEdit_, &QLineEdit::textChanged, this, &AuditLogWidget::onFiltersChanged);
    connect(searchFilterEdit_, &QLineEdit::textChanged, this, &AuditLogWidget::onFiltersChanged);

    buttonLayout->addStretch();
    buttonLayout->addWidget(exportCSVButton_);
    buttonLayout->addWidget(refreshButton_);

    mainLayout->addLayout(buttonLayout);

    setLayout(mainLayout);
}

void AuditLogWidget::loadAuditLogs()
{
    allLogs_ = auditLogRepo_.findAll(1000);
    applyFilters();
    Logger::instance().info("AuditLogWidget", QString("Loaded %1 audit log entries").arg(allLogs_.size()));
}

void AuditLogWidget::applyFilters()
{
    auditTable_->setRowCount(0);
    const QString userFilter = userFilterEdit_ ? userFilterEdit_->text().trimmed().toLower() : "";
    const QString actionFilter = actionFilterEdit_ ? actionFilterEdit_->text().trimmed().toLower() : "";
    const QString searchFilter = searchFilterEdit_ ? searchFilterEdit_->text().trimmed().toLower() : "";

    int row = 0;
    for (const AuditLog& log : allLogs_) {
        const QString user = log.userId().toLower();
        const QString action = log.action().toLower();
        const QString details = log.details().toLower();

        if (!userFilter.isEmpty() && !user.contains(userFilter)) {
            continue;
        }
        if (!actionFilter.isEmpty() && !action.contains(actionFilter)) {
            continue;
        }
        if (!searchFilter.isEmpty()) {
            const QString haystack = QString("%1 %2 %3").arg(user, action, details);
            if (!haystack.contains(searchFilter)) {
                continue;
            }
        }

        auditTable_->insertRow(row);
        auditTable_->setItem(row, 0, new QTableWidgetItem(log.timestamp().toString("yyyy-MM-dd hh:mm:ss")));
        auditTable_->setItem(row, 1, new QTableWidgetItem(log.userId()));
        auditTable_->setItem(row, 2, new QTableWidgetItem(log.action()));
        auditTable_->setItem(row, 3, new QTableWidgetItem(log.details()));
        row++;
    }
}

void AuditLogWidget::onRefreshClicked()
{
    loadAuditLogs();
}

void AuditLogWidget::onFiltersChanged()
{
    applyFilters();
}

void AuditLogWidget::onExportCSVClicked()
{
    const QString filename = QFileDialog::getSaveFileName(
        this,
        "Export Audit Log",
        QDir::homePath() + "/audit_log.csv",
        "CSV Files (*.csv)");

    if (filename.isEmpty()) {
        return;
    }

    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::critical(this, "Export Failed", "Could not write audit CSV file.");
        return;
    }

    QTextStream out(&file);
    out << "Timestamp,User,Action,Details\n";

    for (int row = 0; row < auditTable_->rowCount(); ++row) {
        auto esc = [](const QString& value) {
            QString safe = value;
            safe.replace("\"", "\"\"");
            return QString("\"%1\"").arg(safe);
        };

        const QString timestamp = auditTable_->item(row, 0) ? auditTable_->item(row, 0)->text() : "";
        const QString user = auditTable_->item(row, 1) ? auditTable_->item(row, 1)->text() : "";
        const QString action = auditTable_->item(row, 2) ? auditTable_->item(row, 2)->text() : "";
        const QString details = auditTable_->item(row, 3) ? auditTable_->item(row, 3)->text() : "";

        out << esc(timestamp) << ","
            << esc(user) << ","
            << esc(action) << ","
            << esc(details) << "\n";
    }

    file.close();
    QMessageBox::information(this, "Export Complete", "Audit log exported successfully.");
}
