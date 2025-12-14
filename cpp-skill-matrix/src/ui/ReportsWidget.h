#ifndef REPORTSWIDGET_H
#define REPORTSWIDGET_H

#include <QWidget>
#include <QPushButton>
#include <QTextEdit>
#include <QComboBox>
#include "../database/EngineerRepository.h"
#include "../database/ProductionRepository.h"
#include "../database/AssessmentRepository.h"
#include "../database/CoreSkillsRepository.h"
#include "../database/CertificationRepository.h"

class ReportsWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ReportsWidget(QWidget* parent = nullptr);
    ~ReportsWidget();

private slots:
    void onGenerateReportClicked();
    void onExportPDFClicked();
    void onExportCSVClicked();
    void onPrintClicked();

private:
    void setupUI();
    QString generateEngineerSkillsReport();
    QString generateProductionAreaCoverageReport();
    QString generateComplianceReport();
    QString generateDetailedSkillMatrixReport();

    bool exportToPDF(const QString& filename, const QString& content);
    bool exportToCSV(const QString& filename);

private:
    QComboBox* reportTypeCombo_;
    QTextEdit* reportDisplay_;
    QPushButton* generateButton_;
    QPushButton* exportPDFButton_;
    QPushButton* exportCSVButton_;
    QPushButton* printButton_;

    // Repositories
    EngineerRepository engineerRepo_;
    ProductionRepository productionRepo_;
    AssessmentRepository assessmentRepo_;
    CoreSkillsRepository coreSkillsRepo_;
    CertificationRepository certificationRepo_;

    QString currentReportContent_;
};

#endif // REPORTSWIDGET_H
