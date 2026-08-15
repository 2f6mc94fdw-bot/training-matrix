#include "CoreSkillsWidget.h"
#include "../core/Application.h"
#include "../utils/Logger.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QMessageBox>
#include <QLabel>
#include <QGroupBox>
#include <QScrollArea>
#include <QShowEvent>
#include <QGraphicsDropShadowEffect>

namespace {
QString scoreButtonStyle(int score, bool isSelected)
{
    const QStringList scoreColors = {"#ff6b6b", "#fbbf24", "#60a5fa", "#4ade80"};
    const QString color = scoreColors.value(score, "#60a5fa");

    if (isSelected) {
        return QString(
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
        ).arg(color);
    }

    return QString(
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
    ).arg(color);
}

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

CoreSkillsWidget::CoreSkillsWidget(QWidget* parent)
    : QWidget(parent)
    , engineerCombo_(nullptr)
    , skillsLayout_(nullptr)
    , skillsContainer_(nullptr)
    , saveButton_(nullptr)
    , refreshButton_(nullptr)
{
    setupUI();
    loadEngineers();
    // Don't load core skills here - wait for showEvent() (lazy loading like AssessmentWidget)
    Logger::instance().info("CoreSkillsWidget", "Core Skills widget initialized");
}

CoreSkillsWidget::~CoreSkillsWidget()
{
}

void CoreSkillsWidget::showEvent(QShowEvent* event)
{
    QWidget::showEvent(event);

    // Lazy loading: only load data on first show (like AssessmentWidget)
    if (isFirstShow_) {
        isFirstShow_ = false;
        Logger::instance().info("CoreSkillsWidget", "First show - loading core skills");
        loadCoreSkills();
    }
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

    // Description
    QLabel* descLabel = new QLabel("Core Skills (0 = No skill, 1 = Basic, 2 = Intermediate, 3 = Advanced)", this);
    mainLayout->addWidget(descLabel);

    // Scrollable skills container (like AssessmentWidget)
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
    // Clear existing widgets
    QLayoutItem* item;
    while ((item = skillsLayout_->takeAt(0)) != nullptr) {
        if (item->widget()) {
            item->widget()->deleteLater();
        }
        delete item;
    }
    scoreButtonGroups_.clear();

    QList<CoreSkillCategory> categories = coreSkillsRepo_.findAllCategories();
    QList<CoreSkill> skills = coreSkillsRepo_.findAllSkills();
    QMap<QString, QList<CoreSkill>> skillsByCategory;
    for (const CoreSkill& skill : skills) {
        skillsByCategory[skill.categoryId()].append(skill);
    }

    QString engineerId = engineerCombo_->currentData().toString();

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

                // Create score buttons (0-3)
                createScoreButtons(skillLayout, engineerId, category.id(), skill.id(), 0);

                cardLayout->addLayout(skillLayout);
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

    Logger::instance().info("CoreSkillsWidget", QString("Loaded %1 categories").arg(categories.size()));
}

void CoreSkillsWidget::loadAssessments()
{
    QString engineerId = engineerCombo_->currentData().toString();

    if (engineerId.isEmpty()) {
        return;
    }

    const QList<CoreSkillAssessment> assessments = coreSkillsRepo_.findAssessmentsByEngineer(engineerId);
    QMap<QString, CoreSkillAssessment> assessmentBySkill;
    for (const CoreSkillAssessment& assessment : assessments) {
        assessmentBySkill.insert(assessment.categoryId() + "|" + assessment.skillId(), assessment);
    }

    // Update button groups with existing scores
    for (ScoreButtonGroup& buttonGroup : scoreButtonGroups_) {
        if (buttonGroup.engineerId != engineerId) {
            continue;
        }

        const QString key = buttonGroup.categoryId + "|" + buttonGroup.skillId;
        const bool hasAssessment = assessmentBySkill.contains(key);
        const int currentScore = hasAssessment ? assessmentBySkill.value(key).score() : 0;
        buttonGroup.hasInitialScore = hasAssessment;
        buttonGroup.initialScore = currentScore;

        // Update button styles
        for (int score = 0; score < 4; score++) {
            QPushButton* button = buttonGroup.buttons[score];
            bool isSelected = (score == currentScore);

            // Set the property for save detection
            button->setProperty("isSelected", isSelected);

            button->setStyleSheet(scoreButtonStyle(score, isSelected));
        }
    }

    Logger::instance().info("CoreSkillsWidget", "Loaded assessments for engineer: " + engineerId);
}

void CoreSkillsWidget::onEngineerChanged(int index)
{
    QString newEngineerId = engineerCombo_->currentData().toString();

    // Update all button groups with new engineer ID and reset to score 0
    for (ScoreButtonGroup& buttonGroup : scoreButtonGroups_) {
        buttonGroup.engineerId = newEngineerId;
        buttonGroup.hasInitialScore = false;
        buttonGroup.initialScore = 0;

        // Reset all buttons to unselected state, with button 0 selected
        for (int score = 0; score < 4; score++) {
            QPushButton* button = buttonGroup.buttons[score];
            button->setProperty("engineerId", newEngineerId);

            bool isSelected = (score == 0);  // Default to score 0
            button->setProperty("isSelected", isSelected);  // Set property for save detection

            button->setStyleSheet(scoreButtonStyle(score, isSelected));
        }
    }

    // If an engineer is selected, load their assessments
    if (index > 0) {
        loadAssessments();
    }
}

void CoreSkillsWidget::onSaveClicked()
{
    const QString engineerId = engineerCombo_->currentData().toString();

    if (engineerId.isEmpty()) {
        QMessageBox::warning(this, "Selection Required", "Please select an engineer.");
        return;
    }

    QList<OfficialCoreScoreChange> changes;
    for (const ScoreButtonGroup& buttonGroup : scoreButtonGroups_) {
        if (buttonGroup.engineerId != engineerId) {
            continue;
        }

        int selectedScore = 0;
        for (int score = 0; score < 4; score++) {
            const QPushButton* button = buttonGroup.buttons[score];
            if (button->property("isSelected").toBool()) {
                selectedScore = score;
                break;
            }
        }

        if (selectedScore != buttonGroup.initialScore) {
            OfficialCoreScoreChange change;
            change.categoryId = buttonGroup.categoryId;
            change.skillId = buttonGroup.skillId;
            change.expectedHasScore = buttonGroup.hasInitialScore;
            change.expectedScore = buttonGroup.initialScore;
            change.newScore = selectedScore;
            changes.append(change);
        }
    }

    if (changes.isEmpty()) {
        QMessageBox::information(this, "No Changes", "No core-skill scores have changed.");
        return;
    }

    Session* session = Application::instance().session();
    if (!session || !session->isAdmin()) {
        QMessageBox::warning(this, "Permission Denied",
            "An authenticated manager is required to update official core-skill scores.");
        loadAssessments();
        return;
    }
    if (!assessmentWorkflowRepo_.saveOfficialCoreSkills(
            engineerId,
            changes,
            session ? session->userId() : QString(),
            session ? session->username() : QString())) {
        Logger::instance().error("CoreSkillsWidget",
            "Failed to save core-skill assessments: " + assessmentWorkflowRepo_.lastError());
        QMessageBox::warning(this, "Save Failed",
            assessmentWorkflowRepo_.lastError() + "\n\nNo core-skill changes were saved.");
        loadAssessments();
        return;
    }

    for (const OfficialCoreScoreChange& change : changes) {
        for (ScoreButtonGroup& buttonGroup : scoreButtonGroups_) {
            if (buttonGroup.categoryId == change.categoryId && buttonGroup.skillId == change.skillId) {
                buttonGroup.hasInitialScore = true;
                buttonGroup.initialScore = change.newScore;
                break;
            }
        }
    }

    Logger::instance().info("CoreSkillsWidget",
        QString("Saved %1 core-skill assessment changes").arg(changes.size()));
    QMessageBox::information(this, "Assessments Updated",
        QString("Saved %1 core-skill score change%2 with an audit record.")
            .arg(changes.size())
            .arg(changes.size() == 1 ? QString() : "s"));
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
    buttonGroup.initialScore = currentScore;

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
        button->setProperty("engineerId", engineerId);
        button->setProperty("categoryId", categoryId);
        button->setProperty("skillId", skillId);
        button->setProperty("score", score);

        // Style button based on whether it's selected
        bool isSelected = (score == currentScore);
        button->setProperty("isSelected", isSelected);  // Store for save detection

        button->setStyleSheet(scoreButtonStyle(score, isSelected));

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

    // Find the button group and update all buttons in that group
    for (ScoreButtonGroup& buttonGroup : scoreButtonGroups_) {
        if (buttonGroup.engineerId == engineerId &&
            buttonGroup.categoryId == categoryId &&
            buttonGroup.skillId == skillId) {

            // Update styles for all buttons in this group
            for (int i = 0; i < 4; i++) {
                QPushButton* button = buttonGroup.buttons[i];
                bool isSelected = (i == score);

                // Store selected state as property for reliable save detection
                button->setProperty("isSelected", isSelected);

                button->setStyleSheet(scoreButtonStyle(i, isSelected));
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
