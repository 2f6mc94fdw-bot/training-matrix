#ifndef ENGINEERDEVELOPMENTWIDGET_H
#define ENGINEERDEVELOPMENTWIDGET_H

#include <QWidget>
#include <QList>
#include "../models/Assessment.h"
#include "../database/EngineerRepository.h"
#include "../database/AssessmentRepository.h"
#include "../database/DevelopmentPlanRepository.h"

class QChartView;
class QComboBox;
class QDateEdit;
class QLabel;
class QPushButton;
class QTableWidget;
class QTextEdit;

class EngineerDevelopmentWidget : public QWidget
{
    Q_OBJECT

public:
    explicit EngineerDevelopmentWidget(QWidget* parent = nullptr);
    void refresh();

private slots:
    void onEngineerChanged();
    void onAreaChanged();
    void onScopeChanged();
    void addSelectedGapsToPlan();
    void markSelectedPlanComplete();
    void cancelSelectedPlanItem();
    void generateReport();
    void exportReportPdf();

private:
    void setupUI();
    void loadEngineers();
    void loadAreaFilter();
    void loadMachineFilter();
    void loadEngineerDashboard();
    void updateProfileChart();
    void updateGapTable();
    void updatePlanTable();
    QString currentEngineerId() const;
    QString scoreLabel(int score) const;
    QString guidanceForScore(int score) const;
    QString buildReportHtml();

    QComboBox* engineerCombo_;
    QComboBox* areaCombo_;
    QComboBox* machineCombo_;
    QComboBox* targetScoreCombo_;
    QDateEdit* dueDateEdit_;
    QLabel* averageScoreLabel_;
    QLabel* proficiencyLabel_;
    QLabel* gapsLabel_;
    QLabel* activePlansLabel_;
    QChartView* profileChartView_;
    QTableWidget* gapTable_;
    QTableWidget* planTable_;
    QTextEdit* managerNotesEdit_;
    QTextEdit* reportPreview_;
    QPushButton* exportPdfButton_;

    EngineerRepository engineerRepo_;
    AssessmentRepository assessmentRepo_;
    DevelopmentPlanRepository planRepo_;
    QList<Assessment> engineerAssessments_;
    QString reportHtml_;
};

#endif // ENGINEERDEVELOPMENTWIDGET_H
