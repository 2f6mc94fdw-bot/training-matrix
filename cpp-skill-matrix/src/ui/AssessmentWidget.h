#ifndef ASSESSMENTWIDGET_H
#define ASSESSMENTWIDGET_H

#include <QWidget>
#include <QComboBox>
#include <QPushButton>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QLabel>
#include <QMap>
#include <QTimer>
#include <QFutureWatcher>
#include "../database/EngineerRepository.h"
#include "../database/ProductionRepository.h"
#include "../database/AssessmentRepository.h"
#include "../models/Engineer.h"
#include "../models/Competency.h"

class AssessmentWidget : public QWidget
{
    Q_OBJECT

public:
    explicit AssessmentWidget(QWidget* parent = nullptr);
    ~AssessmentWidget();

protected:
    void showEvent(QShowEvent* event) override;

signals:
    void dataLoadingFinished();

private slots:
    void onAreaFilterChanged(int index);
    void onScoreButtonClicked();
    void onRefreshClicked();
    void onDataLoaded();
    void loadNextEngineerCard();

private:
    void setupUI();
    void loadEngineerCards();
    void createScoreButtons(QHBoxLayout* layout, const QString& engineerId, int areaId, int machineId, int competencyId, int currentScore);
    void updateEngineerSummary(const QString& engineerId, QLabel* summaryLabel);

private:
    QComboBox* areaFilterCombo_;
    QVBoxLayout* engineersLayout_;  // Container for engineer cards
    QWidget* engineersContainer_;

    // Map to track score button groups
    struct ScoreButtonGroup {
        QPushButton* buttons[4];  // 0-3 buttons
        QString engineerId;
        QString competencyId;
    };
    QList<ScoreButtonGroup> scoreButtonGroups_;

    EngineerRepository engineerRepo_;
    ProductionRepository productionRepo_;
    AssessmentRepository assessmentRepo_;

    // Progressive loading state
    bool isFirstShow_;
    QTimer* loadTimer_;
    QLabel* loadingLabel_;

    // Cached data for progressive loading
    QList<Engineer> cachedEngineers_;
    QMap<QString, int> cachedAssessmentScores_;
    QMap<int, QString> cachedAreaNames_;  // areaId -> areaName lookup
    struct MachineData {
        Machine machine;
        QList<Competency> competencies;
    };
    QMap<int, QList<MachineData>> cachedAreaToMachines_;
    int currentEngineerIndex_;

    // Background data loading
    struct LoadedData {
        QList<Engineer> engineers;
        QList<Assessment> assessments;
        QList<ProductionArea> areas;
        QMap<int, QList<MachineData>> areaToMachines;
    };
    QFutureWatcher<LoadedData>* dataWatcher_;
};

#endif // ASSESSMENTWIDGET_H
