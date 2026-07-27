#ifndef DASHBOARDWIDGET_H
#define DASHBOARDWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QListWidget>
#include <QCheckBox>
#include <QShowEvent>
#include <QFutureWatcher>
#include <QHash>
#include <QElapsedTimer>
#include <QStringList>
#include <QVector>
#include <QComboBox>
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
    void showEvent(QShowEvent* event) override;

public slots:
    void refresh();

private slots:
    void onRefreshClicked();
    void onWeightedScoreToggled(bool checked);
    void onMetricsComputationFinished();
    void onAreaFocusChanged(int index);
    void clearDashboardFocus();

private:
    void setupUI();
    void loadStatisticsDeferred();
    void loadStatistics();

    // Section setup methods
    void setupCriticalAlertsSection(QVBoxLayout* mainLayout);
    void setupProductionHealthSection(QVBoxLayout* mainLayout);
    void setupInteractiveFocusSection(QVBoxLayout* mainLayout);
    void setupMachineReadinessSection(QVBoxLayout* mainLayout);
    void setupShiftComparisonSection(QVBoxLayout* mainLayout);
    void setupTrainingPipelineSection(QVBoxLayout* mainLayout);
    void setupRecentActivitySection(QVBoxLayout* mainLayout);
    void setupQuickActionsSection(QVBoxLayout* mainLayout);
    void setupAreaCoverageRadarSection(QVBoxLayout* mainLayout);
    void setupTeamKnowledgeRadarSection(QVBoxLayout* mainLayout);
    void setupEngineerScoresSection(QVBoxLayout* mainLayout);
    void setupAreaProgressSection(QVBoxLayout* mainLayout);

    // Data update methods
    void updateCriticalAlerts();
    void updateProductionHealth();
    void updateMachineReadiness();
    void updateShiftComparison();
    void updateTrainingPipeline();
    void updateRecentActivity();
    void updateAreaCoverageRadar();
    void updateTeamKnowledgeRadar();
    void updateEngineerScoresChart();
    void updateAreaProgressChart();
    void updateInteractiveFocusPanel();
    void selectAreaFocus(int areaId);

public:
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

    struct EngineerScoreRowData {
        QString name;
        double rawPercent = 0.0;
        double weightedCapabilityPercent = 0.0;
        double confidencePercent = 0.0;
    };

private:
    // Calculation methods
    QList<MachineReadiness> calculateMachineReadiness();
    QList<TrainingRecommendation> calculateTrainingPriorities();
    QList<ShiftPerformance> calculateShiftPerformance();
    QMap<QString, double> calculateProductionAreaCoverage();

private:
    // Section 1: Critical Alerts
    QListWidget* criticalAlertsList_;

    // Section 2: Production Health (4 cards)
    QLabel* machinesAtRiskLabel_;
    QLabel* criticalCompetenciesLabel_;
    QLabel* trainingInProgressLabel_;
    QLabel* promotionReadyLabel_;

    // Interactive dashboard focus
    QComboBox* areaFocusCombo_;
    QLabel* focusTitleLabel_;
    QLabel* focusDetailLabel_;
    QPushButton* clearFocusButton_;
    int selectedAreaId_;

    // Section 3: Machine Readiness
    QWidget* machineReadinessContainer_;

    // Section 4: Shift Comparison
    QWidget* shiftComparisonContainer_;

    // Section 5: Training Pipeline
    QListWidget* urgentTrainingList_;
    QListWidget* promotionReadyList_;

    // Section 6: Recent Activity
    QListWidget* recentActivityList_;

    // Section 7: Production area radar
    QChartView* areaCoverageRadarView_;

    // Section 8: Engineer scores
    QChartView* engineerScoresChartView_;
    QCheckBox* weightedScoreToggle_;
    QLabel* topEngineerLabel_;
    QLabel* worstEngineerLabel_;

    // Section 9: 12-month area progress
    QChartView* areaProgressChartView_;

    // Section 10: Team knowledge radar
    QChartView* teamKnowledgeRadarView_;

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

    // Per-refresh cached data and indexes
    QList<Engineer> cachedEngineers_;
    QList<Assessment> cachedAssessments_;
    QList<ProductionArea> cachedAreas_;
    QHash<int, QList<Machine>> machinesByArea_;
    QHash<int, QList<Competency>> competenciesByMachine_;
    QHash<QString, QHash<int, int>> assessmentScoreByEngineerAndCompetency_;
    QHash<QString, QList<Assessment>> assessmentsByEngineer_;
    QList<MachineReadiness> machineReadinessCache_;
    QList<TrainingRecommendation> trainingPrioritiesCache_;
    QList<ShiftPerformance> shiftPerformanceCache_;
    QMap<QString, double> areaCoverageCache_;
    QList<EngineerScoreRowData> engineerScoreRowsCache_;
    QStringList areaProgressMonthLabelsCache_;
    QHash<int, QVector<double>> areaProgressByAreaIdCache_;
    struct MetricsComputationResult {
        QList<MachineReadiness> machineReadiness;
        QList<TrainingRecommendation> trainingPriorities;
        QList<ShiftPerformance> shiftPerformance;
        QMap<QString, double> areaCoverage;
        QList<EngineerScoreRowData> engineerScoreRows;
        QStringList areaProgressMonthLabels;
        QHash<int, QVector<double>> areaProgressByAreaId;
    };
    QFutureWatcher<MetricsComputationResult>* metricsWatcher_;
    QElapsedTimer refreshTimer_;
    QElapsedTimer computeTimer_;
    qint64 lastDataFetchMs_;
    qint64 lastComputeMs_;

    bool isFirstShow_;
    bool isLoading_;
    bool waitingForCacheWarmup_;
    bool showWeightedScores_;
};

#endif // DASHBOARDWIDGET_H
