#include "ReportsWidget.h"
#include "StyleManager.h"
#include "../utils/Logger.h"
#include "../utils/IconProvider.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QGroupBox>
#include <QDateTime>
#include <QFileDialog>
#include <QMessageBox>
#include <QPrinter>
#include <QPainter>
#include <QTextDocument>
#include <QFile>
#include <QTextStream>
#include <QMap>

ReportsWidget::ReportsWidget(QWidget* parent)
    : QWidget(parent)
    , reportTypeCombo_(nullptr)
    , reportDisplay_(nullptr)
    , generateButton_(nullptr)
    , exportPDFButton_(nullptr)
    , exportCSVButton_(nullptr)
    , printButton_(nullptr)
{
    setupUI();
    Logger::instance().info("ReportsWidget", "Reports widget initialized with full functionality");
}

ReportsWidget::~ReportsWidget()
{
}

void ReportsWidget::setupUI()
{
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(12);

    // Title
    QLabel* titleLabel = new QLabel("📄 Reports Generation", this);
    QFont titleFont = titleLabel->font();
    titleFont.setPointSize(20);
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);
    titleLabel->setStyleSheet("color: " + StyleManager::instance().getColor("primary").name() + ";");
    mainLayout->addWidget(titleLabel);

    QLabel* subtitleLabel = new QLabel("Generate comprehensive skill matrix and compliance reports", this);
    subtitleLabel->setStyleSheet("color: " + StyleManager::instance().getColor("textSecondary").name() + ";");
    mainLayout->addWidget(subtitleLabel);

    mainLayout->addSpacing(10);

    // Report Selection
    QGroupBox* selectionBox = new QGroupBox("Report Type", this);
    QHBoxLayout* selectionLayout = new QHBoxLayout(selectionBox);

    QLabel* typeLabel = new QLabel("Select Report:", this);
    selectionLayout->addWidget(typeLabel);

    reportTypeCombo_ = new QComboBox(this);
    reportTypeCombo_->addItem("📊 Engineer Skills Report", 1);
    reportTypeCombo_->addItem("🏭 Production Area Coverage Report", 2);
    reportTypeCombo_->addItem("🏆 Compliance & Certification Report", 3);
    reportTypeCombo_->addItem("📋 Detailed Skill Matrix Report", 4);
    selectionLayout->addWidget(reportTypeCombo_, 1);

    generateButton_ = new QPushButton(
        IconProvider::instance().getIcon(IconProvider::Refresh),
        "Generate Report",
        this
    );
    generateButton_->setStyleSheet(StyleManager::instance().getButtonStyle("primary"));
    connect(generateButton_, &QPushButton::clicked, this, &ReportsWidget::onGenerateReportClicked);
    selectionLayout->addWidget(generateButton_);

    mainLayout->addWidget(selectionBox);

    // Report Display
    QGroupBox* displayBox = new QGroupBox("Report Preview", this);
    QVBoxLayout* displayLayout = new QVBoxLayout(displayBox);

    reportDisplay_ = new QTextEdit(this);
    reportDisplay_->setReadOnly(true);
    reportDisplay_->setPlainText("Select a report type and click 'Generate Report' to view...\n\nAvailable Reports:\n\n"
                                  "• Engineer Skills Report: Shows all engineers with their competency levels\n"
                                  "• Production Area Coverage: Displays coverage statistics for each area\n"
                                  "• Compliance & Certification: Lists certification statuses and expiry dates\n"
                                  "• Detailed Skill Matrix: Comprehensive matrix of all skills and engineers");

    QFont monoFont("Courier New", 9);
    reportDisplay_->setFont(monoFont);
    reportDisplay_->setMinimumHeight(400);

    displayLayout->addWidget(reportDisplay_);
    mainLayout->addWidget(displayBox);

    // Export Options
    QHBoxLayout* exportLayout = new QHBoxLayout();
    exportLayout->addStretch();

    exportPDFButton_ = new QPushButton(
        IconProvider::instance().getIcon(IconProvider::Save),
        "Export to PDF",
        this
    );
    exportPDFButton_->setStyleSheet(StyleManager::instance().getButtonStyle("success"));
    exportPDFButton_->setEnabled(false);
    connect(exportPDFButton_, &QPushButton::clicked, this, &ReportsWidget::onExportPDFClicked);
    exportLayout->addWidget(exportPDFButton_);

    exportCSVButton_ = new QPushButton(
        IconProvider::instance().getIcon(IconProvider::Export),
        "Export to CSV",
        this
    );
    exportCSVButton_->setStyleSheet(StyleManager::instance().getButtonStyle("success"));
    exportCSVButton_->setEnabled(false);
    connect(exportCSVButton_, &QPushButton::clicked, this, &ReportsWidget::onExportCSVClicked);
    exportLayout->addWidget(exportCSVButton_);

    printButton_ = new QPushButton(
        IconProvider::instance().getIcon(IconProvider::Print),
        "Print Report",
        this
    );
    printButton_->setStyleSheet(StyleManager::instance().getButtonStyle("secondary"));
    printButton_->setEnabled(false);
    connect(printButton_, &QPushButton::clicked, this, &ReportsWidget::onPrintClicked);
    exportLayout->addWidget(printButton_);

    mainLayout->addLayout(exportLayout);

    setLayout(mainLayout);
}

void ReportsWidget::onGenerateReportClicked()
{
    int reportType = reportTypeCombo_->currentData().toInt();

    QString report;
    switch (reportType) {
        case 1:
            report = generateEngineerSkillsReport();
            break;
        case 2:
            report = generateProductionAreaCoverageReport();
            break;
        case 3:
            report = generateComplianceReport();
            break;
        case 4:
            report = generateDetailedSkillMatrixReport();
            break;
        default:
            report = "Invalid report type selected.";
            break;
    }

    currentReportContent_ = report;
    reportDisplay_->setPlainText(report);

    // Enable export buttons
    exportPDFButton_->setEnabled(true);
    exportCSVButton_->setEnabled(true);
    printButton_->setEnabled(true);

    Logger::instance().info("ReportsWidget",
        QString("Generated report type %1").arg(reportTypeCombo_->currentText()));
}

QString ReportsWidget::generateEngineerSkillsReport()
{
    QString report;
    QTextStream stream(&report);

    stream << "=" << QString("=").repeated(79) << "\n";
    stream << "  ENGINEER SKILLS REPORT\n";
    stream << "=" << QString("=").repeated(79) << "\n";
    stream << "Generated: " << QDateTime::currentDateTime().toString("dddd, MMMM d, yyyy h:mm AP") << "\n";
    stream << "=" << QString("=").repeated(79) << "\n\n";

    QList<Engineer> engineers = engineerRepo_.findAll();

    if (engineers.isEmpty()) {
        stream << "No engineers found in the system.\n";
        return report;
    }

    stream << "Total Engineers: " << engineers.size() << "\n\n";

    for (const Engineer& engineer : engineers) {
        stream << QString("-").repeated(79) << "\n";
        stream << "Engineer: " << engineer.name() << " (ID: " << engineer.id() << ")\n";
        stream << "Shift: " << engineer.shift() << "\n";
        stream << QString("-").repeated(79) << "\n";

        // Get assessments for this engineer
        QList<Assessment> assessments = assessmentRepo_.findAll();
        QList<Assessment> engineerAssessments;
        for (const Assessment& a : assessments) {
            if (a.engineerId() == engineer.id()) {
                engineerAssessments.append(a);
            }
        }

        if (engineerAssessments.isEmpty()) {
            stream << "  No assessments recorded.\n\n";
            continue;
        }

        // Group by production area
        QMap<int, QList<Assessment>> areaAssessments;
        for (const Assessment& a : engineerAssessments) {
            areaAssessments[a.productionAreaId()].append(a);
        }

        stream << "  Production Area Competencies:\n";
        for (auto it = areaAssessments.begin(); it != areaAssessments.end(); ++it) {
            ProductionArea area = productionRepo_.findAreaById(it.key());
            stream << "    Area: " << (area.name().isEmpty() ? QString::number(it.key()) : area.name()) << "\n";

            for (const Assessment& a : it.value()) {
                QString scoreText;
                switch (a.score()) {
                    case 0: scoreText = "Not Assessed"; break;
                    case 1: scoreText = "Basic"; break;
                    case 2: scoreText = "Intermediate"; break;
                    case 3: scoreText = "Advanced"; break;
                    default: scoreText = "Unknown"; break;
                }

                stream << "      - Competency " << a.competencyId()
                       << ": " << scoreText << " (" << a.score() << ")\n";
            }
        }

        // Calculate average score
        double totalScore = 0;
        for (const Assessment& a : engineerAssessments) {
            totalScore += a.score();
        }
        double avgScore = engineerAssessments.isEmpty() ? 0 : totalScore / engineerAssessments.size();

        stream << "  Total Assessments: " << engineerAssessments.size() << "\n";
        stream << "  Average Score: " << QString::number(avgScore, 'f', 2) << "\n";
        stream << "  Completion: " << QString::number((avgScore / 3.0) * 100, 'f', 1) << "%\n\n";
    }

    stream << "=" << QString("=").repeated(79) << "\n";
    stream << "END OF REPORT\n";
    stream << "=" << QString("=").repeated(79) << "\n";

    return report;
}

QString ReportsWidget::generateProductionAreaCoverageReport()
{
    QString report;
    QTextStream stream(&report);

    stream << "=" << QString("=").repeated(79) << "\n";
    stream << "  PRODUCTION AREA COVERAGE REPORT\n";
    stream << "=" << QString("=").repeated(79) << "\n";
    stream << "Generated: " << QDateTime::currentDateTime().toString("dddd, MMMM d, yyyy h:mm AP") << "\n";
    stream << "=" << QString("=").repeated(79) << "\n\n";

    QList<ProductionArea> areas = productionRepo_.findAllAreas();
    QList<Engineer> allEngineers = engineerRepo_.findAll();
    QList<Assessment> allAssessments = assessmentRepo_.findAll();

    if (areas.isEmpty()) {
        stream << "No production areas defined in the system.\n";
        return report;
    }

    stream << "Total Production Areas: " << areas.size() << "\n";
    stream << "Total Engineers: " << allEngineers.size() << "\n\n";

    for (const ProductionArea& area : areas) {
        stream << QString("-").repeated(79) << "\n";
        stream << "Area: " << area.name() << " (ID: " << area.id() << ")\n";
        stream << QString("-").repeated(79) << "\n";

        // Get machines for this area
        QList<Machine> machines = productionRepo_.findMachinesByArea(area.id());
        stream << "  Machines: " << machines.size() << "\n";

        // Count engineers assessed in this area
        QSet<QString> assessedEngineers;
        int totalAssessments = 0;
        double totalScore = 0;

        for (const Assessment& a : allAssessments) {
            if (a.productionAreaId() == area.id()) {
                assessedEngineers.insert(a.engineerId());
                totalAssessments++;
                totalScore += a.score();
            }
        }

        stream << "  Engineers with Assessments: " << assessedEngineers.size() << "\n";
        stream << "  Total Assessments: " << totalAssessments << "\n";

        if (totalAssessments > 0) {
            double avgScore = totalScore / totalAssessments;
            stream << "  Average Score: " << QString::number(avgScore, 'f', 2) << "\n";

            double coverage = (static_cast<double>(assessedEngineers.size()) / allEngineers.size()) * 100;
            stream << "  Coverage: " << QString::number(coverage, 'f', 1) << "%\n";

            // Status indicator
            QString status;
            if (coverage >= 80) status = "EXCELLENT";
            else if (coverage >= 60) status = "GOOD";
            else if (coverage >= 40) status = "FAIR";
            else status = "NEEDS IMPROVEMENT";

            stream << "  Status: " << status << "\n";
        } else {
            stream << "  Coverage: 0.0% (NO ASSESSMENTS)\n";
            stream << "  Status: CRITICAL - NO COVERAGE\n";
        }

        stream << "\n";
    }

    stream << "=" << QString("=").repeated(79) << "\n";
    stream << "SUMMARY\n";
    stream << "=" << QString("=").repeated(79) << "\n";
    stream << "Areas with Good Coverage (>=60%): ";

    int goodCoverageCount = 0;
    for (const ProductionArea& area : areas) {
        QSet<QString> assessed;
        for (const Assessment& a : allAssessments) {
            if (a.productionAreaId() == area.id()) {
                assessed.insert(a.engineerId());
            }
        }
        double coverage = allEngineers.isEmpty() ? 0 :
            (static_cast<double>(assessed.size()) / allEngineers.size()) * 100;
        if (coverage >= 60) goodCoverageCount++;
    }

    stream << goodCoverageCount << " / " << areas.size() << "\n";
    stream << "=" << QString("=").repeated(79) << "\n";

    return report;
}

QString ReportsWidget::generateComplianceReport()
{
    QString report;
    QTextStream stream(&report);

    stream << "=" << QString("=").repeated(79) << "\n";
    stream << "  COMPLIANCE & CERTIFICATION REPORT\n";
    stream << "=" << QString("=").repeated(79) << "\n";
    stream << "Generated: " << QDateTime::currentDateTime().toString("dddd, MMMM d, yyyy h:mm AP") << "\n";
    stream << "=" << QString("=").repeated(79) << "\n\n";

    QList<Certification> certifications = certificationRepo_.findAll();
    QList<Engineer> engineers = engineerRepo_.findAll();

    if (certifications.isEmpty()) {
        stream << "No certifications found in the system.\n";
        return report;
    }

    stream << "Total Certifications: " << certifications.size() << "\n";
    stream << "Total Engineers: " << engineers.size() << "\n\n";

    // Group certifications by engineer
    QMap<QString, QList<Certification>> engineerCerts;
    for (const Certification& cert : certifications) {
        engineerCerts[cert.engineerId()].append(cert);
    }

    int expiredCount = 0;
    int expiringSoonCount = 0;  // Within 30 days
    int validCount = 0;
    QDate today = QDate::currentDate();

    for (const Engineer& engineer : engineers) {
        stream << QString("-").repeated(79) << "\n";
        stream << "Engineer: " << engineer.name() << " (ID: " << engineer.id() << ")\n";
        stream << QString("-").repeated(79) << "\n";

        if (!engineerCerts.contains(engineer.id()) || engineerCerts[engineer.id()].isEmpty()) {
            stream << "  No certifications on record.\n\n";
            continue;
        }

        for (const Certification& cert : engineerCerts[engineer.id()]) {
            QString status;
            QDate expiryDate = cert.expiryDate();
            int daysUntilExpiry = today.daysTo(expiryDate);

            if (daysUntilExpiry < 0) {
                status = "EXPIRED";
                expiredCount++;
            } else if (daysUntilExpiry <= 30) {
                status = QString("EXPIRING SOON (%1 days)").arg(daysUntilExpiry);
                expiringSoonCount++;
            } else {
                status = "VALID";
                validCount++;
            }

            stream << "  - " << cert.name() << "\n";
            stream << "    Issued: " << cert.dateEarned().toString("yyyy-MM-dd") << "\n";
            stream << "    Expires: " << expiryDate.toString("yyyy-MM-dd") << "\n";
            stream << "    Status: " << status << "\n";
        }
        stream << "\n";
    }

    // Engineers without certifications
    QList<QString> uncertifiedEngineers;
    for (const Engineer& engineer : engineers) {
        if (!engineerCerts.contains(engineer.id()) || engineerCerts[engineer.id()].isEmpty()) {
            uncertifiedEngineers.append(engineer.name());
        }
    }

    stream << "=" << QString("=").repeated(79) << "\n";
    stream << "COMPLIANCE SUMMARY\n";
    stream << "=" << QString("=").repeated(79) << "\n";
    stream << "Valid Certifications: " << validCount << "\n";
    stream << "Expiring Soon (<=30 days): " << expiringSoonCount << "\n";
    stream << "Expired Certifications: " << expiredCount << "\n";
    stream << "Engineers Without Certifications: " << uncertifiedEngineers.size() << "\n";

    if (!uncertifiedEngineers.isEmpty()) {
        stream << "\nEngineers needing certification:\n";
        for (const QString& name : uncertifiedEngineers) {
            stream << "  - " << name << "\n";
        }
    }

    stream << "=" << QString("=").repeated(79) << "\n";

    return report;
}

QString ReportsWidget::generateDetailedSkillMatrixReport()
{
    QString report;
    QTextStream stream(&report);

    stream << "=" << QString("=").repeated(79) << "\n";
    stream << "  DETAILED SKILL MATRIX REPORT\n";
    stream << "=" << QString("=").repeated(79) << "\n";
    stream << "Generated: " << QDateTime::currentDateTime().toString("dddd, MMMM d, yyyy h:mm AP") << "\n";
    stream << "=" << QString("=").repeated(79) << "\n\n";

    QList<Engineer> engineers = engineerRepo_.findAll();
    QList<ProductionArea> areas = productionRepo_.findAllAreas();
    QList<Assessment> allAssessments = assessmentRepo_.findAll();

    stream << "Total Engineers: " << engineers.size() << "\n";
    stream << "Total Production Areas: " << areas.size() << "\n";
    stream << "Total Assessments: " << allAssessments.size() << "\n\n";

    // Build a matrix
    stream << "SKILL MATRIX (Scores: 0=Not Assessed, 1=Basic, 2=Intermediate, 3=Advanced)\n";
    stream << QString("=").repeated(79) << "\n\n";

    // Header row
    stream << QString("%-25s").arg("Engineer");
    for (const ProductionArea& area : areas) {
        QString areaShort = area.name().left(10);
        stream << QString(" %10s").arg(areaShort);
    }
    stream << " | Avg\n";
    stream << QString("-").repeated(79) << "\n";

    // Data rows
    for (const Engineer& engineer : engineers) {
        stream << QString("%-25s").arg(engineer.name().left(24));

        double totalScore = 0;
        int scoreCount = 0;

        for (const ProductionArea& area : areas) {
            // Find assessment for this engineer and area
            int score = -1;  // -1 means no assessment
            for (const Assessment& a : allAssessments) {
                if (a.engineerId() == engineer.id() && a.productionAreaId() == area.id()) {
                    score = a.score();
                    totalScore += score;
                    scoreCount++;
                    break;  // Use first found (should consolidate in real app)
                }
            }

            if (score >= 0) {
                stream << QString(" %10d").arg(score);
            } else {
                stream << QString(" %10s").arg("-");
            }
        }

        // Average
        double avg = scoreCount > 0 ? totalScore / scoreCount : 0;
        stream << " | " << QString::number(avg, 'f', 2) << "\n";
    }

    stream << "=" << QString("=").repeated(79) << "\n";
    stream << "END OF REPORT\n";
    stream << "=" << QString("=").repeated(79) << "\n";

    return report;
}

void ReportsWidget::onExportPDFClicked()
{
    QString filename = QFileDialog::getSaveFileName(
        this,
        "Export Report to PDF",
        QDir::homePath() + "/skill_matrix_report.pdf",
        "PDF Files (*.pdf)"
    );

    if (filename.isEmpty()) {
        return;
    }

    if (exportToPDF(filename, currentReportContent_)) {
        QMessageBox::information(this, "Success", "Report exported to PDF successfully!");
        Logger::instance().info("ReportsWidget", "Exported report to PDF: " + filename);
    } else {
        QMessageBox::critical(this, "Error", "Failed to export report to PDF.");
        Logger::instance().error("ReportsWidget", "Failed to export report to PDF");
    }
}

void ReportsWidget::onExportCSVClicked()
{
    QString filename = QFileDialog::getSaveFileName(
        this,
        "Export Report to CSV",
        QDir::homePath() + "/skill_matrix_report.csv",
        "CSV Files (*.csv)"
    );

    if (filename.isEmpty()) {
        return;
    }

    if (exportToCSV(filename)) {
        QMessageBox::information(this, "Success", "Report exported to CSV successfully!");
        Logger::instance().info("ReportsWidget", "Exported report to CSV: " + filename);
    } else {
        QMessageBox::critical(this, "Error", "Failed to export report to CSV.");
        Logger::instance().error("ReportsWidget", "Failed to export report to CSV");
    }
}

void ReportsWidget::onPrintClicked()
{
    QPrinter printer(QPrinter::HighResolution);
    printer.setPageOrientation(QPageLayout::Portrait);

    QTextDocument doc;
    doc.setPlainText(currentReportContent_);

    // Print
    doc.print(&printer);

    Logger::instance().info("ReportsWidget", "Report sent to printer");
}

bool ReportsWidget::exportToPDF(const QString& filename, const QString& content)
{
    QPrinter printer(QPrinter::HighResolution);
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setOutputFileName(filename);
    printer.setPageOrientation(QPageLayout::Portrait);
    printer.setPageMargins(QMarginsF(15, 15, 15, 15), QPageLayout::Millimeter);

    QTextDocument doc;
    doc.setPlainText(content);

    // Set monospace font for better alignment
    QFont font("Courier New", 8);
    doc.setDefaultFont(font);

    doc.print(&printer);

    return QFile::exists(filename);
}

bool ReportsWidget::exportToCSV(const QString& filename)
{
    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        return false;
    }

    QTextStream out(&file);

    // Simple CSV export of current report content
    // In a real application, you'd structure this better
    out << "Skill Matrix Report\n";
    out << "Generated," << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss") << "\n\n";

    // Export engineer data
    out << "Engineer ID,Name,Shift,Total Assessments,Average Score\n";

    QList<Engineer> engineers = engineerRepo_.findAll();
    QList<Assessment> allAssessments = assessmentRepo_.findAll();

    for (const Engineer& engineer : engineers) {
        QList<Assessment> engineerAssessments;
        for (const Assessment& a : allAssessments) {
            if (a.engineerId() == engineer.id()) {
                engineerAssessments.append(a);
            }
        }

        double totalScore = 0;
        for (const Assessment& a : engineerAssessments) {
            totalScore += a.score();
        }
        double avgScore = engineerAssessments.isEmpty() ? 0 : totalScore / engineerAssessments.size();

        out << engineer.id() << ","
            << engineer.name() << ","
            << engineer.shift() << ","
            << engineerAssessments.size() << ","
            << QString::number(avgScore, 'f', 2) << "\n";
    }

    file.close();
    return true;
}
