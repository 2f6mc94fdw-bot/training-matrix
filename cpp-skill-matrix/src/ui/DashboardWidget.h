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

class QVBoxLayout;

class DashboardWidget : public QWidget
{
    Q_OBJECT

public:
    explicit DashboardWidget(QWidget* parent = nullptr);
    ~DashboardWidget();

public slots:
    void refresh() { loadStatistics(); }  // Public refresh method

private slots:
    void onRefreshClicked();

private:
    void setupUI();
    void loadStatistics();

    // Section setup methods
    void setupCriticalAlertsSection(QVBoxLayout* mainLayout);
    void setupProductionHealthSection(QVBoxLayout* mainLayout);
    void setupMachineReadinessSection(QVBoxLayout* mainLayout);
    void setupShiftComparisonSection(QVBoxLayout* mainLayout);
    void setupTrainingPipelineSection(QVBoxLayout* mainLayout);
    void setupRecentActivitySection(QVBoxLayout* mainLayout);
    void setupQuickActionsSection(QVBoxLayout* mainLayout);

    // Data update methods
    void updateCriticalAlerts();
    void updateProductionHealth();
    void updateMachineReadiness();
    void updateShiftComparison();
    void updateTrainingPipeline();
    void updateRecentActivity();

    // Data structures for analytics
    struct MachineReadiness {
        QString machineName;
        int machineId;
        int proficientCount;
        int expertCount;
        int totalEngineers;
        double coveragePercent;
        bool isCritical;
        int importance;
    };

    struct TrainingRecommendation {
        QString competencyName;
        int engineersNeed;
        double impactScore;
        QString priority;
    };

    struct ShiftPerformance {
        QString shiftName;
        double averageCompletion;
        int engineerCount;
    };

    // Calculation methods
    QList<MachineReadiness> calculateMachineReadiness();
    QList<TrainingRecommendation> calculateTrainingPriorities();
    QList<ShiftPerformance> calculateShiftPerformance();

private:
    // Section 1: Critical Alerts
    QListWidget* criticalAlertsList_;

    // Section 2: Production Health (4 cards)
    QLabel* machinesAtRiskLabel_;
    QLabel* criticalCompetenciesLabel_;
    QLabel* trainingInProgressLabel_;
    QLabel* promotionReadyLabel_;

    // Section 3: Machine Readiness
    QWidget* machineReadinessContainer_;

    // Section 4: Shift Comparison
    QWidget* shiftComparisonContainer_;

    // Section 5: Training Pipeline
    QListWidget* urgentTrainingList_;
    QListWidget* promotionReadyList_;

    // Section 6: Recent Activity
    QListWidget* recentActivityList_;

    // Section 7: Quick Actions (buttons)
    QPushButton* viewAnalyticsButton_;
    QPushButton* scheduleTrainingButton_;
    QPushButton* assessEngineersButton_;
    QPushButton* exportReportButton_;

    QLabel* lastUpdateLabel_;
    QPushButton* refreshButton_;

    // Repositories
    EngineerRepository engineerRepo_;
    ProductionRepository productionRepo_;
    AssessmentRepository assessmentRepo_;
    CoreSkillsRepository coreSkillsRepo_;
};

#endif // DASHBOARDWIDGET_H
