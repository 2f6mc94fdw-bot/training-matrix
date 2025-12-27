#ifndef MYCORESKILLSWIDGET_H
#define MYCORESKILLSWIDGET_H

#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QScrollArea>
#include "../database/CoreSkillsRepository.h"

class MyCoreSkillsWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MyCoreSkillsWidget(const QString& engineerId, QWidget* parent = nullptr);
    ~MyCoreSkillsWidget();

private slots:
    void onRefreshClicked();

private:
    void setupUI();
    void loadCoreSkills();

    QString engineerId_;
    QVBoxLayout* skillsLayout_;
    QWidget* skillsContainer_;
    QPushButton* refreshButton_;
    QLabel* summaryLabel_;

    CoreSkillsRepository coreSkillsRepo_;
};

#endif // MYCORESKILLSWIDGET_H
