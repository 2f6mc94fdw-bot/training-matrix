#include "AuditLogWidget.h"
#include "../utils/Logger.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>

AuditLogWidget::AuditLogWidget(QWidget* parent)
    : QWidget(parent)
    , auditTable_(nullptr)
    , refreshButton_(nullptr)
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
    connect(refreshButton_, &QPushButton::clicked, this, &AuditLogWidget::onRefreshClicked);

    buttonLayout->addStretch();
    buttonLayout->addWidget(refreshButton_);

    mainLayout->addLayout(buttonLayout);

    setLayout(mainLayout);
}

void AuditLogWidget::loadAuditLogs()
{
    auditTable_->setRowCount(0);

    QList<AuditLog> logs = auditLogRepo_.findAll(100); // Last 100 logs

    auditTable_->setRowCount(logs.size());

    for (int i = 0; i < logs.size(); ++i) {
        const AuditLog& log = logs[i];

        auditTable_->setItem(i, 0, new QTableWidgetItem(log.timestamp().toString("yyyy-MM-dd hh:mm:ss")));
        auditTable_->setItem(i, 1, new QTableWidgetItem(log.userId()));
        auditTable_->setItem(i, 2, new QTableWidgetItem(log.action()));
        auditTable_->setItem(i, 3, new QTableWidgetItem(log.details()));
    }

    Logger::instance().info("AuditLogWidget", QString("Loaded %1 audit log entries").arg(logs.size()));
}

void AuditLogWidget::onRefreshClicked()
{
    loadAuditLogs();
}
