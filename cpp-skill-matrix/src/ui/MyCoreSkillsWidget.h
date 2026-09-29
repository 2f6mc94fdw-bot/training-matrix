#ifndef MYCORESKILLSWIDGET_H
#define MYCORESKILLSWIDGET_H

#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QScrollArea>
#include "../database/CoreSkillsRepository.h"
#include "../database/NotificationRepository.h"
#include "../database/EngineerRepository.h"
#include "../database/AssessmentSubmissionRepository.h"

class MyCoreSkillsWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MyCoreSkillsWidget(const QString& engineerId, QWidget* parent = nullptr);
    ~MyCoreSkillsWidget();

private slots:
    void onRefreshClicked();
    void onSaveClicked();
    void onScoreButtonClicked();

private:
    void setupUI();
    void loadCoreSkills();
    void createScoreButtons(QHBoxLayout* layout, const QString& categoryId,
                           const QString& skillId, int currentScore, bool pendingApproval);

    QString engineerId_;
    QVBoxLayout* skillsLayout_;
    QWidget* skillsContainer_;
    QPushButton* saveButton_;
    QPushButton* refreshButton_;
    QLabel* summaryLabel_;

    // Map to track score button groups
    struct ScoreButtonGroup {
        QPushButton* buttons[4];  // 0-3 buttons
        QString categoryId;
        QString skillId;
    };
    QList<ScoreButtonGroup> scoreButtonGroups_;

    CoreSkillsRepository coreSkillsRepo_;
    AssessmentSubmissionRepository submissionRepo_;
};

#endif // MYCORESKILLSWIDGET_H
