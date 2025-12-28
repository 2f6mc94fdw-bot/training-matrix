#ifndef MYPROGRESSWIDGET_H
#define MYPROGRESSWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QListWidget>
#include <QComboBox>
#include <QtCharts/QChart>
#include <QtCharts/QChartView>
#include "../database/EngineerRepository.h"
#include "../database/AssessmentRepository.h"
#include "../database/ProductionRepository.h"
#include "../database/CoreSkillsRepository.h"
#include "../database/SnapshotRepository.h"
#include "../database/CertificationRepository.h"
#include "../models/Engineer.h"
#include "../models/Assessment.h"
#include "../models/Snapshot.h"
#include "../models/Certification.h"

class MyProgressWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MyProgressWidget(const QString& engineerId, QWidget* parent = nullptr);
    ~MyProgressWidget();

protected:
    void showEvent(QShowEvent* event) override;

private slots:
    void onRefreshClicked();
    void onSnapshotComparisonChanged(int index);

private:
    void setupUI();
    void loadProgressData();
    void createSkillProgressChart();
    void createCoreSkillProgressChart();
    void updateSnapshotComparison();
    void updateCertificationTimeline();
    void updateGoalsProgress();

private:
    QString engineerId_;
    bool isFirstShow_;

    // Progress stats
    QLabel* totalImprovementsLabel_;
    QLabel* avgGrowthRateLabel_;
    QLabel* skillsGainedLabel_;
    QLabel* upcomingCertsLabel_;

    // Charts
    QChartView* skillProgressChartView_;
    QChartView* coreSkillProgressChartView_;

    // Snapshot comparison
    QComboBox* snapshotCombo_;
    QLabel* snapshotComparisonLabel_;
    QListWidget* changesListWidget_;

    // Certifications timeline
    QListWidget* certificationsListWidget_;

    // Goals/Targets
    QListWidget* goalsListWidget_;

    // Buttons
    QPushButton* refreshButton_;

    // Repositories
    EngineerRepository engineerRepo_;
    AssessmentRepository assessmentRepo_;
    ProductionRepository productionRepo_;
    CoreSkillsRepository coreSkillsRepo_;
    SnapshotRepository snapshotRepo_;
    CertificationRepository certificationRepo_;

    // Cached data
    Engineer currentEngineer_;
    QList<Assessment> assessments_;
    QList<CoreSkillAssessment> coreSkillAssessments_;
    QList<Snapshot> snapshots_;
    QList<Certification> certifications_;
};

#endif // MYPROGRESSWIDGET_H
