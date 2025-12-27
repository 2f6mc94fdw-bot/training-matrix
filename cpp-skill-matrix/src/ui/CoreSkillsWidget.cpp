#include "CoreSkillsWidget.h"
#include "../utils/Logger.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QHeaderView>
#include <QMessageBox>
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
    skillsTable_->setColumnCount(3);
    skillsTable_->setHorizontalHeaderLabels({"Category", "Skill", "Score"});

    // Configure column sizing for proper display
    skillsTable_->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents); // Category
    skillsTable_->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);          // Skill (takes remaining space)
    skillsTable_->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Fixed);            // Score (fixed width)
    skillsTable_->setColumnWidth(2, 180);  // Set Score column width for button group

    skillsTable_->setAlternatingRowColors(true);
    skillsTable_->setSortingEnabled(true);  // Enable column sorting
    skillsTable_->sortByColumn(0, Qt::AscendingOrder);  // Default sort by Category

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
    scoreButtonGroups_.clear();

    QList<CoreSkillCategory> categories = coreSkillsRepo_.findAllCategories();
    QList<CoreSkill> skills = coreSkillsRepo_.findAllSkills();

    QString engineerId = engineerCombo_->currentData().toString();

    int row = 0;
    for (const CoreSkillCategory& category : categories) {
        // Find skills for this category
        for (const CoreSkill& skill : skills) {
            if (skill.categoryId() == category.id()) {
                skillsTable_->insertRow(row);

                skillsTable_->setItem(row, 0, new QTableWidgetItem(category.name()));
                skillsTable_->setItem(row, 1, new QTableWidgetItem(skill.name()));

                // Create score buttons widget
                QWidget* buttonWidget = new QWidget(this);
                QHBoxLayout* buttonLayout = new QHBoxLayout(buttonWidget);
                buttonLayout->setContentsMargins(4, 2, 4, 2);
                buttonLayout->setSpacing(6);

                createScoreButtons(buttonLayout, engineerId, category.id(), skill.id(), 0);

                buttonWidget->setLayout(buttonLayout);
                skillsTable_->setCellWidget(row, 2, buttonWidget);

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

    // Color scheme for score buttons
    QStringList scoreColors = {"#ff6b6b", "#fbbf24", "#60a5fa", "#4ade80"};  // Red, Yellow, Blue, Green

    // Update button groups with existing scores
    for (ScoreButtonGroup& buttonGroup : scoreButtonGroups_) {
        if (buttonGroup.engineerId != engineerId) {
            continue;
        }

        int currentScore = 0;

        // Find matching assessment
        for (const CoreSkillAssessment& assessment : assessments) {
            if (assessment.engineerId() == engineerId &&
                assessment.categoryId() == buttonGroup.categoryId &&
                assessment.skillId() == buttonGroup.skillId) {
                currentScore = assessment.score();
                break;
            }
        }

        // Update button styles
        for (int score = 0; score < 4; score++) {
            QPushButton* button = buttonGroup.buttons[score];
            bool isSelected = (score == currentScore);

            QString buttonStyle;
            if (isSelected) {
                buttonStyle = QString(
                    "QPushButton {"
                    "    background-color: %1;"
                    "    color: white;"
                    "    border: 2px solid %1;"
                    "    border-radius: 16px;"
                    "    font-weight: bold;"
                    "    font-size: 12px;"
                    "}"
                    "QPushButton:hover {"
                    "    opacity: 0.9;"
                    "}"
                ).arg(scoreColors[score]);
            } else {
                buttonStyle = QString(
                    "QPushButton {"
                    "    background-color: transparent;"
                    "    color: #64748b;"
                    "    border: 2px solid #e2e8f0;"
                    "    border-radius: 16px;"
                    "    font-size: 12px;"
                    "}"
                    "QPushButton:hover {"
                    "    border-color: %1;"
                    "    color: %1;"
                    "    background-color: rgba(255, 255, 255, 0.05);"
                    "}"
                ).arg(scoreColors[score]);
            }

            button->setStyleSheet(buttonStyle);
        }
    }

    Logger::instance().info("CoreSkillsWidget", "Loaded assessments for engineer: " + engineerId);
}

void CoreSkillsWidget::onEngineerChanged(int index)
{
    QString newEngineerId = engineerCombo_->currentData().toString();

    // Update all button groups with new engineer ID and reset to score 0
    QStringList scoreColors = {"#ff6b6b", "#fbbf24", "#60a5fa", "#4ade80"};

    for (ScoreButtonGroup& buttonGroup : scoreButtonGroups_) {
        buttonGroup.engineerId = newEngineerId;

        // Reset all buttons to unselected state, with button 0 selected
        for (int score = 0; score < 4; score++) {
            QPushButton* button = buttonGroup.buttons[score];
            button->setProperty("engineerId", newEngineerId);

            bool isSelected = (score == 0);  // Default to score 0
            QString buttonStyle;
            if (isSelected) {
                buttonStyle = QString(
                    "QPushButton {"
                    "    background-color: %1;"
                    "    color: white;"
                    "    border: 2px solid %1;"
                    "    border-radius: 16px;"
                    "    font-weight: bold;"
                    "    font-size: 12px;"
                    "}"
                    "QPushButton:hover {"
                    "    opacity: 0.9;"
                    "}"
                ).arg(scoreColors[score]);
            } else {
                buttonStyle = QString(
                    "QPushButton {"
                    "    background-color: transparent;"
                    "    color: #64748b;"
                    "    border: 2px solid #e2e8f0;"
                    "    border-radius: 16px;"
                    "    font-size: 12px;"
                    "}"
                    "QPushButton:hover {"
                    "    border-color: %1;"
                    "    color: %1;"
                    "    background-color: rgba(255, 255, 255, 0.05);"
                    "}"
                ).arg(scoreColors[score]);
            }
            button->setStyleSheet(buttonStyle);
        }
    }

    // If an engineer is selected, load their assessments
    if (index > 0) {
        loadAssessments();
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

    // Loop through all button groups and find selected score for each skill
    for (const ScoreButtonGroup& buttonGroup : scoreButtonGroups_) {
        if (buttonGroup.engineerId != engineerId) {
            continue;
        }

        // Find which button is selected (has the score property and colored background)
        int selectedScore = 0;
        for (int score = 0; score < 4; score++) {
            QPushButton* button = buttonGroup.buttons[score];
            // Check if button has a colored background (contains "background-color: #" in style)
            if (button->styleSheet().contains("background-color: #")) {
                selectedScore = score;
                break;
            }
        }

        CoreSkillAssessment assessment;
        assessment.setEngineerId(engineerId);
        assessment.setCategoryId(buttonGroup.categoryId);
        assessment.setSkillId(buttonGroup.skillId);
        assessment.setScore(selectedScore);

        if (coreSkillsRepo_.saveOrUpdateAssessment(assessment)) {
            savedCount++;
        } else {
            errorCount++;
            Logger::instance().error("CoreSkillsWidget",
                QString("Failed to save assessment: %1").arg(coreSkillsRepo_.lastError()));
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

void CoreSkillsWidget::createScoreButtons(QHBoxLayout* layout, const QString& engineerId,
                                         const QString& categoryId, const QString& skillId,
                                         int currentScore)
{
    // Score labels and colors
    struct ScoreInfo {
        QString label;
        QString color;
    };

    QList<ScoreInfo> scoreInfos = {
        {"0", "#ff6b6b"},  // Not Trained - Red
        {"1", "#fbbf24"},  // Basic - Yellow
        {"2", "#60a5fa"},  // Competent - Blue
        {"3", "#4ade80"}   // Expert - Green
    };

    ScoreButtonGroup buttonGroup;
    buttonGroup.engineerId = engineerId;
    buttonGroup.categoryId = categoryId;
    buttonGroup.skillId = skillId;

    for (int score = 0; score < 4; score++) {
        QPushButton* button = new QPushButton(scoreInfos[score].label, this);
        button->setFixedSize(32, 32);
        button->setCursor(Qt::PointingHandCursor);

        // Store metadata
        button->setProperty("engineerId", engineerId);
        button->setProperty("categoryId", categoryId);
        button->setProperty("skillId", skillId);
        button->setProperty("score", score);

        // Style button based on whether it's selected
        bool isSelected = (score == currentScore);

        QString buttonStyle;
        if (isSelected) {
            // Active button: colored background, white text
            buttonStyle = QString(
                "QPushButton {"
                "    background-color: %1;"
                "    color: white;"
                "    border: 2px solid %1;"
                "    border-radius: 16px;"
                "    font-weight: bold;"
                "    font-size: 12px;"
                "}"
                "QPushButton:hover {"
                "    opacity: 0.9;"
                "}"
            ).arg(scoreInfos[score].color);
        } else {
            // Inactive button: transparent background, colored border
            buttonStyle = QString(
                "QPushButton {"
                "    background-color: transparent;"
                "    color: #64748b;"
                "    border: 2px solid #e2e8f0;"
                "    border-radius: 16px;"
                "    font-size: 12px;"
                "}"
                "QPushButton:hover {"
                "    border-color: %1;"
                "    color: %1;"
                "    background-color: rgba(255, 255, 255, 0.05);"
                "}"
            ).arg(scoreInfos[score].color);
        }

        button->setStyleSheet(buttonStyle);

        connect(button, &QPushButton::clicked, this, &CoreSkillsWidget::onScoreButtonClicked);

        layout->addWidget(button);
        buttonGroup.buttons[score] = button;
    }

    scoreButtonGroups_.append(buttonGroup);
}

void CoreSkillsWidget::onScoreButtonClicked()
{
    QPushButton* clickedButton = qobject_cast<QPushButton*>(sender());
    if (!clickedButton) {
        return;
    }

    QString engineerId = clickedButton->property("engineerId").toString();
    QString categoryId = clickedButton->property("categoryId").toString();
    QString skillId = clickedButton->property("skillId").toString();
    int score = clickedButton->property("score").toInt();

    // Color scheme
    QStringList scoreColors = {"#ff6b6b", "#fbbf24", "#60a5fa", "#4ade80"};

    // Find the button group and update all buttons in that group
    for (ScoreButtonGroup& buttonGroup : scoreButtonGroups_) {
        if (buttonGroup.engineerId == engineerId &&
            buttonGroup.categoryId == categoryId &&
            buttonGroup.skillId == skillId) {

            // Update styles for all buttons in this group
            for (int i = 0; i < 4; i++) {
                QPushButton* button = buttonGroup.buttons[i];
                bool isSelected = (i == score);

                QString buttonStyle;
                if (isSelected) {
                    buttonStyle = QString(
                        "QPushButton {"
                        "    background-color: %1;"
                        "    color: white;"
                        "    border: 2px solid %1;"
                        "    border-radius: 16px;"
                        "    font-weight: bold;"
                        "    font-size: 12px;"
                        "}"
                        "QPushButton:hover {"
                        "    opacity: 0.9;"
                        "}"
                    ).arg(scoreColors[i]);
                } else {
                    buttonStyle = QString(
                        "QPushButton {"
                        "    background-color: transparent;"
                        "    color: #64748b;"
                        "    border: 2px solid #e2e8f0;"
                        "    border-radius: 16px;"
                        "    font-size: 12px;"
                        "}"
                        "QPushButton:hover {"
                        "    border-color: %1;"
                        "    color: %1;"
                        "    background-color: rgba(255, 255, 255, 0.05);"
                        "}"
                    ).arg(scoreColors[i]);
                }

                button->setStyleSheet(buttonStyle);
            }

            // Log the score change
            Logger::instance().info("CoreSkillsWidget",
                QString("Score changed to %1 for skill %2 (engineer %3)")
                    .arg(score)
                    .arg(skillId)
                    .arg(engineerId));

            break;
        }
    }
}
