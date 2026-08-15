#include "MyCoreSkillsWidget.h"
#include "../utils/Logger.h"
#include "../core/Constants.h"
#include "../core/Application.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QGroupBox>
#include <QFont>
#include <QScrollArea>
#include <QMessageBox>
#include <QGraphicsDropShadowEffect>

namespace {
QString coreSkillDiscipline(const CoreSkillCategory& category)
{
    if (!category.discipline().trimmed().isEmpty()) {
        return category.discipline().trimmed();
    }
    const QString id = category.id().toLower();
    const QString name = category.name().toLower();

    if (name.contains("mechanical") || id.contains("mech")) {
        return "Mechanical";
    }
    if (name.contains("electrical") || id.contains("elec")) {
        return "Electrical";
    }
    if (name.contains("software") || id.contains("soft")) {
        return "Software";
    }

    return "Software";
}
}

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
    QLabel* descLabel = new QLabel(
        "Propose changes to your Mechanical, Electrical, and Software scores. Changes become official after manager approval.", this);
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

    saveButton_ = new QPushButton("Submit Changes for Approval", this);
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
    QMap<QString, QList<CoreSkill>> skillsByCategory;
    for (const CoreSkill& skill : skills) {
        skillsByCategory[skill.categoryId()].append(skill);
    }

    // Load this engineer's assessments
    QList<CoreSkillAssessment> assessments = coreSkillsRepo_.findAllAssessments();

    // Create a map of skill_id -> score for quick lookup
    QMap<QString, int> skillScores;
    for (const CoreSkillAssessment& assessment : assessments) {
        if (assessment.engineerId() == engineerId_) {
            skillScores[assessment.skillId()] = assessment.score();
        }
    }

    QMap<QString, int> pendingScores;
    const QList<AssessmentSubmission> pendingSubmissions =
        submissionRepo_.findPendingByEngineer(engineerId_, "core");
    for (const AssessmentSubmission& submission : pendingSubmissions) {
        pendingScores[submission.skillId] = submission.proposedScore;
    }

    // Track statistics
    int assessedCount = 0;
    int totalScore = 0;
    int maxPossibleScore = 0;
    int totalSkills = 0;

    QMap<QString, QList<CoreSkillCategory>> categoriesByDiscipline;
    for (const CoreSkillCategory& category : categories) {
        categoriesByDiscipline[coreSkillDiscipline(category)].append(category);
    }

    const QStringList disciplineOrder = {"Mechanical", "Electrical", "Software"};

    // Create one card per discipline and render categories within each card.
    for (const QString& discipline : disciplineOrder) {
        const QList<CoreSkillCategory> disciplineCategories = categoriesByDiscipline.value(discipline);
        if (disciplineCategories.isEmpty()) {
            continue;
        }

        QGroupBox* disciplineCard = new QGroupBox(this);
        disciplineCard->setStyleSheet(
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

        QVBoxLayout* cardLayout = new QVBoxLayout(disciplineCard);
        cardLayout->setSpacing(12);

        QLabel* disciplineLabel = new QLabel(QString("%1 Skills").arg(discipline), this);
        QFont disciplineFont = disciplineLabel->font();
        disciplineFont.setPointSize(14);
        disciplineFont.setBold(true);
        disciplineLabel->setFont(disciplineFont);
        cardLayout->addWidget(disciplineLabel);

        bool disciplineHasSkills = false;
        for (const CoreSkillCategory& category : disciplineCategories) {
            const QList<CoreSkill> categorySkills = skillsByCategory.value(category.id());
            if (categorySkills.isEmpty()) {
                continue;
            }

            disciplineHasSkills = true;

            QLabel* categoryLabel = new QLabel(category.name(), this);
            QFont categoryFont = categoryLabel->font();
            categoryFont.setPointSize(12);
            categoryFont.setBold(true);
            categoryLabel->setFont(categoryFont);
            categoryLabel->setStyleSheet("QLabel { color: #475569; margin-top: 6px; }");
            cardLayout->addWidget(categoryLabel);

            for (const CoreSkill& skill : categorySkills) {
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

                skillLayout->addSpacing(16);

                // Get current score
                const int approvedScore = skillScores.value(skill.id(), 0);
                const bool pendingApproval = pendingScores.contains(skill.id());
                const int currentScore = pendingApproval
                    ? pendingScores.value(skill.id())
                    : approvedScore;

                // Create score buttons (0-3)
                createScoreButtons(skillLayout, category.id(), skill.id(), currentScore, pendingApproval);

                cardLayout->addLayout(skillLayout);

                // Track statistics
                if (approvedScore > 0) {
                    assessedCount++;
                    totalScore += approvedScore;
                }
                maxPossibleScore += skill.maxScore();
            }
        }

        if (disciplineHasSkills) {
            skillsLayout_->addWidget(disciplineCard);
        } else {
            delete disciplineCard;
        }
    }

    // Add stretch at the end
    skillsLayout_->addStretch();

    // Update summary
    double completionRate = totalSkills > 0 ? (double)assessedCount / totalSkills * 100.0 : 0.0;
    double averageScore = assessedCount > 0 ? (double)totalScore / assessedCount : 0.0;
    double overallScore = maxPossibleScore > 0 ? (double)totalScore / maxPossibleScore * 100.0 : 0.0;

    QString summaryText = QString("Approved: %1 of %2 skills assessed (%3%) | Average: %4 / 3 | Overall: %5% | Pending: %6")
        .arg(assessedCount)
        .arg(totalSkills)
        .arg(completionRate, 0, 'f', 1)
        .arg(averageScore, 0, 'f', 2)
        .arg(overallScore, 0, 'f', 1)
        .arg(pendingSubmissions.size());

    summaryLabel_->setText(summaryText);

    Logger::instance().info("MyCoreSkillsWidget",
        QString("Loaded %1 core skills (%2 assessed) for engineer %3")
        .arg(totalSkills)
        .arg(assessedCount)
        .arg(engineerId_));
}

void MyCoreSkillsWidget::createScoreButtons(QHBoxLayout* layout, const QString& categoryId,
                                           const QString& skillId, int currentScore, bool pendingApproval)
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
        button->setFixedSize(60, 48);
        button->setCursor(Qt::PointingHandCursor);
        auto* shadow = new QGraphicsDropShadowEffect(button);
        shadow->setBlurRadius(12);
        shadow->setOffset(0, 2);
        shadow->setColor(QColor(15, 23, 42, 40));
        button->setGraphicsEffect(shadow);

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
                "    border-radius: 14px;"
                "    padding: 0px;"
                "    min-width: 0px;"
                "    min-height: 0px;"
                "    text-align: center;"
                "    font-weight: bold;"
                "    font-size: 20px;"
                "}"
                "QPushButton:hover {"
                "    border: 2px solid #1e293b;"
                "    background-color: %1;"
                "}"
            ).arg(scoreInfos[score].color);
        } else {
            // Inactive button: transparent background, colored border
            buttonStyle = QString(
                "QPushButton {"
                "    background-color: #f8fafc;"
                "    color: #475569;"
                "    border: 2px solid #cbd5e1;"
                "    border-radius: 14px;"
                "    padding: 0px;"
                "    min-width: 0px;"
                "    min-height: 0px;"
                "    text-align: center;"
                "    font-size: 20px;"
                "    font-weight: bold;"
                "}"
                "QPushButton:hover {"
                "    border-color: %1;"
                "    color: %1;"
                "    background-color: #e2e8f0;"
                "}"
            ).arg(scoreInfos[score].color);
        }

        button->setStyleSheet(buttonStyle);

        connect(button, &QPushButton::clicked, this, &MyCoreSkillsWidget::onScoreButtonClicked);

        layout->addWidget(button);
        buttonGroup.buttons[score] = button;
    }

    if (pendingApproval) {
        QLabel* pendingLabel = new QLabel("Pending approval", this);
        pendingLabel->setStyleSheet(
            "QLabel { color: #b45309; background: #fffbeb; border: 1px solid #fbbf24; "
            "border-radius: 8px; padding: 6px 10px; font-weight: 600; }");
        layout->addWidget(pendingLabel);
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
                        "    border-radius: 14px;"
                        "    padding: 0px;"
                        "    min-width: 0px;"
                        "    min-height: 0px;"
                        "    text-align: center;"
                        "    font-weight: bold;"
                        "    font-size: 20px;"
                        "}"
                        "QPushButton:hover {"
                        "    border: 2px solid #1e293b;"
                        "    background-color: %1;"
                        "}"
                    ).arg(scoreColors[i]);
                } else {
                    buttonStyle = QString(
                        "QPushButton {"
                        "    background-color: #f8fafc;"
                        "    color: #475569;"
                        "    border: 2px solid #cbd5e1;"
                        "    border-radius: 14px;"
                        "    padding: 0px;"
                        "    min-width: 0px;"
                        "    min-height: 0px;"
                        "    text-align: center;"
                        "    font-size: 20px;"
                        "    font-weight: bold;"
                        "}"
                        "QPushButton:hover {"
                        "    border-color: %1;"
                        "    color: %1;"
                        "    background-color: #e2e8f0;"
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
    int submittedCount = 0;
    int errorCount = 0;

    const QList<CoreSkillAssessment> existingAssessments = coreSkillsRepo_.findAllAssessments();
    QMap<QString, int> existingScores;
    for (const CoreSkillAssessment& assessment : existingAssessments) {
        if (assessment.engineerId() != engineerId_) {
            continue;
        }
        existingScores[assessment.skillId()] = assessment.score();
    }

    QMap<QString, int> pendingScores;
    const QList<AssessmentSubmission> pendingSubmissions =
        submissionRepo_.findPendingByEngineer(engineerId_, "core");
    for (const AssessmentSubmission& submission : pendingSubmissions) {
        pendingScores[submission.skillId] = submission.proposedScore;
    }

    Session* session = Application::instance().session();
    const QString submittedByUserId = session ? session->userId() : QString();
    const QString submittedByName = session ? session->username() : QString();

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

        const int approvedScore = existingScores.value(buttonGroup.skillId, 0);
        const int displayedScore = pendingScores.contains(buttonGroup.skillId)
            ? pendingScores.value(buttonGroup.skillId)
            : approvedScore;

        if (selectedScore == displayedScore) {
            continue;
        }

        Logger::instance().debug("MyCoreSkillsWidget",
            QString("Attempting to save: engineerId=%1, categoryId=%2, skillId=%3, score=%4")
                .arg(engineerId_)
                .arg(buttonGroup.categoryId)
                .arg(buttonGroup.skillId)
                .arg(selectedScore));

        if (submissionRepo_.submitCoreSkill(
                engineerId_, buttonGroup.categoryId, buttonGroup.skillId,
                selectedScore, submittedByUserId, submittedByName)) {
            submittedCount++;
        } else {
            errorCount++;
            Logger::instance().error("MyCoreSkillsWidget",
                QString("Failed to submit assessment for skill %1: %2")
                    .arg(buttonGroup.skillId)
                    .arg(submissionRepo_.lastError()));
        }
    }

    if (errorCount > 0) {
        QMessageBox::warning(this, "Partial Success",
            QString("Submitted %1 changes, but %2 failed.").arg(submittedCount).arg(errorCount));
    } else if (submittedCount == 0) {
        QMessageBox::information(this, "No Changes", "There are no new core-skill changes to submit.");
    } else {
        Logger::instance().info("MyCoreSkillsWidget", QString("Submitted %1 core skill changes").arg(submittedCount));
        QMessageBox::information(this, "Submitted for Approval",
            QString("Submitted %1 core-skill change%2 for manager approval.")
                .arg(submittedCount)
                .arg(submittedCount == 1 ? "" : "s"));
    }
    loadCoreSkills();
}

void MyCoreSkillsWidget::onRefreshClicked()
{
    loadCoreSkills();
    Logger::instance().info("MyCoreSkillsWidget", "Core skills refreshed");
}
