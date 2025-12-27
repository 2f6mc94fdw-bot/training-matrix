#include "CoreSkillsWidget.h"
#include "../utils/Logger.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QHeaderView>
#include <QMessageBox>
#include <QSpinBox>
#include <QLabel>
#include <QGroupBox>

CoreSkillsWidget::CoreSkillsWidget(QWidget* parent)
    : QWidget(parent)
    , engineerCombo_(nullptr)
    , skillsTable_(nullptr)
    , saveButton_(nullptr)
    , refreshButton_(nullptr)
{
    setupUI();
    loadEngineers();
    loadCoreSkills();
    Logger::instance().info("CoreSkillsWidget", "Core Skills widget initialized");
}

CoreSkillsWidget::~CoreSkillsWidget()
{
}

void CoreSkillsWidget::setupUI()
{
    QVBoxLayout* mainLayout = new QVBoxLayout(this);

    // Title
    QLabel* titleLabel = new QLabel("Core Skills Assessment", this);
    QFont titleFont = titleLabel->font();
    titleFont.setPointSize(16);
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);
    mainLayout->addWidget(titleLabel);

    // Engineer Selection
    QGroupBox* selectionGroup = new QGroupBox("Select Engineer", this);
    QFormLayout* formLayout = new QFormLayout(selectionGroup);

    engineerCombo_ = new QComboBox(this);
    connect(engineerCombo_, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &CoreSkillsWidget::onEngineerChanged);

    formLayout->addRow("Engineer:", engineerCombo_);
    mainLayout->addWidget(selectionGroup);

    // Skills Table
    QLabel* tableLabel = new QLabel("Core Skills (0 = No skill, 1 = Basic, 2 = Intermediate, 3 = Advanced)", this);
    mainLayout->addWidget(tableLabel);

    skillsTable_ = new QTableWidget(this);
    skillsTable_->setColumnCount(4);
    skillsTable_->setHorizontalHeaderLabels({"Category", "Skill", "Score", "IDs"});
    skillsTable_->horizontalHeader()->setStretchLastSection(true);
    skillsTable_->setAlternatingRowColors(true);
    skillsTable_->setSortingEnabled(true);  // Enable column sorting
    skillsTable_->sortByColumn(0, Qt::AscendingOrder);  // Default sort by Category
    skillsTable_->hideColumn(3); // Hide IDs column

    mainLayout->addWidget(skillsTable_);

    // Buttons
    QHBoxLayout* buttonLayout = new QHBoxLayout();

    saveButton_ = new QPushButton("Save Assessments", this);
    refreshButton_ = new QPushButton("Refresh", this);

    connect(saveButton_, &QPushButton::clicked, this, &CoreSkillsWidget::onSaveClicked);
    connect(refreshButton_, &QPushButton::clicked, this, &CoreSkillsWidget::onRefreshClicked);

    buttonLayout->addWidget(saveButton_);
    buttonLayout->addStretch();
    buttonLayout->addWidget(refreshButton_);

    mainLayout->addLayout(buttonLayout);

    setLayout(mainLayout);
}

void CoreSkillsWidget::loadEngineers()
{
    engineerCombo_->clear();
    engineerCombo_->addItem("-- Select Engineer --", "");

    QList<Engineer> engineers = engineerRepo_.findAll();

    for (const Engineer& engineer : engineers) {
        engineerCombo_->addItem(engineer.name(), engineer.id());
    }
}

void CoreSkillsWidget::loadCoreSkills()
{
    // Disable sorting while loading to improve performance and prevent issues
    skillsTable_->setSortingEnabled(false);
    skillsTable_->setRowCount(0);

    QList<CoreSkillCategory> categories = coreSkillsRepo_.findAllCategories();
    QList<CoreSkill> skills = coreSkillsRepo_.findAllSkills();

    int row = 0;
    for (const CoreSkillCategory& category : categories) {
        // Find skills for this category
        for (const CoreSkill& skill : skills) {
            if (skill.categoryId() == category.id()) {
                skillsTable_->insertRow(row);

                skillsTable_->setItem(row, 0, new QTableWidgetItem(category.name()));
                skillsTable_->setItem(row, 1, new QTableWidgetItem(skill.name()));

                // Score spinner
                QSpinBox* scoreSpinner = new QSpinBox(this);
                scoreSpinner->setRange(0, 3);
                scoreSpinner->setValue(0);
                skillsTable_->setCellWidget(row, 2, scoreSpinner);

                // Store IDs (hidden)
                QString ids = QString("%1|%2").arg(category.id()).arg(skill.id());
                skillsTable_->setItem(row, 3, new QTableWidgetItem(ids));

                row++;
            }
        }
    }

    // Re-enable sorting and apply default sort
    skillsTable_->setSortingEnabled(true);
    skillsTable_->sortByColumn(0, Qt::AscendingOrder);

    Logger::instance().info("CoreSkillsWidget", QString("Loaded %1 core skills").arg(row));
}

void CoreSkillsWidget::loadAssessments()
{
    QString engineerId = engineerCombo_->currentData().toString();

    if (engineerId.isEmpty()) {
        return;
    }

    QList<CoreSkillAssessment> assessments = coreSkillsRepo_.findAllAssessments();

    // Update table with existing scores
    for (int row = 0; row < skillsTable_->rowCount(); ++row) {
        QString ids = skillsTable_->item(row, 3)->text();
        QStringList parts = ids.split('|');

        if (parts.size() == 2) {
            QString categoryId = parts[0];
            QString skillId = parts[1];

            // Find matching assessment
            for (const CoreSkillAssessment& assessment : assessments) {
                if (assessment.engineerId() == engineerId &&
                    assessment.categoryId() == categoryId &&
                    assessment.skillId() == skillId) {

                    QSpinBox* spinner = qobject_cast<QSpinBox*>(skillsTable_->cellWidget(row, 2));
                    if (spinner) {
                        spinner->setValue(assessment.score());
                    }
                    break;
                }
            }
        }
    }

    Logger::instance().info("CoreSkillsWidget", "Loaded assessments for engineer: " + engineerId);
}

void CoreSkillsWidget::onEngineerChanged(int index)
{
    if (index > 0) {
        loadAssessments();
    } else {
        // Reset scores
        for (int row = 0; row < skillsTable_->rowCount(); ++row) {
            QSpinBox* spinner = qobject_cast<QSpinBox*>(skillsTable_->cellWidget(row, 2));
            if (spinner) {
                spinner->setValue(0);
            }
        }
    }
}

void CoreSkillsWidget::onSaveClicked()
{
    QString engineerId = engineerCombo_->currentData().toString();

    if (engineerId.isEmpty()) {
        QMessageBox::warning(this, "Selection Required", "Please select an engineer.");
        return;
    }

    int savedCount = 0;
    int errorCount = 0;

    for (int row = 0; row < skillsTable_->rowCount(); ++row) {
        QString ids = skillsTable_->item(row, 3)->text();
        QStringList parts = ids.split('|');

        if (parts.size() == 2) {
            QString categoryId = parts[0];
            QString skillId = parts[1];

            QSpinBox* spinner = qobject_cast<QSpinBox*>(skillsTable_->cellWidget(row, 2));
            if (spinner) {
                int score = spinner->value();

                CoreSkillAssessment assessment;
                assessment.setEngineerId(engineerId);
                assessment.setCategoryId(categoryId);
                assessment.setSkillId(skillId);
                assessment.setScore(score);

                if (coreSkillsRepo_.saveOrUpdateAssessment(assessment)) {
                    savedCount++;
                } else {
                    errorCount++;
                    Logger::instance().error("CoreSkillsWidget",
                        QString("Failed to save assessment: %1").arg(coreSkillsRepo_.lastError()));
                }
            }
        }
    }

    if (errorCount > 0) {
        QMessageBox::warning(this, "Partial Success",
            QString("Saved %1 assessments, but %2 failed.").arg(savedCount).arg(errorCount));
    } else {
        Logger::instance().info("CoreSkillsWidget", QString("Saved %1 core skill assessments").arg(savedCount));
        QMessageBox::information(this, "Success",
            QString("Successfully saved %1 core skill assessments.").arg(savedCount));
    }
}

void CoreSkillsWidget::onRefreshClicked()
{
    loadEngineers();
    loadCoreSkills();
    if (engineerCombo_->currentIndex() > 0) {
        loadAssessments();
    }
}
