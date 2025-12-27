#include "MyCoreSkillsWidget.h"
#include "../utils/Logger.h"
#include "../core/Constants.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QGroupBox>
#include <QFont>

MyCoreSkillsWidget::MyCoreSkillsWidget(const QString& engineerId, QWidget* parent)
    : QWidget(parent)
    , engineerId_(engineerId)
    , skillsTable_(nullptr)
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

    // Skills Table
    QLabel* tableLabel = new QLabel("Skill Levels: 0 = Not Assessed | 1 = Basic | 2 = Intermediate | 3 = Advanced", this);
    tableLabel->setStyleSheet("QLabel { color: #666; font-size: 11pt; }");
    mainLayout->addWidget(tableLabel);

    skillsTable_ = new QTableWidget(this);
    skillsTable_->setColumnCount(3);
    skillsTable_->setHorizontalHeaderLabels({"Category", "Skill", "Your Score"});
    skillsTable_->horizontalHeader()->setStretchLastSection(true);
    skillsTable_->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    skillsTable_->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    skillsTable_->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    skillsTable_->setAlternatingRowColors(true);
    skillsTable_->setSortingEnabled(true);
    skillsTable_->setEditTriggers(QAbstractItemView::NoEditTriggers);  // Read-only
    skillsTable_->setSelectionBehavior(QAbstractItemView::SelectRows);
    skillsTable_->sortByColumn(0, Qt::AscendingOrder);

    mainLayout->addWidget(skillsTable_);

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
    // Disable sorting while loading
    skillsTable_->setSortingEnabled(false);
    skillsTable_->setRowCount(0);

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

    // Build the table
    int row = 0;
    int assessedCount = 0;
    int totalScore = 0;
    int maxPossibleScore = 0;

    for (const CoreSkillCategory& category : categories) {
        for (const CoreSkill& skill : skills) {
            if (skill.categoryId() == category.id()) {
                skillsTable_->insertRow(row);

                // Category
                QTableWidgetItem* categoryItem = new QTableWidgetItem(category.name());
                skillsTable_->setItem(row, 0, categoryItem);

                // Skill name
                QTableWidgetItem* skillItem = new QTableWidgetItem(skill.name());
                skillsTable_->setItem(row, 1, skillItem);

                // Score
                int score = skillScores.value(skill.id(), 0);
                QTableWidgetItem* scoreItem = new QTableWidgetItem(QString::number(score));
                scoreItem->setTextAlignment(Qt::AlignCenter);

                // Color code the score
                if (score == 0) {
                    scoreItem->setForeground(QBrush(QColor("#999")));  // Gray for not assessed
                } else if (score == 1) {
                    scoreItem->setForeground(QBrush(QColor("#f59e0b")));  // Orange for basic
                } else if (score == 2) {
                    scoreItem->setForeground(QBrush(QColor("#3b82f6")));  // Blue for intermediate
                } else if (score == 3) {
                    scoreItem->setForeground(QBrush(QColor("#10b981")));  // Green for advanced
                }

                QFont scoreFont = scoreItem->font();
                scoreFont.setBold(true);
                scoreFont.setPointSize(12);
                scoreItem->setFont(scoreFont);

                skillsTable_->setItem(row, 2, scoreItem);

                // Track statistics
                if (score > 0) {
                    assessedCount++;
                    totalScore += score;
                }
                maxPossibleScore += skill.maxScore();

                row++;
            }
        }
    }

    // Re-enable sorting
    skillsTable_->setSortingEnabled(true);
    skillsTable_->sortByColumn(0, Qt::AscendingOrder);

    // Update summary
    int totalSkills = row;
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
