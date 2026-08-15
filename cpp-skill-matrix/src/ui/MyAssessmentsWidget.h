#ifndef MYASSESSMENTSWIDGET_H
#define MYASSESSMENTSWIDGET_H

#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QScrollArea>
#include "../database/AssessmentRepository.h"
#include "../database/ProductionRepository.h"
#include "../database/NotificationRepository.h"
#include "../database/EngineerRepository.h"
#include "../database/AssessmentSubmissionRepository.h"

class MyAssessmentsWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MyAssessmentsWidget(const QString& engineerId, QWidget* parent = nullptr);
    ~MyAssessmentsWidget();

private slots:
    void onRefreshClicked();
    void onSaveClicked();
    void onScoreButtonClicked();
    void onMarkNotificationsReadClicked();

private:
    void setupUI();
    void loadAssessments();
    void createScoreButtons(QHBoxLayout* layout, int areaId, int machineId,
                           int competencyId, int currentScore, bool pendingApproval);

    QString engineerId_;
    QVBoxLayout* assessmentsLayout_;
    QWidget* assessmentsContainer_;
    QPushButton* saveButton_;
    QPushButton* refreshButton_;
    QPushButton* markNotificationsReadButton_;
    QLabel* summaryLabel_;
    QLabel* notificationsSummaryLabel_;
    QWidget* notificationsContainer_;
    QVBoxLayout* notificationsLayout_;

    // Map to track score button groups
    struct ScoreButtonGroup {
        QPushButton* buttons[4];  // 0-3 buttons
        int areaId;
        int machineId;
        int competencyId;
    };
    QList<ScoreButtonGroup> scoreButtonGroups_;

    AssessmentRepository assessmentRepo_;
    ProductionRepository productionRepo_;
    NotificationRepository notificationRepo_;
    AssessmentSubmissionRepository submissionRepo_;
};

#endif // MYASSESSMENTSWIDGET_H
