#ifndef DASHBOARDWIDGET_H
#define DASHBOARDWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QListWidget>
#include "../database/EngineerRepository.h"
#include "../database/ProductionRepository.h"
#include "../database/AssessmentRepository.h"
#include "../database/CoreSkillsRepository.h"
#include "../models/Engineer.h"
#include "../models/Assessment.h"
#include "../models/ProductionArea.h"
#include <QtCharts/QChart>
#include <QtCharts/QChartView>
#include <QtCharts/QBarSeries>
#include <QtCharts/QPieSeries>

class DashboardWidget : public QWidget
{
    Q_OBJECT

public:
    explicit DashboardWidget(QWidget* parent = nullptr);
    ~DashboardWidget();

private slots:
    void onRefreshClicked();

private:
    void setupUI();
    void loadStatistics();
    void createScoreDistributionCharts(const QList<Engineer>& engineers, const QList<Assessment>& assessments);
    void createPerformanceLists();
    void updateQuickStats(const QList<Engineer>& engineers, const QList<Assessment>& assessments, int totalCompetencies);
    void updateKeyInsights(const QList<Assessment>& assessments);

private:
    // Stat labels - 4 cards matching web app
    QLabel* engineerCountLabel_;
    QLabel* competencyCountLabel_;
    QLabel* avgSkillLevelLabel_;
    QLabel* completionRateLabel_;

    QLabel* lastUpdateLabel_;

    // Charts - Pie + Bar charts side by side
    QChartView* pieChartView_;
    QChart* pieChart_;
    QChartView* barChartView_;
    QChart* barChart_;

    // Performance lists - Top Performers + Needs Attention
    QListWidget* topPerformersList_;
    QListWidget* needsAttentionList_;

    // Key Insights labels
    QLabel* totalAssessmentsLabel_;
    QLabel* fullyTrainedLabel_;
    QLabel* needTrainingLabel_;

    // Buttons
    QPushButton* refreshButton_;

    // Repositories
    EngineerRepository engineerRepo_;
    ProductionRepository productionRepo_;
    AssessmentRepository assessmentRepo_;
    CoreSkillsRepository coreSkillsRepo_;
};

#endif // DASHBOARDWIDGET_H
