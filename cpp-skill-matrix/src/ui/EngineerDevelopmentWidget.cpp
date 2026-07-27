#include "EngineerDevelopmentWidget.h"
#include "../core/Application.h"
#include "../core/DataCache.h"
#include "../core/Session.h"
#include "../models/Engineer.h"
#include "../models/ProductionArea.h"
#include "../models/Machine.h"
#include "../models/Competency.h"
#include "../utils/Logger.h"

#include <QAbstractItemView>
#include <QBarCategoryAxis>
#include <QBarSeries>
#include <QBarSet>
#include <QChart>
#include <QChartView>
#include <QComboBox>
#include <QDateEdit>
#include <QFrame>
#include <QFileDialog>
#include <QGroupBox>
#include <QHeaderView>
#include <QHBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QPageSize>
#include <QPainter>
#include <QPrinter>
#include <QPushButton>
#include <QScrollArea>
#include <QTableWidget>
#include <QTextDocument>
#include <QTextEdit>
#include <QValueAxis>
#include <QVBoxLayout>
#include <algorithm>

namespace {
QLabel* makeMetricLabel(QWidget* parent)
{
    QLabel* label = new QLabel("-", parent);
    QFont font = label->font();
    font.setPointSize(24);
    font.setBold(true);
    label->setFont(font);
    label->setStyleSheet("color:#0f172a;");
    return label;
}

QWidget* makeMetricCard(const QString& title, QLabel* value, const QString& accent, QWidget* parent)
{
    QGroupBox* card = new QGroupBox(parent);
    card->setStyleSheet(QString(
        "QGroupBox{background:white;border:1px solid #dbe4ef;border-left:5px solid %1;"
        "border-radius:10px;padding:14px;}").arg(accent));
    QVBoxLayout* layout = new QVBoxLayout(card);
    QLabel* heading = new QLabel(title, card);
    heading->setStyleSheet("color:#64748b;font-weight:600;");
    layout->addWidget(heading);
    layout->addWidget(value);
    return card;
}
}

EngineerDevelopmentWidget::EngineerDevelopmentWidget(QWidget* parent)
    : QWidget(parent)
    , engineerCombo_(nullptr)
    , areaCombo_(nullptr)
    , machineCombo_(nullptr)
    , targetScoreCombo_(nullptr)
    , dueDateEdit_(nullptr)
    , averageScoreLabel_(nullptr)
    , proficiencyLabel_(nullptr)
    , gapsLabel_(nullptr)
    , activePlansLabel_(nullptr)
    , profileChartView_(nullptr)
    , gapTable_(nullptr)
    , planTable_(nullptr)
    , managerNotesEdit_(nullptr)
    , reportPreview_(nullptr)
    , exportPdfButton_(nullptr)
{
    setupUI();
    refresh();
}

void EngineerDevelopmentWidget::setupUI()
{
    QVBoxLayout* outer = new QVBoxLayout(this);
    outer->setContentsMargins(0, 0, 0, 0);
    QScrollArea* scroll = new QScrollArea(this);
    scroll->setWidgetResizable(true);
    scroll->setFrameShape(QFrame::NoFrame);
    QWidget* content = new QWidget(scroll);
    QVBoxLayout* layout = new QVBoxLayout(content);
    layout->setSpacing(16);

    QGroupBox* controls = new QGroupBox("Engineer and focus scope", content);
    QHBoxLayout* controlsLayout = new QHBoxLayout(controls);
    engineerCombo_ = new QComboBox(controls);
    areaCombo_ = new QComboBox(controls);
    machineCombo_ = new QComboBox(controls);
    engineerCombo_->setMinimumWidth(220);
    areaCombo_->setMinimumWidth(200);
    machineCombo_->setMinimumWidth(220);
    controlsLayout->addWidget(new QLabel("Engineer", controls));
    controlsLayout->addWidget(engineerCombo_);
    controlsLayout->addWidget(new QLabel("Production area", controls));
    controlsLayout->addWidget(areaCombo_);
    controlsLayout->addWidget(new QLabel("Machine", controls));
    controlsLayout->addWidget(machineCombo_);
    controlsLayout->addStretch();
    layout->addWidget(controls);

    QHBoxLayout* metrics = new QHBoxLayout();
    averageScoreLabel_ = makeMetricLabel(content);
    proficiencyLabel_ = makeMetricLabel(content);
    gapsLabel_ = makeMetricLabel(content);
    activePlansLabel_ = makeMetricLabel(content);
    metrics->addWidget(makeMetricCard("Weighted average", averageScoreLabel_, "#2563eb", content));
    metrics->addWidget(makeMetricCard("Competencies at 2+", proficiencyLabel_, "#10b981", content));
    metrics->addWidget(makeMetricCard("Current gaps", gapsLabel_, "#f59e0b", content));
    metrics->addWidget(makeMetricCard("Active plan items", activePlansLabel_, "#ef4444", content));
    layout->addLayout(metrics);

    QGroupBox* chartGroup = new QGroupBox("Production area profile", content);
    QVBoxLayout* chartLayout = new QVBoxLayout(chartGroup);
    profileChartView_ = new QChartView(chartGroup);
    profileChartView_->setRenderHint(QPainter::Antialiasing);
    profileChartView_->setMinimumHeight(300);
    chartLayout->addWidget(profileChartView_);
    layout->addWidget(chartGroup);

    QGroupBox* gapsGroup = new QGroupBox("Development opportunities in selected scope", content);
    QVBoxLayout* gapsLayout = new QVBoxLayout(gapsGroup);
    gapTable_ = new QTableWidget(gapsGroup);
    gapTable_->setColumnCount(5);
    gapTable_->setHorizontalHeaderLabels({"Production Area", "Machine", "Competency", "Current", "Suggested Target"});
    gapTable_->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);
    gapTable_->setSelectionBehavior(QAbstractItemView::SelectRows);
    gapTable_->setSelectionMode(QAbstractItemView::ExtendedSelection);
    gapTable_->setEditTriggers(QAbstractItemView::NoEditTriggers);
    gapTable_->setMinimumHeight(280);
    gapsLayout->addWidget(gapTable_);

    QHBoxLayout* planControls = new QHBoxLayout();
    targetScoreCombo_ = new QComboBox(gapsGroup);
    targetScoreCombo_->addItem("2 - Trained", 2);
    targetScoreCombo_->addItem("3 - Able to train others", 3);
    dueDateEdit_ = new QDateEdit(QDate::currentDate().addMonths(3), gapsGroup);
    dueDateEdit_->setCalendarPopup(true);
    dueDateEdit_->setDisplayFormat("dd MMM yyyy");
    managerNotesEdit_ = new QTextEdit(gapsGroup);
    managerNotesEdit_->setPlaceholderText("Manager guidance or agreed learning activity...");
    managerNotesEdit_->setMaximumHeight(70);
    QPushButton* addButton = new QPushButton("Add selected gaps to plan", gapsGroup);
    addButton->setStyleSheet("QPushButton{background:#0b5ed7;color:white;border:0;border-radius:8px;padding:10px 16px;font-weight:600;} QPushButton:hover{background:#084db4;}");
    planControls->addWidget(new QLabel("Target", gapsGroup));
    planControls->addWidget(targetScoreCombo_);
    planControls->addWidget(new QLabel("Due", gapsGroup));
    planControls->addWidget(dueDateEdit_);
    planControls->addWidget(managerNotesEdit_, 1);
    planControls->addWidget(addButton);
    gapsLayout->addLayout(planControls);
    layout->addWidget(gapsGroup);

    QGroupBox* planGroup = new QGroupBox("Saved development plan", content);
    QVBoxLayout* savedLayout = new QVBoxLayout(planGroup);
    planTable_ = new QTableWidget(planGroup);
    planTable_->setColumnCount(7);
    planTable_->setHorizontalHeaderLabels({"Area", "Machine", "Competency", "Current", "Target", "Due", "Status"});
    planTable_->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);
    planTable_->setSelectionBehavior(QAbstractItemView::SelectRows);
    planTable_->setSelectionMode(QAbstractItemView::SingleSelection);
    planTable_->setEditTriggers(QAbstractItemView::NoEditTriggers);
    planTable_->setMinimumHeight(220);
    savedLayout->addWidget(planTable_);
    QHBoxLayout* savedButtons = new QHBoxLayout();
    QPushButton* completeButton = new QPushButton("Mark complete", planGroup);
    QPushButton* cancelButton = new QPushButton("Cancel item", planGroup);
    savedButtons->addStretch();
    savedButtons->addWidget(completeButton);
    savedButtons->addWidget(cancelButton);
    savedLayout->addLayout(savedButtons);
    layout->addWidget(planGroup);

    QGroupBox* reportGroup = new QGroupBox("Engineer focus report", content);
    QVBoxLayout* reportLayout = new QVBoxLayout(reportGroup);
    QLabel* reportHelp = new QLabel("Generate a practical report for the selected production area or machine. It combines current gaps with saved plan actions.", reportGroup);
    reportHelp->setWordWrap(true);
    reportHelp->setStyleSheet("color:#64748b;");
    reportPreview_ = new QTextEdit(reportGroup);
    reportPreview_->setReadOnly(true);
    reportPreview_->setMinimumHeight(280);
    reportPreview_->setPlaceholderText("Choose an engineer and scope, then generate the report.");
    QHBoxLayout* reportButtons = new QHBoxLayout();
    QPushButton* generateButton = new QPushButton("Generate focus report", reportGroup);
    exportPdfButton_ = new QPushButton("Export PDF", reportGroup);
    exportPdfButton_->setEnabled(false);
    reportButtons->addStretch();
    reportButtons->addWidget(generateButton);
    reportButtons->addWidget(exportPdfButton_);
    reportLayout->addWidget(reportHelp);
    reportLayout->addWidget(reportPreview_);
    reportLayout->addLayout(reportButtons);
    layout->addWidget(reportGroup);

    scroll->setWidget(content);
    outer->addWidget(scroll);

    connect(engineerCombo_, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &EngineerDevelopmentWidget::onEngineerChanged);
    connect(areaCombo_, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &EngineerDevelopmentWidget::onAreaChanged);
    connect(machineCombo_, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &EngineerDevelopmentWidget::onScopeChanged);
    connect(addButton, &QPushButton::clicked, this, &EngineerDevelopmentWidget::addSelectedGapsToPlan);
    connect(completeButton, &QPushButton::clicked, this, &EngineerDevelopmentWidget::markSelectedPlanComplete);
    connect(cancelButton, &QPushButton::clicked, this, &EngineerDevelopmentWidget::cancelSelectedPlanItem);
    connect(generateButton, &QPushButton::clicked, this, &EngineerDevelopmentWidget::generateReport);
    connect(exportPdfButton_, &QPushButton::clicked, this, &EngineerDevelopmentWidget::exportReportPdf);
}

void EngineerDevelopmentWidget::refresh()
{
    DataCache& cache = DataCache::instance();
    if (!cache.isLoaded()) {
        cache.load();
    }
    loadEngineers();
    loadAreaFilter();
    loadMachineFilter();
    loadEngineerDashboard();
}

void EngineerDevelopmentWidget::loadEngineers()
{
    const QString selected = currentEngineerId();
    engineerCombo_->blockSignals(true);
    engineerCombo_->clear();
    QList<Engineer> engineers = engineerRepo_.findAll();
    std::sort(engineers.begin(), engineers.end(), [](const Engineer& a, const Engineer& b) { return a.name() < b.name(); });
    for (const Engineer& engineer : engineers) {
        engineerCombo_->addItem(engineer.name(), engineer.id());
    }
    int restore = engineerCombo_->findData(selected);
    if (restore >= 0) engineerCombo_->setCurrentIndex(restore);
    engineerCombo_->blockSignals(false);
}

void EngineerDevelopmentWidget::loadAreaFilter()
{
    const int selected = areaCombo_->currentData().toInt();
    areaCombo_->blockSignals(true);
    areaCombo_->clear();
    areaCombo_->addItem("All production areas", 0);
    for (const ProductionArea& area : DataCache::instance().getAreas()) {
        areaCombo_->addItem(area.name(), area.id());
    }
    int restore = areaCombo_->findData(selected);
    if (restore >= 0) areaCombo_->setCurrentIndex(restore);
    areaCombo_->blockSignals(false);
}

void EngineerDevelopmentWidget::loadMachineFilter()
{
    const int areaId = areaCombo_->currentData().toInt();
    const int selected = machineCombo_->currentData().toInt();
    machineCombo_->blockSignals(true);
    machineCombo_->clear();
    machineCombo_->addItem("All machines", 0);
    for (const ProductionArea& area : DataCache::instance().getAreas()) {
        if (areaId > 0 && area.id() != areaId) continue;
        for (const Machine& machine : DataCache::instance().getMachinesByArea(area.id())) {
            machineCombo_->addItem(machine.name(), machine.id());
        }
    }
    int restore = machineCombo_->findData(selected);
    if (restore >= 0) machineCombo_->setCurrentIndex(restore);
    machineCombo_->blockSignals(false);
}

QString EngineerDevelopmentWidget::currentEngineerId() const
{
    return engineerCombo_ ? engineerCombo_->currentData().toString() : QString();
}

void EngineerDevelopmentWidget::onEngineerChanged() { loadEngineerDashboard(); }
void EngineerDevelopmentWidget::onAreaChanged() { loadMachineFilter(); loadEngineerDashboard(); }
void EngineerDevelopmentWidget::onScopeChanged() { loadEngineerDashboard(); }

void EngineerDevelopmentWidget::loadEngineerDashboard()
{
    const QString engineerId = currentEngineerId();
    if (engineerId.isEmpty()) return;
    engineerAssessments_ = assessmentRepo_.findByEngineer(engineerId);
    updateProfileChart();
    updateGapTable();
    updatePlanTable();

    QHash<int, int> scores;
    for (const Assessment& assessment : engineerAssessments_) scores[assessment.competencyId()] = assessment.score();
    double weighted = 0.0;
    double weights = 0.0;
    int total = 0;
    int proficient = 0;
    int gaps = 0;
    for (const ProductionArea& area : DataCache::instance().getAreas()) {
        for (const Machine& machine : DataCache::instance().getMachinesByArea(area.id())) {
            for (const Competency& competency : DataCache::instance().getCompetenciesByMachine(machine.id())) {
                const int score = scores.value(competency.id(), 0);
                const double weight = competency.calculatedWeight();
                weighted += score * weight;
                weights += weight;
                total++;
                if (score >= 2) proficient++;
                else gaps++;
            }
        }
    }
    averageScoreLabel_->setText(QString::number(weights > 0 ? weighted / weights : 0.0, 'f', 2) + " / 3");
    proficiencyLabel_->setText(QString("%1 / %2").arg(proficient).arg(total));
    gapsLabel_->setText(QString::number(gaps));
    activePlansLabel_->setText(QString::number(planRepo_.findByEngineer(engineerId, "active").size()));
}

void EngineerDevelopmentWidget::updateProfileChart()
{
    const int selectedAreaId = areaCombo_->currentData().toInt();
    const int selectedMachineId = machineCombo_->currentData().toInt();
    QHash<int, int> scores;
    for (const Assessment& assessment : engineerAssessments_) scores[assessment.competencyId()] = assessment.score();

    QBarSet* set = new QBarSet("Weighted average");
    set->setColor(QColor("#2563eb"));
    QStringList categories;

    QString chartTitle = "Knowledge by Production Area";
    for (const ProductionArea& area : DataCache::instance().getAreas()) {
        if (selectedAreaId > 0 && area.id() != selectedAreaId) continue;

        const QList<Machine> machines = DataCache::instance().getMachinesByArea(area.id());
        if (selectedAreaId > 0) {
            chartTitle = QString("%1 - Knowledge by Machine").arg(area.name());
        }

        if (selectedAreaId == 0) {
            double areaSum = 0.0;
            double areaWeights = 0.0;
            for (const Machine& machine : machines) {
                for (const Competency& competency : DataCache::instance().getCompetenciesByMachine(machine.id())) {
                    const double weight = competency.calculatedWeight();
                    areaSum += scores.value(competency.id(), 0) * weight;
                    areaWeights += weight;
                }
            }
            if (areaWeights > 0.0) {
                *set << areaSum / areaWeights;
                categories << area.name();
            }
            continue;
        }

        for (const Machine& machine : machines) {
            if (selectedMachineId > 0 && machine.id() != selectedMachineId) continue;
            double machineSum = 0.0;
            double machineWeights = 0.0;
            for (const Competency& competency : DataCache::instance().getCompetenciesByMachine(machine.id())) {
                const double weight = competency.calculatedWeight();
                machineSum += scores.value(competency.id(), 0) * weight;
                machineWeights += weight;
            }
            if (machineWeights > 0.0) {
                *set << machineSum / machineWeights;
                categories << machine.name();
            }
        }
    }

    QBarSeries* series = new QBarSeries();
    series->append(set);
    QChart* chart = new QChart();
    chart->addSeries(series);
    chart->setTitle(chartTitle);
    chart->setAnimationOptions(QChart::NoAnimation);
    QBarCategoryAxis* x = new QBarCategoryAxis();
    x->append(categories);
    if (categories.size() > 5) {
        x->setLabelsAngle(-30);
    }
    chart->addAxis(x, Qt::AlignBottom);
    series->attachAxis(x);
    QValueAxis* y = new QValueAxis();
    y->setRange(0, 3);
    y->setTickCount(4);
    y->setTitleText("Weighted score (0-3)");
    chart->addAxis(y, Qt::AlignLeft);
    series->attachAxis(y);
    chart->legend()->setVisible(false);
    profileChartView_->setChart(chart);
}

void EngineerDevelopmentWidget::updateGapTable()
{
    gapTable_->setRowCount(0);
    const int areaFilter = areaCombo_->currentData().toInt();
    const int machineFilter = machineCombo_->currentData().toInt();
    QHash<int, int> scores;
    for (const Assessment& assessment : engineerAssessments_) scores[assessment.competencyId()] = assessment.score();
    for (const ProductionArea& area : DataCache::instance().getAreas()) {
        if (areaFilter > 0 && area.id() != areaFilter) continue;
        for (const Machine& machine : DataCache::instance().getMachinesByArea(area.id())) {
            if (machineFilter > 0 && machine.id() != machineFilter) continue;
            for (const Competency& competency : DataCache::instance().getCompetenciesByMachine(machine.id())) {
                const int score = scores.value(competency.id(), 0);
                if (score >= 2) continue;
                const int row = gapTable_->rowCount();
                gapTable_->insertRow(row);
                QTableWidgetItem* areaItem = new QTableWidgetItem(area.name());
                areaItem->setData(Qt::UserRole, area.id());
                QTableWidgetItem* machineItem = new QTableWidgetItem(machine.name());
                machineItem->setData(Qt::UserRole, machine.id());
                QTableWidgetItem* competencyItem = new QTableWidgetItem(competency.name());
                competencyItem->setData(Qt::UserRole, competency.id());
                gapTable_->setItem(row, 0, areaItem);
                gapTable_->setItem(row, 1, machineItem);
                gapTable_->setItem(row, 2, competencyItem);
                gapTable_->setItem(row, 3, new QTableWidgetItem(QString("%1 - %2").arg(score).arg(scoreLabel(score))));
                gapTable_->setItem(row, 4, new QTableWidgetItem("2 - Trained"));
            }
        }
    }
}

void EngineerDevelopmentWidget::updatePlanTable()
{
    planTable_->setRowCount(0);
    for (const DevelopmentPlanItem& item : planRepo_.findByEngineer(currentEngineerId())) {
        const int row = planTable_->rowCount();
        planTable_->insertRow(row);
        QTableWidgetItem* area = new QTableWidgetItem(item.productionAreaName);
        area->setData(Qt::UserRole, item.id);
        planTable_->setItem(row, 0, area);
        planTable_->setItem(row, 1, new QTableWidgetItem(item.machineName));
        planTable_->setItem(row, 2, new QTableWidgetItem(item.competencyName));
        planTable_->setItem(row, 3, new QTableWidgetItem(QString::number(item.currentScore)));
        planTable_->setItem(row, 4, new QTableWidgetItem(QString::number(item.targetScore)));
        planTable_->setItem(row, 5, new QTableWidgetItem(item.dueDate.isValid() ? item.dueDate.toString("dd MMM yyyy") : "Not set"));
        planTable_->setItem(row, 6, new QTableWidgetItem(item.status));
    }
}

QString EngineerDevelopmentWidget::scoreLabel(int score) const
{
    switch (score) {
    case 0: return "No knowledge";
    case 1: return "Limited";
    case 2: return "Trained";
    case 3: return "Able to train others";
    default: return "Unknown";
    }
}

QString EngineerDevelopmentWidget::guidanceForScore(int score) const
{
    if (score <= 0) return "Review the approved procedure, shadow a competent engineer, then complete supervised practical repetitions.";
    if (score == 1) return "Complete at least three successful supervised occurrences and record evidence before reassessment.";
    return "Consolidate through fault scenarios and demonstrate the task back to a competent assessor.";
}

void EngineerDevelopmentWidget::addSelectedGapsToPlan()
{
    const QModelIndexList rows = gapTable_->selectionModel()->selectedRows();
    if (rows.isEmpty()) {
        QMessageBox::information(this, "Select gaps", "Select one or more competency rows first.");
        return;
    }
    Session* session = Application::instance().session();
    int added = 0;
    for (const QModelIndex& index : rows) {
        const int row = index.row();
        DevelopmentPlanItem item;
        item.engineerId = currentEngineerId();
        item.productionAreaId = gapTable_->item(row, 0)->data(Qt::UserRole).toInt();
        item.machineId = gapTable_->item(row, 1)->data(Qt::UserRole).toInt();
        item.competencyId = gapTable_->item(row, 2)->data(Qt::UserRole).toInt();
        item.currentScore = gapTable_->item(row, 3)->text().left(1).toInt();
        item.targetScore = targetScoreCombo_->currentData().toInt();
        item.dueDate = dueDateEdit_->date();
        item.managerNotes = managerNotesEdit_->toPlainText().trimmed();
        item.guidance = guidanceForScore(item.currentScore);
        if (session) {
            item.createdByUserId = session->userId();
            item.createdByName = session->username();
        }
        if (planRepo_.create(item)) added++;
    }
    managerNotesEdit_->clear();
    updatePlanTable();
    activePlansLabel_->setText(QString::number(planRepo_.findByEngineer(currentEngineerId(), "active").size()));
    QMessageBox::information(this, "Development plan", QString("%1 selected item(s) processed. Existing active items were not duplicated.").arg(added));
}

void EngineerDevelopmentWidget::markSelectedPlanComplete()
{
    const int row = planTable_->currentRow();
    if (row < 0) return;
    if (!planRepo_.updateStatus(planTable_->item(row, 0)->data(Qt::UserRole).toInt(), "completed")) {
        QMessageBox::warning(this, "Update failed", planRepo_.lastError());
    }
    loadEngineerDashboard();
}

void EngineerDevelopmentWidget::cancelSelectedPlanItem()
{
    const int row = planTable_->currentRow();
    if (row < 0) return;
    if (!planRepo_.updateStatus(planTable_->item(row, 0)->data(Qt::UserRole).toInt(), "cancelled")) {
        QMessageBox::warning(this, "Update failed", planRepo_.lastError());
    }
    loadEngineerDashboard();
}

QString EngineerDevelopmentWidget::buildReportHtml()
{
    const Engineer engineer = engineerRepo_.findById(currentEngineerId());
    const QString area = areaCombo_->currentText();
    const QString machine = machineCombo_->currentData().toInt() > 0 ? machineCombo_->currentText() : "All machines";
    QString html = QString(
        "<h1>Engineer Development Focus</h1><p><b>Engineer:</b> %1 &nbsp; <b>Shift:</b> %2</p>"
        "<p><b>Scope:</b> %3 / %4 &nbsp; <b>Generated:</b> %5</p>"
        "<h2>Priority competencies</h2><table width='100%' cellspacing='0' cellpadding='6' border='1'>"
        "<tr><th>Machine</th><th>Competency</th><th>Current</th><th>Target</th><th>Recommended activity</th></tr>")
        .arg(engineer.name().toHtmlEscaped(), engineer.shift().toHtmlEscaped(), area.toHtmlEscaped(),
             machine.toHtmlEscaped(), QDate::currentDate().toString("dd MMM yyyy"));
    for (int row = 0; row < gapTable_->rowCount(); ++row) {
        const int score = gapTable_->item(row, 3)->text().left(1).toInt();
        html += QString("<tr><td>%1</td><td>%2</td><td>%3</td><td>2</td><td>%4</td></tr>")
            .arg(gapTable_->item(row, 1)->text().toHtmlEscaped(),
                 gapTable_->item(row, 2)->text().toHtmlEscaped(),
                 gapTable_->item(row, 3)->text().toHtmlEscaped(),
                 guidanceForScore(score).toHtmlEscaped());
    }
    if (gapTable_->rowCount() == 0) html += "<tr><td colspan='5'>No competencies below score 2 in this scope.</td></tr>";
    html += "</table><h2>Agreed development plan</h2><table width='100%' cellspacing='0' cellpadding='6' border='1'>"
            "<tr><th>Machine</th><th>Competency</th><th>Target</th><th>Due</th><th>Status</th></tr>";
    const QList<DevelopmentPlanItem> plans = planRepo_.findByEngineer(currentEngineerId(), "active");
    int included = 0;
    const int areaFilter = areaCombo_->currentData().toInt();
    const int machineFilter = machineCombo_->currentData().toInt();
    for (const DevelopmentPlanItem& item : plans) {
        if (areaFilter > 0 && item.productionAreaId != areaFilter) continue;
        if (machineFilter > 0 && item.machineId != machineFilter) continue;
        html += QString("<tr><td>%1</td><td>%2</td><td>%3</td><td>%4</td><td>%5</td></tr>")
            .arg(item.machineName.toHtmlEscaped(), item.competencyName.toHtmlEscaped())
            .arg(item.targetScore)
            .arg(item.dueDate.isValid() ? item.dueDate.toString("dd MMM yyyy") : "Not set", item.status.toHtmlEscaped());
        included++;
    }
    if (included == 0) html += "<tr><td colspan='5'>No active plan items saved for this scope.</td></tr>";
    html += "</table><h2>Assessment standard</h2><p>0 - No knowledge | 1 - Limited | 2 - Trained | 3 - Able to train others competently</p>";
    return html;
}

void EngineerDevelopmentWidget::generateReport()
{
    reportHtml_ = buildReportHtml();
    reportPreview_->setHtml(reportHtml_);
    exportPdfButton_->setEnabled(true);
}

void EngineerDevelopmentWidget::exportReportPdf()
{
    if (reportHtml_.isEmpty()) generateReport();
    const QString filename = QFileDialog::getSaveFileName(this, "Export engineer development report", "Engineer_Development_Plan.pdf", "PDF (*.pdf)");
    if (filename.isEmpty()) return;
    QPrinter printer(QPrinter::HighResolution);
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setOutputFileName(filename);
    printer.setPageSize(QPageSize(QPageSize::A4));
    QTextDocument document;
    document.setHtml(reportHtml_);
    document.print(&printer);
    QMessageBox::information(this, "Report exported", "The engineer development report was exported successfully.");
}
