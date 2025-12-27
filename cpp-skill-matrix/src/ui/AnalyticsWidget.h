#ifndef ANALYTICSWIDGET_H
#define ANALYTICSWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QListWidget>
#include <QStackedWidget>
#include <QtCharts/QChartView>
#include "../database/AssessmentRepository.h"
#include "../database/EngineerRepository.h"
#include "../database/ProductionRepository.h"

class AnalyticsWidget : public QWidget
{
    Q_OBJECT

public:
    explicit AnalyticsWidget(QWidget* parent = nullptr);
    ~AnalyticsWidget();

protected:
    void showEvent(QShowEvent* event) override;

signals:
    void dataLoadingFinished();

public slots:
    void refresh() { loadAnalytics(); }  // Public refresh method

private slots:
    void onTabChanged(int tabIndex);
    void onRefreshClicked();

private:
    void setupUI();
    void loadAnalytics();

    // Tab setup methods
    void setupTrendsTab(QWidget* trendsWidget);
    void setupShiftComparisonTab(QWidget* shiftsWidget);
    void setupAutomatedInsightsTab(QWidget* insightsWidget);

    // Data update methods
    void updateTrendsData();
    void updateShiftComparisonData();
    void updateAutomatedInsights();

    // Helper methods
    struct PredictionData {
        double current;
        double predicted;
        double change;
        QString trend;  // "up", "down", "stable"
    };
    PredictionData calculatePrediction();

    struct ShiftStats {
        QString shiftName;
        double averageCompletion;
        int engineerCount;
        int totalScore;
        int maxScore;
    };
    QList<ShiftStats> calculateShiftComparison();

    struct Insight {
        QString type;  // "positive", "warning", "alert"
        QString icon;
        QString title;
        QString message;
    };
    QList<Insight> generateAutomatedInsights();

private:
    // Navigation
    QStackedWidget* contentStack_;
    QPushButton* trendsButton_;
    QPushButton* shiftsButton_;
    QPushButton* insightsButton_;

    // Trends Tab Components
    QLabel* currentCompletionLabel_;
    QLabel* predictedCompletionLabel_;
    QLabel* changeLabel_;
    QChartView* trendChartView_;

    // Shift Comparison Tab Components
    QWidget* shiftCardsContainer_;
    QChartView* shiftChartView_;

    // Automated Insights Tab Components
    QListWidget* insightsList_;

    // Repositories
    AssessmentRepository assessmentRepo_;
    EngineerRepository engineerRepo_;
    ProductionRepository productionRepo_;

    // Cached data for performance optimization
    QList<Engineer> cachedEngineers_;
    QList<Assessment> cachedAssessments_;
    QList<ProductionArea> cachedAreas_;
    int cachedTotalCompetencies_;

    // Lazy loading state
    bool isFirstShow_;
};

#endif // ANALYTICSWIDGET_H
