#include "MyCoreSkillsWidget.h"
#include "../utils/Logger.h"
#include "../core/Constants.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QGroupBox>
#include <QFont>
#include <QScrollArea>

MyCoreSkillsWidget::MyCoreSkillsWidget(const QString& engineerId, QWidget* parent)
    : QWidget(parent)
    , engineerId_(engineerId)
    , skillsLayout_(nullptr)
    , skillsContainer_(nullptr)
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
    QLabel* descLabel = new QLabel("View your core skill assessments across Mechanical, Electrical, and Software competencies.", this);
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

    // Refresh Button
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();

    refreshButton_ = new QPushButton("Refresh", this);
    connect(refreshButton_, &QPushButton::clicked, this, &MyCoreSkillsWidget::onRefreshClicked);

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

    // Score colors
    QStringList scoreColors = {"#999999", "#f59e0b", "#3b82f6", "#10b981"};  // Gray, Orange, Blue, Green
    QStringList scoreLabels = {"Not Assessed", "Basic", "Intermediate", "Advanced"};

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

                // Score badge
                int score = skillScores.value(skill.id(), 0);

                QLabel* scoreLabel = new QLabel(scoreLabels[score], this);
                scoreLabel->setAlignment(Qt::AlignCenter);
                scoreLabel->setFixedSize(120, 32);

                QFont scoreLabelFont = scoreLabel->font();
                scoreLabelFont.setBold(true);
                scoreLabelFont.setPointSize(12);
                scoreLabel->setFont(scoreLabelFont);

                QString scoreBadgeStyle = QString(
                    "QLabel {"
                    "    background-color: %1;"
                    "    color: white;"
                    "    border-radius: 16px;"
                    "    padding: 4px 12px;"
                    "}"
                ).arg(scoreColors[score]);

                scoreLabel->setStyleSheet(scoreBadgeStyle);
                skillLayout->addWidget(scoreLabel);

                cardLayout->addLayout(skillLayout);

                // Track statistics
                if (score > 0) {
                    assessedCount++;
                    totalScore += score;
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

void MyCoreSkillsWidget::onRefreshClicked()
{
    loadCoreSkills();
    Logger::instance().info("MyCoreSkillsWidget", "Core skills refreshed");
}
