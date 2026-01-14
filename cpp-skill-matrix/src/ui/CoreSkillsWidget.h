#ifndef CORESKILLSWIDGET_H
#define CORESKILLSWIDGET_H

#include <QWidget>
#include <QPushButton>
#include <QComboBox>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QScrollArea>
#include "../database/CoreSkillsRepository.h"
#include "../database/EngineerRepository.h"

class CoreSkillsWidget : public QWidget
{
    Q_OBJECT

public:
    explicit CoreSkillsWidget(QWidget* parent = nullptr);
    ~CoreSkillsWidget();

protected:
    void showEvent(QShowEvent* event) override;

private slots:
    void onEngineerChanged(int index);
    void onSaveClicked();
    void onRefreshClicked();
    void onScoreButtonClicked();

private:
    void setupUI();
    void loadCoreSkills();
    void loadEngineers();
    void loadAssessments();
    void createScoreButtons(QHBoxLayout* layout, const QString& engineerId,
                           const QString& categoryId, const QString& skillId,
                           int currentScore);

private:
    QComboBox* engineerCombo_;
    QVBoxLayout* skillsLayout_;
    QWidget* skillsContainer_;
    QPushButton* saveButton_;
    QPushButton* refreshButton_;

    // Map to track score button groups
    struct ScoreButtonGroup {
        QPushButton* buttons[4];  // 0-3 buttons
        QString engineerId;
        QString categoryId;
        QString skillId;
    };
    QList<ScoreButtonGroup> scoreButtonGroups_;

    CoreSkillsRepository coreSkillsRepo_;
    EngineerRepository engineerRepo_;

    bool isFirstShow_ = true;
};

#endif // CORESKILLSWIDGET_H
