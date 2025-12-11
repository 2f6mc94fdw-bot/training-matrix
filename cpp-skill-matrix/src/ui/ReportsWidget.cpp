#include "ReportsWidget.h"
#include "../utils/Logger.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QGroupBox>
#include <QDateTime>

ReportsWidget::ReportsWidget(QWidget* parent)
    : QWidget(parent)
    , reportDisplay_(nullptr)
    , engineerReportButton_(nullptr)
    , areaReportButton_(nullptr)
    , complianceReportButton_(nullptr)
{
    setupUI();
    Logger::instance().info("ReportsWidget", "Reports widget initialized");
}

ReportsWidget::~ReportsWidget()
{
}

void ReportsWidget::setupUI()
{
    QVBoxLayout* mainLayout = new QVBoxLayout(this);

    // Title
    QLabel* titleLabel = new QLabel("Reports Generation", this);
    QFont titleFont = titleLabel->font();
    titleFont.setPointSize(16);
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);
    mainLayout->addWidget(titleLabel);

    // Report Options
    QGroupBox* optionsBox = new QGroupBox("Available Reports", this);
    QVBoxLayout* optionsLayout = new QVBoxLayout(optionsBox);

    engineerReportButton_ = new QPushButton("Engineer Skills Report", this);
    areaReportButton_ = new QPushButton("Production Area Coverage Report", this);
    complianceReportButton_ = new QPushButton("Compliance & Certification Report", this);

    connect(engineerReportButton_, &QPushButton::clicked, this, &ReportsWidget::onGenerateEngineerReportClicked);
    connect(areaReportButton_, &QPushButton::clicked, this, &ReportsWidget::onGenerateAreaReportClicked);
    connect(complianceReportButton_, &QPushButton::clicked, this, &ReportsWidget::onGenerateComplianceReportClicked);

    optionsLayout->addWidget(engineerReportButton_);
    optionsLayout->addWidget(areaReportButton_);
    optionsLayout->addWidget(complianceReportButton_);

    mainLayout->addWidget(optionsBox);

    // Report Display
    QLabel* displayLabel = new QLabel("Report Preview:", this);
    mainLayout->addWidget(displayLabel);

    reportDisplay_ = new QTextEdit(this);
    reportDisplay_->setReadOnly(true);
    reportDisplay_->setPlainText("Select a report type above to generate...");

    mainLayout->addWidget(reportDisplay_);

    setLayout(mainLayout);
}

void ReportsWidget::onGenerateEngineerReportClicked()
{
    QString report = "ENGINEER SKILLS REPORT\n";
    report += "======================\n\n";
    report += "Generated: " + QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss") + "\n\n";
    report += "This report shows the skill levels of all engineers across different competencies.\n\n";
    report += "[Report generation placeholder - implement using EngineerRepository and AssessmentRepository]\n\n";
    report += "Export options: PDF, Excel (Coming Soon)";

    reportDisplay_->setPlainText(report);
    Logger::instance().info("ReportsWidget", "Generated engineer skills report");
}

void ReportsWidget::onGenerateAreaReportClicked()
{
    QString report = "PRODUCTION AREA COVERAGE REPORT\n";
    report += "================================\n\n";
    report += "Generated: " + QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss") + "\n\n";
    report += "This report shows which production areas have adequate coverage and which need more trained personnel.\n\n";
    report += "[Report generation placeholder - implement using ProductionRepository and AssessmentRepository]\n\n";
    report += "Export options: PDF, Excel (Coming Soon)";

    reportDisplay_->setPlainText(report);
    Logger::instance().info("ReportsWidget", "Generated production area coverage report");
}

void ReportsWidget::onGenerateComplianceReportClicked()
{
    QString report = "COMPLIANCE & CERTIFICATION REPORT\n";
    report += "==================================\n\n";
    report += "Generated: " + QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss") + "\n\n";
    report += "This report shows certification status and expiry dates.\n\n";
    report += "[Report generation placeholder - implement using CertificationRepository]\n\n";
    report += "Export options: PDF, Excel (Coming Soon)";

    reportDisplay_->setPlainText(report);
    Logger::instance().info("ReportsWidget", "Generated compliance report");
}
