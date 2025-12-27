#ifndef CORESKILLSWIDGET_H
#define CORESKILLSWIDGET_H

#include <QWidget>
#include <QTreeWidget>
#include <QPushButton>
#include <QTableWidget>
#include <QComboBox>
#include <QHBoxLayout>
#include "../database/CoreSkillsRepository.h"
#include "../database/EngineerRepository.h"

class CoreSkillsWidget : public QWidget
{
    Q_OBJECT

public:
    explicit CoreSkillsWidget(QWidget* parent = nullptr);
    ~CoreSkillsWidget();

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
    void createScoreButtons(QHBoxLayout* layout, QWidget* parentWidget,
                           const QString& engineerId,
                           const QString& categoryId, const QString& skillId,
                           int currentScore);

private:
    QComboBox* engineerCombo_;
    QTableWidget* skillsTable_;
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
};

#endif // CORESKILLSWIDGET_H
