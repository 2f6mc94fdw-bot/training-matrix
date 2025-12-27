#include "MyCoreSkillsWidget.h"
#include "../utils/Logger.h"
#include "../core/Constants.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QGroupBox>
#include <QFont>
#include <QScrollArea>
#include <QMessageBox>

MyCoreSkillsWidget::MyCoreSkillsWidget(const QString& engineerId, QWidget* parent)
    : QWidget(parent)
    , engineerId_(engineerId)
    , skillsLayout_(nullptr)
    , skillsContainer_(nullptr)
    , saveButton_(nullptr)
    , refreshButton_(nullptr)
    , summaryLabel_(nullptr)
{
    setupUI();
    loadCoreSkills();
    Logger::instance().info("MyCoreSkillsWidget", QString("My Core Skills widget initialized for engineer: %1").arg(engineerId_));
}

MyCoreSkillsWidget::~MyCoreSkillsWidget()
{
}

void MyCoreSkillsWidget::setupUI()
{
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(20);
    mainLayout->setContentsMargins(30, 30, 30, 30);

    // Title
    QLabel* titleLabel = new QLabel("My Core Skills", this);
    QFont titleFont = titleLabel->font();
    titleFont.setPointSize(20);
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);
    mainLayout->addWidget(titleLabel);

    // Description
    QLabel* descLabel = new QLabel("View and update your core skill assessments across Mechanical, Electrical, and Software competencies.", this);
    descLabel->setWordWrap(true);
    mainLayout->addWidget(descLabel);

    // Summary
    summaryLabel_ = new QLabel("Loading...", this);
    QFont summaryFont = summaryLabel_->font();
    summaryFont.setPointSize(12);
    summaryFont.setBold(true);
    summaryLabel_->setFont(summaryFont);
    summaryLabel_->setStyleSheet("QLabel { color: " + QString(Constants::BRAND_ACCENT) + "; padding: 10px; }");
    mainLayout->addWidget(summaryLabel_);

    // Skill level legend
    QLabel* legendLabel = new QLabel("Skill Levels: 0 = Not Assessed | 1 = Basic | 2 = Intermediate | 3 = Advanced", this);
    legendLabel->setStyleSheet("QLabel { color: #666; font-size: 11pt; }");
    mainLayout->addWidget(legendLabel);

    // Scrollable skills container
    QScrollArea* scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);

    skillsContainer_ = new QWidget();
    skillsLayout_ = new QVBoxLayout(skillsContainer_);
    skillsLayout_->setSpacing(16);
    skillsLayout_->setContentsMargins(0, 0, 0, 0);

    scrollArea->setWidget(skillsContainer_);
    mainLayout->addWidget(scrollArea);

    // Buttons
    QHBoxLayout* buttonLayout = new QHBoxLayout();

    saveButton_ = new QPushButton("Save My Assessments", this);
    refreshButton_ = new QPushButton("Refresh", this);

    connect(saveButton_, &QPushButton::clicked, this, &MyCoreSkillsWidget::onSaveClicked);
    connect(refreshButton_, &QPushButton::clicked, this, &MyCoreSkillsWidget::onRefreshClicked);

    buttonLayout->addWidget(saveButton_);
    buttonLayout->addStretch();
    buttonLayout->addWidget(refreshButton_);

    mainLayout->addLayout(buttonLayout);

    setLayout(mainLayout);
}

void MyCoreSkillsWidget::loadCoreSkills()
{
    // Clear existing widgets
    QLayoutItem* item;
    while ((item = skillsLayout_->takeAt(0)) != nullptr) {
        if (item->widget()) {
            item->widget()->deleteLater();
        }
        delete item;
    }
    scoreButtonGroups_.clear();

    // Load all categories and skills
    QList<CoreSkillCategory> categories = coreSkillsRepo_.findAllCategories();
    QList<CoreSkill> skills = coreSkillsRepo_.findAllSkills();

    // Load this engineer's assessments
    QList<CoreSkillAssessment> assessments = coreSkillsRepo_.findAllAssessments();

    // Create a map of skill_id -> score for quick lookup
    QMap<QString, int> skillScores;
    for (const CoreSkillAssessment& assessment : assessments) {
        if (assessment.engineerId() == engineerId_) {
            skillScores[assessment.skillId()] = assessment.score();
        }
    }

    // Track statistics
    int assessedCount = 0;
    int totalScore = 0;
    int maxPossibleScore = 0;
    int totalSkills = 0;

    // Create a card for each category
    for (const CoreSkillCategory& category : categories) {
        // Create category card
        QGroupBox* categoryCard = new QGroupBox(this);
        categoryCard->setStyleSheet(
            "QGroupBox {"
            "    border: 2px solid #e2e8f0;"
            "    border-radius: 8px;"
            "    padding: 16px;"
            "    background-color: transparent;"
            "    margin-top: 12px;"
            "}"
            "QGroupBox::title {"
            "    subcontrol-origin: margin;"
            "    left: 16px;"
            "    padding: 0 8px 0 8px;"
            "}"
        );

        QVBoxLayout* cardLayout = new QVBoxLayout(categoryCard);
        cardLayout->setSpacing(12);

        // Category title
        QLabel* categoryLabel = new QLabel(category.name(), this);
        QFont categoryFont = categoryLabel->font();
        categoryFont.setPointSize(14);
        categoryFont.setBold(true);
        categoryLabel->setFont(categoryFont);
        cardLayout->addWidget(categoryLabel);

        // Add skills for this category
        bool hasSkills = false;
        for (const CoreSkill& skill : skills) {
            if (skill.categoryId() == category.id()) {
                hasSkills = true;
                totalSkills++;

                QHBoxLayout* skillLayout = new QHBoxLayout();
                skillLayout->setSpacing(12);
                skillLayout->setContentsMargins(0, 4, 0, 4);

                // Skill name
                QLabel* skillLabel = new QLabel(skill.name(), this);
                QFont skillFont = skillLabel->font();
                skillFont.setPointSize(13);
                skillLabel->setFont(skillFont);
                skillLabel->setWordWrap(true);
                skillLabel->setMinimumWidth(250);
                skillLabel->setMaximumWidth(500);
                skillLayout->addWidget(skillLabel, 1);

                skillLayout->addStretch();

                // Get current score
                int currentScore = skillScores.value(skill.id(), 0);

                // Create score buttons (0-3)
                createScoreButtons(skillLayout, category.id(), skill.id(), currentScore);

                cardLayout->addLayout(skillLayout);

                // Track statistics
                if (currentScore > 0) {
                    assessedCount++;
                    totalScore += currentScore;
                }
                maxPossibleScore += skill.maxScore();
            }
        }

        // Only add the card if it has skills
        if (hasSkills) {
            skillsLayout_->addWidget(categoryCard);
        } else {
            delete categoryCard;
        }
    }

    // Add stretch at the end
    skillsLayout_->addStretch();

    // Update summary
    double completionRate = totalSkills > 0 ? (double)assessedCount / totalSkills * 100.0 : 0.0;
    double averageScore = assessedCount > 0 ? (double)totalScore / assessedCount : 0.0;
    double overallScore = maxPossibleScore > 0 ? (double)totalScore / maxPossibleScore * 100.0 : 0.0;

    QString summaryText = QString("📊 %1 of %2 skills assessed (%.1%%) | Average Score: %.2 / 3 | Overall: %.1%%")
        .arg(assessedCount)
        .arg(totalSkills)
        .arg(completionRate, 0, 'f', 1)
        .arg(averageScore, 0, 'f', 2)
        .arg(overallScore, 0, 'f', 1);

    summaryLabel_->setText(summaryText);

    Logger::instance().info("MyCoreSkillsWidget",
        QString("Loaded %1 core skills (%2 assessed) for engineer %3")
        .arg(totalSkills)
        .arg(assessedCount)
        .arg(engineerId_));
}

void MyCoreSkillsWidget::createScoreButtons(QHBoxLayout* layout, const QString& categoryId,
                                           const QString& skillId, int currentScore)
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
    buttonGroup.categoryId = categoryId;
    buttonGroup.skillId = skillId;

    for (int score = 0; score < 4; score++) {
        QPushButton* button = new QPushButton(scoreInfos[score].label);
        button->setFixedSize(32, 32);
        button->setCursor(Qt::PointingHandCursor);

        // Store metadata
        button->setProperty("categoryId", categoryId);
        button->setProperty("skillId", skillId);
        button->setProperty("score", score);

        // Style button based on whether it's selected
        bool isSelected = (score == currentScore);
        button->setProperty("isSelected", isSelected);  // Store for save detection

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

        connect(button, &QPushButton::clicked, this, &MyCoreSkillsWidget::onScoreButtonClicked);

        layout->addWidget(button);
        buttonGroup.buttons[score] = button;
    }

    scoreButtonGroups_.append(buttonGroup);
}

void MyCoreSkillsWidget::onScoreButtonClicked()
{
    QPushButton* clickedButton = qobject_cast<QPushButton*>(sender());
    if (!clickedButton) {
        return;
    }

    QString categoryId = clickedButton->property("categoryId").toString();
    QString skillId = clickedButton->property("skillId").toString();
    int score = clickedButton->property("score").toInt();

    // Color scheme
    QStringList scoreColors = {"#ff6b6b", "#fbbf24", "#60a5fa", "#4ade80"};

    // Find the button group and update all buttons in that group
    for (ScoreButtonGroup& buttonGroup : scoreButtonGroups_) {
        if (buttonGroup.categoryId == categoryId &&
            buttonGroup.skillId == skillId) {

            // Update styles for all buttons in this group
            for (int i = 0; i < 4; i++) {
                QPushButton* button = buttonGroup.buttons[i];
                bool isSelected = (i == score);

                // Store selected state as property for reliable save detection
                button->setProperty("isSelected", isSelected);

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
            Logger::instance().info("MyCoreSkillsWidget",
                QString("Score changed to %1 for skill %2")
                    .arg(score)
                    .arg(skillId));

            break;
        }
    }
}

void MyCoreSkillsWidget::onSaveClicked()
{
    int savedCount = 0;
    int errorCount = 0;

    // Loop through all button groups and find selected score for each skill
    for (const ScoreButtonGroup& buttonGroup : scoreButtonGroups_) {
        // Find which button is selected using the isSelected property
        int selectedScore = 0;
        for (int score = 0; score < 4; score++) {
            QPushButton* button = buttonGroup.buttons[score];
            if (button->property("isSelected").toBool()) {
                selectedScore = score;
                break;
            }
        }

        CoreSkillAssessment assessment;
        assessment.setEngineerId(engineerId_);
        assessment.setCategoryId(buttonGroup.categoryId);
        assessment.setSkillId(buttonGroup.skillId);
        assessment.setScore(selectedScore);

        if (coreSkillsRepo_.saveOrUpdateAssessment(assessment)) {
            savedCount++;
        } else {
            errorCount++;
            Logger::instance().error("MyCoreSkillsWidget",
                QString("Failed to save assessment: %1").arg(coreSkillsRepo_.lastError()));
        }
    }

    if (errorCount > 0) {
        QMessageBox::warning(this, "Partial Success",
            QString("Saved %1 assessments, but %2 failed.").arg(savedCount).arg(errorCount));
    } else {
        Logger::instance().info("MyCoreSkillsWidget", QString("Saved %1 core skill assessments").arg(savedCount));
        QMessageBox::information(this, "Success",
            QString("Successfully saved %1 core skill assessments.").arg(savedCount));

        // Refresh to update summary statistics
        loadCoreSkills();
    }
}

void MyCoreSkillsWidget::onRefreshClicked()
{
    loadCoreSkills();
    Logger::instance().info("MyCoreSkillsWidget", "Core skills refreshed");
}
