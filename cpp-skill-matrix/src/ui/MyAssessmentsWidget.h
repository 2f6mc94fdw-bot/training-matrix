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

private:
    void setupUI();
    void loadAssessments();
    void createScoreButtons(QHBoxLayout* layout, int areaId, int machineId,
                           int competencyId, int currentScore);

    QString engineerId_;
    QVBoxLayout* assessmentsLayout_;
    QWidget* assessmentsContainer_;
    QPushButton* saveButton_;
    QPushButton* refreshButton_;
    QLabel* summaryLabel_;

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
};

#endif // MYASSESSMENTSWIDGET_H
