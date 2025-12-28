#ifndef CORESKILLSMANAGEMENTWIDGET_H
#define CORESKILLSMANAGEMENTWIDGET_H

#include <QWidget>
#include <QTreeWidget>
#include <QPushButton>
#include <QComboBox>
#include "../database/CoreSkillsRepository.h"

enum CoreSkillItemType {
    CategoryItem = 0,
    SkillItem = 1
};

class CoreSkillsManagementWidget : public QWidget
{
    Q_OBJECT

public:
    explicit CoreSkillsManagementWidget(QWidget* parent = nullptr);
    ~CoreSkillsManagementWidget();

private slots:
    void onAddClicked();
    void onEditClicked();
    void onDeleteClicked();
    void onRefreshClicked();
    void onTreeItemDoubleClicked(QTreeWidgetItem* item, int column);
    void onCategoryFilterChanged(int index);

private:
    void setupUI();
    void loadCoreSkills();
    void loadCategoryFilter();
    void loadSkillsForCategory(const QString& categoryId);
    void showCategoryDialog(const CoreSkillCategory* category = nullptr);
    void showSkillDialog(const QString& parentCategoryId, const CoreSkill* skill = nullptr);

private:
    QComboBox* categoryFilterCombo_;
    QTreeWidget* treeWidget_;
    QPushButton* addButton_;
    QPushButton* editButton_;
    QPushButton* deleteButton_;
    QPushButton* refreshButton_;

    CoreSkillsRepository repository_;
    QList<CoreSkillCategory> allCategories_;
};

#endif // CORESKILLSMANAGEMENTWIDGET_H
