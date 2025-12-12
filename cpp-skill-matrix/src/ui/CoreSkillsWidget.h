#ifndef CORESKILLSWIDGET_H
#define CORESKILLSWIDGET_H

#include <QWidget>
#include <QTreeWidget>
#include <QPushButton>
#include <QTableWidget>
#include <QComboBox>
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

private:
    void setupUI();
    void loadCoreSkills();
    void loadEngineers();
    void loadAssessments();

private:
    QComboBox* engineerCombo_;
    QTableWidget* skillsTable_;
    QPushButton* saveButton_;
    QPushButton* refreshButton_;

    CoreSkillsRepository coreSkillsRepo_;
    EngineerRepository engineerRepo_;
};

#endif // CORESKILLSWIDGET_H
