#include "AssessmentWidget.h"
#include "../utils/Logger.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QScrollArea>
#include <QMessageBox>
#include <QShowEvent>
#include <QtConcurrent/QtConcurrent>
#include <algorithm>

AssessmentWidget::AssessmentWidget(QWidget* parent)
    : QWidget(parent)
    , areaFilterCombo_(nullptr)
    , engineersLayout_(nullptr)
    , engineersContainer_(nullptr)
    , isFirstShow_(true)
    , loadTimer_(nullptr)
    , loadingLabel_(nullptr)
    , currentEngineerIndex_(0)
    , dataWatcher_(nullptr)
{
    setupUI();

    // Initialize background data loader
    dataWatcher_ = new QFutureWatcher<LoadedData>(this);
    connect(dataWatcher_, &QFutureWatcher<LoadedData>::finished, this, &AssessmentWidget::onDataLoaded);

    Logger::instance().info("AssessmentWidget", "Assessment widget initialized");
}

AssessmentWidget::~AssessmentWidget()
{
}

void AssessmentWidget::setupUI()
{
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(16);
    mainLayout->setContentsMargins(24, 24, 24, 24);

    // Title
    QLabel* titleLabel = new QLabel("Competency Assessment", this);
    QFont titleFont = titleLabel->font();
    titleFont.setPointSize(32);
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);
    mainLayout->addWidget(titleLabel);

    // Subtitle
    QLabel* subtitleLabel = new QLabel("Assess engineer competencies by production area", this);
    QFont subtitleFont = subtitleLabel->font();
    subtitleFont.setPointSize(14);
    subtitleLabel->setFont(subtitleFont);
    subtitleLabel->setStyleSheet("color: #64748b;");
    mainLayout->addWidget(subtitleLabel);
    mainLayout->addSpacing(16);

    // Filter bar
    QHBoxLayout* filterLayout = new QHBoxLayout();
    filterLayout->setSpacing(12);

    QLabel* filterLabel = new QLabel("Filter by Area:", this);
    QFont filterFont = filterLabel->font();
    filterFont.setPointSize(14);
    filterFont.setWeight(QFont::Medium);
    filterLabel->setFont(filterFont);
    filterLayout->addWidget(filterLabel);

    areaFilterCombo_ = new QComboBox(this);
    areaFilterCombo_->setMinimumWidth(250);
    areaFilterCombo_->addItem("All Areas", 0);

    // Load production areas
    QList<ProductionArea> areas = productionRepo_.findAllAreas();
    for (const ProductionArea& area : areas) {
        areaFilterCombo_->addItem(area.name(), area.id());
    }

    connect(areaFilterCombo_, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &AssessmentWidget::onAreaFilterChanged);

    filterLayout->addWidget(areaFilterCombo_);

    // Refresh button
    QPushButton* refreshButton = new QPushButton("Refresh", this);
    refreshButton->setMinimumWidth(100);
    connect(refreshButton, &QPushButton::clicked, this, &AssessmentWidget::onRefreshClicked);
    filterLayout->addWidget(refreshButton);

    filterLayout->addStretch();

    mainLayout->addLayout(filterLayout);
    mainLayout->addSpacing(8);

    // Scrollable engineer cards container
    QScrollArea* scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    engineersContainer_ = new QWidget();
    engineersLayout_ = new QVBoxLayout(engineersContainer_);
    engineersLayout_->setSpacing(16);
    engineersLayout_->setContentsMargins(0, 0, 0, 0);
    engineersLayout_->addStretch();

    scrollArea->setWidget(engineersContainer_);
    mainLayout->addWidget(scrollArea);

    // Add loading label (initially hidden)
    loadingLabel_ = new QLabel("Loading engineers...", this);
    QFont loadingFont = loadingLabel_->font();
    loadingFont.setPointSize(16);
    loadingFont.setBold(true);
    loadingLabel_->setFont(loadingFont);
    loadingLabel_->setAlignment(Qt::AlignCenter);
    loadingLabel_->setStyleSheet("color: #64748b; padding: 40px;");
    loadingLabel_->setVisible(false);
    engineersLayout_->insertWidget(0, loadingLabel_);

    setLayout(mainLayout);

    // Don't load data here - wait for showEvent() (lazy loading)
    // This makes tab switching instant
}

void AssessmentWidget::showEvent(QShowEvent* event)
{
    QWidget::showEvent(event);

    // Lazy loading: only load data on first show
    if (isFirstShow_) {
        isFirstShow_ = false;
        loadEngineerCards();
    }
}

void AssessmentWidget::loadEngineerCards()
{
    // Show loading label IMMEDIATELY (before any blocking work)
    loadingLabel_->setVisible(true);
    loadingLabel_->setText("Loading data from database...");

    // Clear existing cards
    QLayoutItem* item;
    while ((item = engineersLayout_->takeAt(0)) != nullptr) {
        if (item->widget() && item->widget() != loadingLabel_) {
            item->widget()->deleteLater();
        }
        delete item;
    }
    scoreButtonGroups_.clear();

    // Stop existing timer if any
    if (loadTimer_) {
        loadTimer_->stop();
        loadTimer_->deleteLater();
    }

    Logger::instance().info("AssessmentWidget", "Loading assessment data...");

    // Load all data from database (optimized with caching)
    cachedEngineers_ = engineerRepo_.findAll();
    QList<ProductionArea> allAreas = productionRepo_.findAllAreas();
    QList<Assessment> allAssessments = assessmentRepo_.findAll();

    // Build assessment lookup map for O(1) access
    cachedAssessmentScores_.clear();
    for (const Assessment& assessment : allAssessments) {
        QString key = QString("%1_%2_%3_%4")
            .arg(assessment.engineerId())
            .arg(assessment.productionAreaId())
            .arg(assessment.machineId())
            .arg(assessment.competencyId());
        cachedAssessmentScores_[key] = assessment.score();
    }

    // Pre-load all machines and competencies
    QMap<int, QString> areaNames;
    cachedAreaToMachines_.clear();
    for (const ProductionArea& area : allAreas) {
        areaNames[area.id()] = area.name();

        QList<Machine> machines = productionRepo_.findMachinesByArea(area.id());
        QList<MachineData> machineDataList;

        for (const Machine& machine : machines) {
            MachineData data;
            data.machine = machine;
            data.competencies = productionRepo_.findCompetenciesByMachine(machine.id());
            if (!data.competencies.isEmpty()) {
                machineDataList.append(data);
            }
        }

        if (!machineDataList.isEmpty()) {
            cachedAreaToMachines_[area.id()] = machineDataList;
        }
    }

    cachedAreaNames_ = areaNames;

    // Sort engineers by name
    std::sort(cachedEngineers_.begin(), cachedEngineers_.end(),
              [](const Engineer& a, const Engineer& b) {
                  return a.name() < b.name();
              });

    Logger::instance().info("AssessmentWidget",
        QString("Loaded %1 engineers from database. Creating UI...")
            .arg(cachedEngineers_.size()));

    // Create UI cards progressively
    currentEngineerIndex_ = 0;

    if (cachedEngineers_.isEmpty()) {
        loadingLabel_->setText("No engineers found");
        return;
    }

    // Create timer to load engineer cards in batches (5 at a time for smoother loading)
    loadTimer_ = new QTimer(this);
    connect(loadTimer_, &QTimer::timeout, this, &AssessmentWidget::loadNextEngineerCard);
    loadTimer_->start(1);  // Process as fast as possible
}

void AssessmentWidget::loadNextEngineerCard()
{
    if (currentEngineerIndex_ >= cachedEngineers_.size()) {
        // All cards loaded
        loadTimer_->stop();
        loadingLabel_->setVisible(false);
        Logger::instance().info("AssessmentWidget",
            QString("Loaded %1 engineer cards").arg(currentEngineerIndex_));
        return;
    }

    int filterAreaId = areaFilterCombo_->currentData().toInt();

    // Process 5 engineers per timer tick for smoother loading (batch processing)
    for (int batch = 0; batch < 5 && currentEngineerIndex_ < cachedEngineers_.size(); batch++) {
        // Update loading progress
        loadingLabel_->setText(QString("Loading engineers... %1/%2")
            .arg(currentEngineerIndex_ + 1)
            .arg(cachedEngineers_.size()));

        const Engineer& engineer = cachedEngineers_[currentEngineerIndex_++];

    // Create engineer card (same code as before, but for just one engineer)
    {
        // Create engineer card
        QGroupBox* engineerCard = new QGroupBox(this);
        engineerCard->setStyleSheet(
            "QGroupBox {"
            "    border: 2px solid #e2e8f0;"
            "    border-radius: 8px;"
            "    padding: 20px;"
            "    background-color: transparent;"
            "}"
        );

        QVBoxLayout* cardLayout = new QVBoxLayout(engineerCard);
        cardLayout->setSpacing(16);

        // Engineer header
        QHBoxLayout* headerLayout = new QHBoxLayout();

        QLabel* engineerName = new QLabel(engineer.name(), this);
        QFont nameFont = engineerName->font();
        nameFont.setPointSize(20);
        nameFont.setBold(true);
        engineerName->setFont(nameFont);
        headerLayout->addWidget(engineerName);

        headerLayout->addStretch();

        // Summary label
        QLabel* summaryLabel = new QLabel(this);
        QFont summaryFont = summaryLabel->font();
        summaryFont.setPointSize(14);
        summaryLabel->setFont(summaryFont);
        summaryLabel->setStyleSheet("color: #64748b;");
        headerLayout->addWidget(summaryLabel);

        cardLayout->addLayout(headerLayout);

        // Use cached data
        bool hasContent = false;
        int totalCompetencies = 0;
        int trainedCompetencies = 0;

        // Iterate through cached area-to-machines mapping
        for (auto it = cachedAreaToMachines_.constBegin(); it != cachedAreaToMachines_.constEnd(); ++it) {
            int areaId = it.key();

            // Skip if filtering and doesn't match
            if (filterAreaId != 0 && areaId != filterAreaId) {
                continue;
            }

            const QList<MachineData>& machines = it.value();

            // Get area name from cache (O(1) lookup, no database query)
            QString areaName = cachedAreaNames_.value(areaId, "Unknown Area");

            // Area header
            QLabel* areaLabel = new QLabel(areaName, this);
            QFont areaFont = areaLabel->font();
            areaFont.setPointSize(16);
            areaFont.setBold(true);
            areaLabel->setFont(areaFont);
            areaLabel->setStyleSheet("color: #334155; margin-top: 8px;");
            cardLayout->addWidget(areaLabel);

            for (const MachineData& machineData : machines) {
                hasContent = true;

                // Machine header
                QLabel* machineLabel = new QLabel("  " + machineData.machine.name(), this);
                QFont machineFont = machineLabel->font();
                machineFont.setPointSize(14);
                machineFont.setWeight(QFont::Medium);
                machineLabel->setFont(machineFont);
                machineLabel->setStyleSheet("color: #475569; margin-left: 16px;");
                cardLayout->addWidget(machineLabel);

                // Competencies grid
                for (const Competency& competency : machineData.competencies) {
                    totalCompetencies++;

                    QHBoxLayout* compLayout = new QHBoxLayout();
                    compLayout->setSpacing(12);
                    compLayout->setContentsMargins(32, 4, 0, 4);

                    // Competency name
                    QLabel* compLabel = new QLabel(competency.name(), this);
                    QFont compFont = compLabel->font();
                    compFont.setPointSize(13);
                    compLabel->setFont(compFont);
                    compLabel->setWordWrap(true);
                    compLabel->setMinimumWidth(250);
                    compLabel->setMaximumWidth(500);
                    compLayout->addWidget(compLabel, 1);

                    compLayout->addStretch();

                    // Get current score from cached data
                    QString key = QString("%1_%2_%3_%4")
                        .arg(engineer.id())
                        .arg(areaId)
                        .arg(machineData.machine.id())
                        .arg(competency.id());
                    int currentScore = cachedAssessmentScores_.value(key, -1);

                    if (currentScore > 0) {
                        trainedCompetencies++;
                    }

                    // Create score buttons (0-3)
                    createScoreButtons(compLayout, engineer.id(), areaId, machineData.machine.id(),
                                     competency.id(), currentScore);

                    cardLayout->addLayout(compLayout);
                }
            }
        }

        // Only add card if it has content
        if (hasContent) {
            // Update summary using already calculated values
            QString summaryText = QString("%1/%2 competencies trained")
                                     .arg(trainedCompetencies)
                                     .arg(totalCompetencies);
            summaryLabel->setText(summaryText);
            engineersLayout_->insertWidget(engineersLayout_->count() - 1, engineerCard);
        } else {
            delete engineerCard;
        }
    }
    }  // end of batch loop
}

void AssessmentWidget::createScoreButtons(QHBoxLayout* layout, const QString& engineerId,
                                         int areaId, int machineId, int competencyId,
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
    buttonGroup.competencyId = QString::number(competencyId);

    for (int score = 0; score < 4; score++) {
        QPushButton* button = new QPushButton(scoreInfos[score].label, this);
        button->setFixedSize(32, 32);
        button->setCursor(Qt::PointingHandCursor);

        // Store metadata
        button->setProperty("engineerId", engineerId);
        button->setProperty("areaId", areaId);
        button->setProperty("machineId", machineId);
        button->setProperty("competencyId", competencyId);
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

        connect(button, &QPushButton::clicked, this, &AssessmentWidget::onScoreButtonClicked);

        layout->addWidget(button);
        buttonGroup.buttons[score] = button;
    }

    scoreButtonGroups_.append(buttonGroup);
}

void AssessmentWidget::onScoreButtonClicked()
{
    QPushButton* clickedButton = qobject_cast<QPushButton*>(sender());
    if (!clickedButton) {
        return;
    }

    QString engineerId = clickedButton->property("engineerId").toString();
    int areaId = clickedButton->property("areaId").toInt();
    int machineId = clickedButton->property("machineId").toInt();
    int competencyId = clickedButton->property("competencyId").toInt();
    int score = clickedButton->property("score").toInt();

    // Save assessment to database
    Assessment assessment(0, engineerId, areaId, machineId, competencyId, score);

    if (assessmentRepo_.saveOrUpdate(assessment)) {
        Logger::instance().info("AssessmentWidget",
            QString("Saved score %1 for engineer %2, competency %3")
                .arg(score).arg(engineerId).arg(competencyId));

        // Update button styles in this group
        // Find all buttons for this competency
        for (int i = 0; i < 4; i++) {
            // Find the sibling buttons
            QWidget* parent = clickedButton->parentWidget();
            if (!parent) continue;

            QHBoxLayout* layout = qobject_cast<QHBoxLayout*>(parent->layout());
            if (!layout) continue;

            // Iterate through buttons in this layout
            for (int j = 0; j < layout->count(); j++) {
                QLayoutItem* item = layout->itemAt(j);
                if (!item || !item->widget()) continue;

                QPushButton* btn = qobject_cast<QPushButton*>(item->widget());
                if (!btn) continue;

                // Check if same competency
                if (btn->property("engineerId").toString() == engineerId &&
                    btn->property("competencyId").toInt() == competencyId) {

                    int btnScore = btn->property("score").toInt();
                    bool isSelected = (btnScore == score);

                    // Score colors
                    QStringList colors = {"#ff6b6b", "#fbbf24", "#60a5fa", "#4ade80"};
                    QString color = colors[btnScore];

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
                        ).arg(color);
                    } else {
                        buttonStyle = QString(
                            "QPushButton {"
                            "    background-color: white;"
                            "    color: #64748b;"
                            "    border: 2px solid #e2e8f0;"
                            "    border-radius: 16px;"
                            "    font-size: 12px;"
                            "}"
                            "QPushButton:hover {"
                            "    border-color: %1;"
                            "    color: %1;"
                            "}"
                        ).arg(color);
                    }

                    btn->setStyleSheet(buttonStyle);
                }
            }
        }

        // Update engineer summary
        // Find the summary label for this engineer
        for (int i = 0; i < engineersLayout_->count(); i++) {
            QLayoutItem* item = engineersLayout_->itemAt(i);
            if (!item || !item->widget()) continue;

            QGroupBox* card = qobject_cast<QGroupBox*>(item->widget());
            if (!card) continue;

            QVBoxLayout* cardLayout = qobject_cast<QVBoxLayout*>(card->layout());
            if (!cardLayout || cardLayout->count() == 0) continue;

            QLayoutItem* headerItem = cardLayout->itemAt(0);
            if (!headerItem) continue;

            QHBoxLayout* headerLayout = qobject_cast<QHBoxLayout*>(headerItem->layout());
            if (!headerLayout || headerLayout->count() < 3) continue;

            QLayoutItem* summaryItem = headerLayout->itemAt(2);
            if (!summaryItem || !summaryItem->widget()) continue;

            QLabel* summaryLabel = qobject_cast<QLabel*>(summaryItem->widget());
            if (summaryLabel) {
                updateEngineerSummary(engineerId, summaryLabel);
                break;
            }
        }

    } else {
        Logger::instance().error("AssessmentWidget",
            QString("Failed to save assessment: %1").arg(assessmentRepo_.lastError()));
        QMessageBox::warning(this, "Error",
            "Failed to save assessment. Please try again.");
    }
}

void AssessmentWidget::updateEngineerSummary(const QString& engineerId, QLabel* summaryLabel)
{
    // OPTIMIZATION: Use cached data instead of repeated database queries
    int filterAreaId = areaFilterCombo_->currentData().toInt();

    // Load assessment scores once
    QList<Assessment> assessments = assessmentRepo_.findByEngineer(engineerId);
    QMap<QString, int> assessmentScores;
    for (const Assessment& assessment : assessments) {
        QString key = QString("%1_%2_%3")
            .arg(assessment.productionAreaId())
            .arg(assessment.machineId())
            .arg(assessment.competencyId());
        assessmentScores[key] = assessment.score();
    }

    int totalCompetencies = 0;
    int trainedCompetencies = 0;

    QList<ProductionArea> areas = productionRepo_.findAllAreas();
    for (const ProductionArea& area : areas) {
        if (filterAreaId != 0 && area.id() != filterAreaId) {
            continue;
        }

        QList<Machine> machines = productionRepo_.findMachinesByArea(area.id());
        for (const Machine& machine : machines) {
            QList<Competency> competencies = productionRepo_.findCompetenciesByMachine(machine.id());

            for (const Competency& competency : competencies) {
                totalCompetencies++;

                QString key = QString("%1_%2_%3")
                    .arg(area.id())
                    .arg(machine.id())
                    .arg(competency.id());

                if (assessmentScores.value(key, 0) > 0) {
                    trainedCompetencies++;
                }
            }
        }
    }

    QString summaryText = QString("%1/%2 competencies trained")
                             .arg(trainedCompetencies)
                             .arg(totalCompetencies);
    summaryLabel->setText(summaryText);
}

void AssessmentWidget::onAreaFilterChanged(int index)
{
    Q_UNUSED(index);
    loadEngineerCards();
}

void AssessmentWidget::onDataLoaded()
{
    // Get the loaded data from the background thread
    LoadedData data = dataWatcher_->result();

    Logger::instance().info("AssessmentWidget",
        QString("Background load complete. Processing %1 engineers...").arg(data.engineers.size()));

    // Store loaded data in member variables
    cachedEngineers_ = data.engineers;
    cachedAreaToMachines_ = data.areaToMachines;

    // Build assessment lookup map for O(1) access
    cachedAssessmentScores_.clear();
    for (const Assessment& assessment : data.assessments) {
        QString key = QString("%1_%2_%3_%4")
            .arg(assessment.engineerId())
            .arg(assessment.productionAreaId())
            .arg(assessment.machineId())
            .arg(assessment.competencyId());
        cachedAssessmentScores_[key] = assessment.score();
    }

    // Build area ID to name lookup map
    QMap<int, QString> areaNames;
    for (const ProductionArea& area : data.areas) {
        areaNames[area.id()] = area.name();
    }
    cachedAreaNames_ = areaNames;

    // Check if we have engineers to display
    if (cachedEngineers_.isEmpty()) {
        loadingLabel_->setText("No engineers found");
        return;
    }

    // Update loading label
    loadingLabel_->setText(QString("Creating UI for %1 engineers...").arg(cachedEngineers_.size()));

    // Create UI cards progressively (one every 10ms to avoid UI freeze)
    currentEngineerIndex_ = 0;
    loadTimer_ = new QTimer(this);
    connect(loadTimer_, &QTimer::timeout, this, &AssessmentWidget::loadNextEngineerCard);
    loadTimer_->start(10);
}

void AssessmentWidget::onRefreshClicked()
{
    loadEngineerCards();
    Logger::instance().info("AssessmentWidget", "Refreshed assessment data");
}
