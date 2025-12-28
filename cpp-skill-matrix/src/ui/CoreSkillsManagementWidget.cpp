#include "CoreSkillsManagementWidget.h"
#include "../utils/Logger.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QInputDialog>
#include <QGroupBox>
#include <QFormLayout>
#include <QLineEdit>
#include <QSpinBox>
#include <QDialog>
#include <QDialogButtonBox>

CoreSkillsManagementWidget::CoreSkillsManagementWidget(QWidget* parent)
    : QWidget(parent)
    , categoryFilterCombo_(nullptr)
    , treeWidget_(nullptr)
    , addButton_(nullptr)
    , editButton_(nullptr)
    , deleteButton_(nullptr)
    , refreshButton_(nullptr)
{
    setupUI();
    loadCoreSkills();
}

CoreSkillsManagementWidget::~CoreSkillsManagementWidget()
{
}

void CoreSkillsManagementWidget::setupUI()
{
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(20, 20, 20, 20);
    mainLayout->setSpacing(15);

    // Title
    QLabel* titleLabel = new QLabel("Core Skills Management", this);
    QFont titleFont = titleLabel->font();
    titleFont.setPointSize(18);
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);
    mainLayout->addWidget(titleLabel);

    // Description
    QLabel* descLabel = new QLabel("Manage core skill categories and individual skills. These are used for assessing engineer competencies across general skills.", this);
    descLabel->setWordWrap(true);
    descLabel->setStyleSheet("color: #666; font-style: italic;");
    mainLayout->addWidget(descLabel);

    // Filter section
    QHBoxLayout* filterLayout = new QHBoxLayout();
    QLabel* filterLabel = new QLabel("Filter by category:", this);
    categoryFilterCombo_ = new QComboBox(this);
    connect(categoryFilterCombo_, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &CoreSkillsManagementWidget::onCategoryFilterChanged);
    filterLayout->addWidget(filterLabel);
    filterLayout->addWidget(categoryFilterCombo_, 1);
    filterLayout->addStretch();
    mainLayout->addLayout(filterLayout);

    // Tree widget
    treeWidget_ = new QTreeWidget(this);
    treeWidget_->setHeaderLabels({"Name", "ID", "Max Score"});
    treeWidget_->setColumnWidth(0, 300);
    treeWidget_->setColumnWidth(1, 200);
    treeWidget_->setColumnWidth(2, 100);
    treeWidget_->setAlternatingRowColors(true);
    connect(treeWidget_, &QTreeWidget::itemDoubleClicked,
            this, &CoreSkillsManagementWidget::onTreeItemDoubleClicked);
    mainLayout->addWidget(treeWidget_);

    // Buttons
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    addButton_ = new QPushButton("Add", this);
    editButton_ = new QPushButton("Edit", this);
    deleteButton_ = new QPushButton("Delete", this);
    refreshButton_ = new QPushButton("Refresh", this);

    connect(addButton_, &QPushButton::clicked, this, &CoreSkillsManagementWidget::onAddClicked);
    connect(editButton_, &QPushButton::clicked, this, &CoreSkillsManagementWidget::onEditClicked);
    connect(deleteButton_, &QPushButton::clicked, this, &CoreSkillsManagementWidget::onDeleteClicked);
    connect(refreshButton_, &QPushButton::clicked, this, &CoreSkillsManagementWidget::onRefreshClicked);

    buttonLayout->addWidget(addButton_);
    buttonLayout->addWidget(editButton_);
    buttonLayout->addWidget(deleteButton_);
    buttonLayout->addStretch();
    buttonLayout->addWidget(refreshButton_);

    mainLayout->addLayout(buttonLayout);
}

void CoreSkillsManagementWidget::loadCoreSkills()
{
    treeWidget_->clear();

    // Load all categories
    allCategories_ = repository_.findAllCategories();

    // Get filter selection
    QString selectedCategoryId = categoryFilterCombo_->currentData().toString();

    // Load all skills
    QList<CoreSkill> allSkills = repository_.findAllSkills();

    // Build tree
    for (const CoreSkillCategory& category : allCategories_) {
        // Apply filter
        if (!selectedCategoryId.isEmpty() && category.id() != selectedCategoryId) {
            continue;
        }

        QTreeWidgetItem* categoryItem = new QTreeWidgetItem(treeWidget_);
        categoryItem->setText(0, category.name());
        categoryItem->setText(1, category.id());
        categoryItem->setText(2, "");
        categoryItem->setData(0, Qt::UserRole, CategoryItem);
        categoryItem->setData(1, Qt::UserRole, category.id());

        QFont boldFont = categoryItem->font(0);
        boldFont.setBold(true);
        categoryItem->setFont(0, boldFont);
        categoryItem->setBackground(0, QColor(240, 240, 240));
        categoryItem->setBackground(1, QColor(240, 240, 240));
        categoryItem->setBackground(2, QColor(240, 240, 240));

        // Add skills for this category
        for (const CoreSkill& skill : allSkills) {
            if (skill.categoryId() == category.id()) {
                QTreeWidgetItem* skillItem = new QTreeWidgetItem(categoryItem);
                skillItem->setText(0, skill.name());
                skillItem->setText(1, skill.id());
                skillItem->setText(2, QString::number(skill.maxScore()));
                skillItem->setData(0, Qt::UserRole, SkillItem);
                skillItem->setData(1, Qt::UserRole, skill.id());
                skillItem->setData(2, Qt::UserRole, category.id());
            }
        }

        categoryItem->setExpanded(true);
    }

    // Update category filter
    loadCategoryFilter();

    Logger::instance().info("CoreSkillsManagementWidget",
        QString("Loaded %1 categories").arg(allCategories_.size()));
}

void CoreSkillsManagementWidget::loadCategoryFilter()
{
    QString currentSelection = categoryFilterCombo_->currentData().toString();

    categoryFilterCombo_->blockSignals(true);
    categoryFilterCombo_->clear();
    categoryFilterCombo_->addItem("All Categories", "");

    for (const CoreSkillCategory& category : allCategories_) {
        categoryFilterCombo_->addItem(category.name(), category.id());
    }

    // Restore selection
    int index = categoryFilterCombo_->findData(currentSelection);
    if (index >= 0) {
        categoryFilterCombo_->setCurrentIndex(index);
    }

    categoryFilterCombo_->blockSignals(false);
}

void CoreSkillsManagementWidget::loadSkillsForCategory(const QString& categoryId)
{
    // This is handled in loadCoreSkills() with the tree structure
    loadCoreSkills();
}

void CoreSkillsManagementWidget::showCategoryDialog(const CoreSkillCategory* category)
{
    QDialog dialog(this);
    dialog.setWindowTitle(category ? "Edit Category" : "Add Category");
    dialog.setMinimumWidth(400);

    QVBoxLayout* layout = new QVBoxLayout(&dialog);
    QFormLayout* formLayout = new QFormLayout();

    // Category ID
    QLineEdit* idEdit = new QLineEdit(&dialog);
    if (category) {
        idEdit->setText(category->id());
        idEdit->setEnabled(false); // Don't allow editing ID
    } else {
        idEdit->setPlaceholderText("e.g., mechanical, electrical");
    }
    formLayout->addRow("ID:", idEdit);

    // Category Name
    QLineEdit* nameEdit = new QLineEdit(&dialog);
    if (category) {
        nameEdit->setText(category->name());
    }
    nameEdit->setPlaceholderText("e.g., Mechanical Skills");
    formLayout->addRow("Name:", nameEdit);

    layout->addLayout(formLayout);

    // Buttons
    QDialogButtonBox* buttonBox = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dialog);
    connect(buttonBox, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);
    layout->addWidget(buttonBox);

    if (dialog.exec() == QDialog::Accepted) {
        QString id = idEdit->text().trimmed();
        QString name = nameEdit->text().trimmed();

        if (id.isEmpty() || name.isEmpty()) {
            QMessageBox::warning(this, "Invalid Input", "Category ID and Name are required.");
            return;
        }

        CoreSkillCategory newCategory;
        newCategory.setId(id);
        newCategory.setName(name);

        if (repository_.saveCategory(newCategory)) {
            QMessageBox::information(this, "Success",
                category ? "Category updated successfully." : "Category created successfully.");
            loadCoreSkills();
        } else {
            QMessageBox::critical(this, "Error",
                QString("Failed to save category: %1").arg(repository_.lastError()));
        }
    }
}

void CoreSkillsManagementWidget::showSkillDialog(const QString& parentCategoryId, const CoreSkill* skill)
{
    QDialog dialog(this);
    dialog.setWindowTitle(skill ? "Edit Skill" : "Add Skill");
    dialog.setMinimumWidth(400);

    QVBoxLayout* layout = new QVBoxLayout(&dialog);
    QFormLayout* formLayout = new QFormLayout();

    // Category selection (if adding new skill)
    QComboBox* categoryCombo = new QComboBox(&dialog);
    for (const CoreSkillCategory& category : allCategories_) {
        categoryCombo->addItem(category.name(), category.id());
    }
    if (!parentCategoryId.isEmpty()) {
        int index = categoryCombo->findData(parentCategoryId);
        if (index >= 0) {
            categoryCombo->setCurrentIndex(index);
        }
    }
    if (skill) {
        int index = categoryCombo->findData(skill->categoryId());
        if (index >= 0) {
            categoryCombo->setCurrentIndex(index);
        }
    }
    formLayout->addRow("Category:", categoryCombo);

    // Skill ID
    QLineEdit* idEdit = new QLineEdit(&dialog);
    if (skill) {
        idEdit->setText(skill->id());
        idEdit->setEnabled(false); // Don't allow editing ID
    } else {
        idEdit->setPlaceholderText("e.g., troubleshooting, plc-programming");
    }
    formLayout->addRow("ID:", idEdit);

    // Skill Name
    QLineEdit* nameEdit = new QLineEdit(&dialog);
    if (skill) {
        nameEdit->setText(skill->name());
    }
    nameEdit->setPlaceholderText("e.g., Troubleshooting");
    formLayout->addRow("Name:", nameEdit);

    // Max Score
    QSpinBox* maxScoreSpinBox = new QSpinBox(&dialog);
    maxScoreSpinBox->setRange(1, 10);
    maxScoreSpinBox->setValue(skill ? skill->maxScore() : 3);
    formLayout->addRow("Max Score:", maxScoreSpinBox);

    layout->addLayout(formLayout);

    // Buttons
    QDialogButtonBox* buttonBox = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dialog);
    connect(buttonBox, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);
    layout->addWidget(buttonBox);

    if (dialog.exec() == QDialog::Accepted) {
        QString categoryId = categoryCombo->currentData().toString();
        QString id = idEdit->text().trimmed();
        QString name = nameEdit->text().trimmed();
        int maxScore = maxScoreSpinBox->value();

        if (id.isEmpty() || name.isEmpty()) {
            QMessageBox::warning(this, "Invalid Input", "Skill ID and Name are required.");
            return;
        }

        CoreSkill newSkill;
        newSkill.setId(id);
        newSkill.setCategoryId(categoryId);
        newSkill.setName(name);
        newSkill.setMaxScore(maxScore);

        if (repository_.saveSkill(newSkill)) {
            QMessageBox::information(this, "Success",
                skill ? "Skill updated successfully." : "Skill created successfully.");
            loadCoreSkills();
        } else {
            QMessageBox::critical(this, "Error",
                QString("Failed to save skill: %1").arg(repository_.lastError()));
        }
    }
}

void CoreSkillsManagementWidget::onAddClicked()
{
    QTreeWidgetItem* selectedItem = treeWidget_->currentItem();

    if (!selectedItem) {
        // No selection - show category dialog
        showCategoryDialog();
        return;
    }

    int itemType = selectedItem->data(0, Qt::UserRole).toInt();

    if (itemType == CategoryItem) {
        // Selected a category - add skill to it
        QString categoryId = selectedItem->data(1, Qt::UserRole).toString();
        showSkillDialog(categoryId);
    } else {
        // Selected a skill - add sibling skill to same category
        QString categoryId = selectedItem->data(2, Qt::UserRole).toString();
        showSkillDialog(categoryId);
    }
}

void CoreSkillsManagementWidget::onEditClicked()
{
    QTreeWidgetItem* selectedItem = treeWidget_->currentItem();
    if (!selectedItem) {
        QMessageBox::information(this, "No Selection", "Please select an item to edit.");
        return;
    }

    int itemType = selectedItem->data(0, Qt::UserRole).toInt();

    if (itemType == CategoryItem) {
        QString categoryId = selectedItem->data(1, Qt::UserRole).toString();

        // Find the category
        for (const CoreSkillCategory& category : allCategories_) {
            if (category.id() == categoryId) {
                showCategoryDialog(&category);
                return;
            }
        }
    } else {
        QString skillId = selectedItem->data(1, Qt::UserRole).toString();
        QString categoryId = selectedItem->data(2, Qt::UserRole).toString();

        // Find the skill
        QList<CoreSkill> allSkills = repository_.findAllSkills();
        for (const CoreSkill& skill : allSkills) {
            if (skill.id() == skillId) {
                showSkillDialog(categoryId, &skill);
                return;
            }
        }
    }
}

void CoreSkillsManagementWidget::onDeleteClicked()
{
    QTreeWidgetItem* selectedItem = treeWidget_->currentItem();
    if (!selectedItem) {
        QMessageBox::information(this, "No Selection", "Please select an item to delete.");
        return;
    }

    int itemType = selectedItem->data(0, Qt::UserRole).toInt();
    QString itemName = selectedItem->text(0);

    QMessageBox::StandardButton reply = QMessageBox::question(this, "Confirm Delete",
        QString("Are you sure you want to delete '%1'?\n\nThis action cannot be undone.").arg(itemName),
        QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        if (itemType == CategoryItem) {
            QString categoryId = selectedItem->data(1, Qt::UserRole).toString();

            if (repository_.deleteCategory(categoryId)) {
                QMessageBox::information(this, "Success", "Category deleted successfully.");
                loadCoreSkills();
            } else {
                QMessageBox::critical(this, "Error",
                    QString("Failed to delete category: %1").arg(repository_.lastError()));
            }
        } else {
            QString skillId = selectedItem->data(1, Qt::UserRole).toString();

            if (repository_.deleteSkill(skillId)) {
                QMessageBox::information(this, "Success", "Skill deleted successfully.");
                loadCoreSkills();
            } else {
                QMessageBox::critical(this, "Error",
                    QString("Failed to delete skill: %1").arg(repository_.lastError()));
            }
        }
    }
}

void CoreSkillsManagementWidget::onRefreshClicked()
{
    loadCoreSkills();
}

void CoreSkillsManagementWidget::onTreeItemDoubleClicked(QTreeWidgetItem* item, int column)
{
    Q_UNUSED(column);
    if (item) {
        onEditClicked();
    }
}

void CoreSkillsManagementWidget::onCategoryFilterChanged(int index)
{
    Q_UNUSED(index);
    loadCoreSkills();
}
