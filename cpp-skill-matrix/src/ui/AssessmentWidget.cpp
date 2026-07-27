#include "AssessmentWidget.h"

#include "../core/DataCache.h"
#include "../core/Application.h"
#include "../models/Assessment.h"
#include "../models/ProductionArea.h"
#include "../utils/Logger.h"

#include <QtConcurrent/QtConcurrent>
#include <QComboBox>
#include <QGroupBox>
#include <QHeaderView>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QPushButton>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QShowEvent>
#include <QStyledItemDelegate>
#include <QUuid>
#include <algorithm>

namespace {
class ScoreComboDelegate : public QStyledItemDelegate
{
public:
    explicit ScoreComboDelegate(QObject* parent = nullptr) : QStyledItemDelegate(parent) {}

    QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem&, const QModelIndex&) const override
    {
        QComboBox* editor = new QComboBox(parent);
        editor->addItem("0 - Not Trained", 0);
        editor->addItem("1 - Basic", 1);
        editor->addItem("2 - Competent", 2);
        editor->addItem("3 - Expert", 3);
        return editor;
    }

    void setEditorData(QWidget* editor, const QModelIndex& index) const override
    {
        QComboBox* combo = qobject_cast<QComboBox*>(editor);
        if (!combo) {
            return;
        }
        const int score = index.model()->data(index, Qt::EditRole).toInt();
        const int comboIndex = combo->findData(score);
        combo->setCurrentIndex(comboIndex >= 0 ? comboIndex : 0);
    }

    void setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const override
    {
        QComboBox* combo = qobject_cast<QComboBox*>(editor);
        if (!combo) {
            return;
        }
        model->setData(index, combo->currentData(), Qt::EditRole);
    }

    void updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex&) const override
    {
        editor->setGeometry(option.rect);
    }
};
} // namespace

AssessmentTableModel::AssessmentTableModel(QObject* parent)
    : QAbstractTableModel(parent)
{
}

int AssessmentTableModel::rowCount(const QModelIndex& parent) const
{
    if (parent.isValid()) {
        return 0;
    }
    return rows_.size();
}

int AssessmentTableModel::columnCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent);
    return ColumnCount;
}

QVariant AssessmentTableModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid() || index.row() < 0 || index.row() >= rows_.size()) {
        return QVariant();
    }

    const AssessmentTableRow& row = rows_.at(index.row());

    if (role == Qt::DisplayRole) {
        switch (index.column()) {
            case EngineerColumn:
                return row.engineerName;
            case AreaColumn:
                return row.areaName;
            case MachineColumn:
                return row.machineName;
            case CompetencyColumn:
                return row.competencyName;
            case ScoreColumn:
                return row.score;
            default:
                return QVariant();
        }
    }

    if (role == Qt::EditRole && index.column() == ScoreColumn) {
        return row.score;
    }

    if (role == EngineerIdRole) {
        return row.engineerId;
    }
    if (role == AreaIdRole) {
        return row.areaId;
    }
    if (role == MachineIdRole) {
        return row.machineId;
    }
    if (role == CompetencyIdRole) {
        return row.competencyId;
    }
    if (role == ScoreRole) {
        return row.score;
    }

    return QVariant();
}

QVariant AssessmentTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation != Qt::Horizontal || role != Qt::DisplayRole) {
        return QAbstractTableModel::headerData(section, orientation, role);
    }

    switch (section) {
        case EngineerColumn:
            return "Engineer";
        case AreaColumn:
            return "Area";
        case MachineColumn:
            return "Machine";
        case CompetencyColumn:
            return "Competency";
        case ScoreColumn:
            return "Score";
        default:
            return QVariant();
    }
}

Qt::ItemFlags AssessmentTableModel::flags(const QModelIndex& index) const
{
    if (!index.isValid()) {
        return Qt::NoItemFlags;
    }

    Qt::ItemFlags itemFlags = Qt::ItemIsEnabled | Qt::ItemIsSelectable;
    if (index.column() == ScoreColumn) {
        itemFlags |= Qt::ItemIsEditable;
    }
    return itemFlags;
}

bool AssessmentTableModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
    if (!index.isValid() || index.column() != ScoreColumn || role != Qt::EditRole) {
        return false;
    }

    const int newScore = value.toInt();
    if (newScore < 0 || newScore > 3) {
        return false;
    }

    AssessmentTableRow& row = rows_[index.row()];
    const int oldScore = row.score;
    if (oldScore == newScore) {
        return false;
    }

    row.score = newScore;
    emit dataChanged(index, index, {Qt::DisplayRole, Qt::EditRole, ScoreRole});
    emit scoreEdited(index.row(), row.engineerId, row.areaId, row.machineId, row.competencyId, oldScore, newScore);
    return true;
}

void AssessmentTableModel::setRows(QVector<AssessmentTableRow> rows)
{
    beginResetModel();
    rows_ = std::move(rows);
    endResetModel();
}

bool AssessmentTableModel::setScoreForRow(int row, int score, bool emitChangeSignal)
{
    if (row < 0 || row >= rows_.size() || score < 0 || score > 3) {
        return false;
    }

    const int oldScore = rows_[row].score;
    if (oldScore == score) {
        return true;
    }

    rows_[row].score = score;
    const QModelIndex rowIndex = index(row, ScoreColumn);
    emit dataChanged(rowIndex, rowIndex, {Qt::DisplayRole, Qt::EditRole, ScoreRole});
    if (emitChangeSignal) {
        const AssessmentTableRow& rowData = rows_[row];
        emit scoreEdited(row, rowData.engineerId, rowData.areaId, rowData.machineId, rowData.competencyId, oldScore, score);
    }
    return true;
}

AssessmentFilterProxyModel::AssessmentFilterProxyModel(QObject* parent)
    : QSortFilterProxyModel(parent)
    , areaFilterId_(0)
    , engineerFilterText_("")
{
}

void AssessmentFilterProxyModel::setAreaFilterId(int areaId)
{
    areaFilterId_ = areaId;
    invalidateFilter();
}

void AssessmentFilterProxyModel::setEngineerFilterText(const QString& text)
{
    engineerFilterText_ = text.trimmed();
    invalidateFilter();
}

bool AssessmentFilterProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex& sourceParent) const
{
    if (areaFilterId_ != 0) {
        const QModelIndex areaIndex = sourceModel()->index(sourceRow, AssessmentTableModel::AreaColumn, sourceParent);
        if (sourceModel()->data(areaIndex, AssessmentTableModel::AreaIdRole).toInt() != areaFilterId_) {
            return false;
        }
    }

    if (engineerFilterText_.isEmpty()) {
        return true;
    }

    const QModelIndex engineerIndex = sourceModel()->index(sourceRow, AssessmentTableModel::EngineerColumn, sourceParent);
    const QString engineerName = sourceModel()->data(engineerIndex, Qt::DisplayRole).toString();
    return engineerName.contains(engineerFilterText_, Qt::CaseInsensitive);
}

AssessmentWidget::AssessmentWidget(QWidget* parent)
    : QWidget(parent)
    , areaFilterCombo_(nullptr)
    , engineerSearchEdit_(nullptr)
    , loadingLabel_(nullptr)
    , summaryLabel_(nullptr)
    , assessmentsTable_(nullptr)
    , mainLayout_(nullptr)
    , tableModel_(nullptr)
    , proxyModel_(nullptr)
    , loadWatcher_(nullptr)
    , isFirstShow_(true)
    , isLoading_(false)
    , isApplyingRollback_(false)
    , totalCompetencies_(0)
    , trainedCompetencies_(0)
{
    setupUI();
    loadWatcher_ = new QFutureWatcher<AssessmentLoadResult>(this);
    connect(loadWatcher_, &QFutureWatcher<AssessmentLoadResult>::finished,
            this, &AssessmentWidget::onBackgroundLoadFinished);
    Logger::instance().info("AssessmentWidget", "Assessment widget initialized (model/view)");
}

AssessmentWidget::~AssessmentWidget() = default;

void AssessmentWidget::showEvent(QShowEvent* event)
{
    QWidget::showEvent(event);

    if (isFirstShow_) {
        isFirstShow_ = false;
        loadAssessments();
    }
}

void AssessmentWidget::setupUI()
{
    mainLayout_ = new QVBoxLayout(this);
    mainLayout_->setSpacing(14);
    mainLayout_->setContentsMargins(24, 24, 24, 24);

    QLabel* titleLabel = new QLabel("Production Assessments", this);
    QFont titleFont = titleLabel->font();
    titleFont.setPointSize(30);
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);
    mainLayout_->addWidget(titleLabel);

    QLabel* subtitleLabel = new QLabel("Manager review workspace: filter, adjust scores, and track coverage", this);
    QFont subtitleFont = subtitleLabel->font();
    subtitleFont.setPointSize(14);
    subtitleLabel->setFont(subtitleFont);
    subtitleLabel->setStyleSheet("color: #475569;");
    mainLayout_->addWidget(subtitleLabel);

    QGroupBox* controlBox = new QGroupBox(this);
    QVBoxLayout* controlLayout = new QVBoxLayout(controlBox);
    controlLayout->setContentsMargins(14, 12, 14, 10);
    controlLayout->setSpacing(10);

    QHBoxLayout* filterLayout = new QHBoxLayout();
    filterLayout->setSpacing(10);

    QLabel* filterLabel = new QLabel("Filter by Area:", this);
    QFont filterFont = filterLabel->font();
    filterFont.setPointSize(14);
    filterFont.setWeight(QFont::Medium);
    filterLabel->setFont(filterFont);
    filterLayout->addWidget(filterLabel);

    areaFilterCombo_ = new QComboBox(this);
    areaFilterCombo_->setMinimumWidth(260);
    areaFilterCombo_->addItem("All Areas", 0);

    const QList<ProductionArea> areas = productionRepo_.findAllAreas();
    for (const ProductionArea& area : areas) {
        areaFilterCombo_->addItem(area.name(), area.id());
    }

    connect(areaFilterCombo_, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &AssessmentWidget::onAreaFilterChanged);
    filterLayout->addWidget(areaFilterCombo_);

    QLabel* searchLabel = new QLabel("Engineer:", this);
    searchLabel->setFont(filterFont);
    filterLayout->addWidget(searchLabel);

    engineerSearchEdit_ = new QLineEdit(this);
    engineerSearchEdit_->setPlaceholderText("Search engineer name...");
    engineerSearchEdit_->setClearButtonEnabled(true);
    engineerSearchEdit_->setMinimumWidth(220);
    connect(engineerSearchEdit_, &QLineEdit::textChanged, this, [this](const QString& text) {
        proxyModel_->setEngineerFilterText(text);
        updateSummary();
    });
    filterLayout->addWidget(engineerSearchEdit_);

    QPushButton* refreshButton = new QPushButton("Refresh", this);
    refreshButton->setMinimumWidth(100);
    connect(refreshButton, &QPushButton::clicked, this, &AssessmentWidget::onRefreshClicked);
    filterLayout->addWidget(refreshButton);

    filterLayout->addStretch();

    summaryLabel_ = new QLabel(this);
    summaryLabel_->setStyleSheet("color: #475569; font-size: 13px; font-weight: 700;");
    filterLayout->addWidget(summaryLabel_);

    controlLayout->addLayout(filterLayout);

    QLabel* editHintLabel = new QLabel("Tip: click a score cell to adjust it. Engineer will be notified when a manager changes a score.", this);
    editHintLabel->setStyleSheet("color: #64748b; font-size: 12px;");
    controlLayout->addWidget(editHintLabel);

    mainLayout_->addWidget(controlBox);

    loadingLabel_ = new QLabel("Loading assessments...", this);
    loadingLabel_->setAlignment(Qt::AlignCenter);
    loadingLabel_->setStyleSheet("color: #64748b; font-size: 14px; padding: 8px;");
    loadingLabel_->setVisible(false);
    mainLayout_->addWidget(loadingLabel_);

    tableModel_ = new AssessmentTableModel(this);
    proxyModel_ = new AssessmentFilterProxyModel(this);
    proxyModel_->setSourceModel(tableModel_);
    proxyModel_->setDynamicSortFilter(true);

    assessmentsTable_ = new QTableView(this);
    assessmentsTable_->setModel(proxyModel_);
    assessmentsTable_->setAlternatingRowColors(true);
    assessmentsTable_->setSelectionBehavior(QAbstractItemView::SelectRows);
    assessmentsTable_->setSelectionMode(QAbstractItemView::SingleSelection);
    assessmentsTable_->setSortingEnabled(true);
    assessmentsTable_->setEditTriggers(QAbstractItemView::CurrentChanged | QAbstractItemView::SelectedClicked);
    assessmentsTable_->setWordWrap(false);
    assessmentsTable_->verticalHeader()->setVisible(false);
    assessmentsTable_->verticalHeader()->setDefaultSectionSize(36);
    assessmentsTable_->horizontalHeader()->setStretchLastSection(true);
    assessmentsTable_->horizontalHeader()->setSectionResizeMode(AssessmentTableModel::EngineerColumn, QHeaderView::ResizeToContents);
    assessmentsTable_->horizontalHeader()->setSectionResizeMode(AssessmentTableModel::AreaColumn, QHeaderView::ResizeToContents);
    assessmentsTable_->horizontalHeader()->setSectionResizeMode(AssessmentTableModel::MachineColumn, QHeaderView::ResizeToContents);
    assessmentsTable_->horizontalHeader()->setSectionResizeMode(AssessmentTableModel::CompetencyColumn, QHeaderView::Stretch);
    assessmentsTable_->horizontalHeader()->setSectionResizeMode(AssessmentTableModel::ScoreColumn, QHeaderView::ResizeToContents);
    assessmentsTable_->setItemDelegateForColumn(AssessmentTableModel::ScoreColumn, new ScoreComboDelegate(this));
    mainLayout_->addWidget(assessmentsTable_);

    connect(tableModel_, &AssessmentTableModel::scoreEdited, this, &AssessmentWidget::onScoreEdited);
}

void AssessmentWidget::loadAssessments()
{
    if (isLoading_) {
        return;
    }
    isLoading_ = true;

    loadingLabel_->setVisible(true);
    loadingLabel_->setText("Loading assessment data...");
    loadTimer_.start();

    const QSqlDatabase mainDb = DatabaseManager::instance().database();
    if (!mainDb.isOpen()) {
        loadingLabel_->setVisible(false);
        isLoading_ = false;
        QMessageBox::warning(this, "Database Error", "Database connection is not open.");
        return;
    }

    const QString driver = mainDb.driverName();
    const QString databaseName = mainDb.databaseName();
    const QString connectOptions = mainDb.connectOptions();

    auto future = QtConcurrent::run([driver, databaseName, connectOptions]() -> AssessmentLoadResult {
        AssessmentLoadResult result;
        const QString connName = QString("AssessmentWidgetLoad_%1")
                                     .arg(QUuid::createUuid().toString(QUuid::WithoutBraces));

        {
            QSqlDatabase workerDb = QSqlDatabase::addDatabase(driver, connName);
            workerDb.setDatabaseName(databaseName);
            workerDb.setConnectOptions(connectOptions);

            if (!workerDb.open()) {
                result.error = workerDb.lastError().text();
                workerDb = QSqlDatabase();
                QSqlDatabase::removeDatabase(connName);
                return result;
            }

            QSqlQuery engineerQuery(workerDb);
            engineerQuery.setForwardOnly(true);
            if (!engineerQuery.exec("SELECT id, name, shift, created_at, updated_at FROM engineers ORDER BY name")) {
                result.error = engineerQuery.lastError().text();
                workerDb.close();
                workerDb = QSqlDatabase();
                QSqlDatabase::removeDatabase(connName);
                return result;
            }

            while (engineerQuery.next()) {
                Engineer engineer;
                engineer.setId(engineerQuery.value(0).toString());
                engineer.setName(engineerQuery.value(1).toString());
                engineer.setShift(engineerQuery.value(2).toString());
                engineer.setCreatedAt(engineerQuery.value(3).toDateTime());
                engineer.setUpdatedAt(engineerQuery.value(4).toDateTime());
                result.engineers.append(engineer);
            }

            QSqlQuery areaQuery(workerDb);
            areaQuery.setForwardOnly(true);
            if (!areaQuery.exec("SELECT id, name, created_at, updated_at FROM production_areas ORDER BY name")) {
                result.error = areaQuery.lastError().text();
                workerDb.close();
                workerDb = QSqlDatabase();
                QSqlDatabase::removeDatabase(connName);
                return result;
            }

            while (areaQuery.next()) {
                ProductionArea area;
                area.setId(areaQuery.value(0).toInt());
                area.setName(areaQuery.value(1).toString());
                area.setCreatedAt(areaQuery.value(2).toDateTime());
                area.setUpdatedAt(areaQuery.value(3).toDateTime());
                result.areas.append(area);
            }

            QSqlQuery assessmentQuery(workerDb);
            assessmentQuery.setForwardOnly(true);
            if (!assessmentQuery.exec("SELECT id, engineer_id, production_area_id, machine_id, competency_id, score, created_at, updated_at FROM assessments")) {
                result.error = assessmentQuery.lastError().text();
                workerDb.close();
                workerDb = QSqlDatabase();
                QSqlDatabase::removeDatabase(connName);
                return result;
            }

            while (assessmentQuery.next()) {
                Assessment assessment;
                assessment.setId(assessmentQuery.value(0).toInt());
                assessment.setEngineerId(assessmentQuery.value(1).toString());
                assessment.setProductionAreaId(assessmentQuery.value(2).toInt());
                assessment.setMachineId(assessmentQuery.value(3).toInt());
                assessment.setCompetencyId(assessmentQuery.value(4).toInt());
                assessment.setScore(assessmentQuery.value(5).toInt());
                assessment.setCreatedAt(assessmentQuery.value(6).toDateTime());
                assessment.setUpdatedAt(assessmentQuery.value(7).toDateTime());
                result.assessments.append(assessment);
            }

            workerDb.close();
            workerDb = QSqlDatabase();
        }

        QSqlDatabase::removeDatabase(connName);
        result.success = true;
        return result;
    });

    loadWatcher_->setFuture(future);
}

void AssessmentWidget::applyLoadedData(const AssessmentLoadResult& result)
{
    cachedAssessmentScores_.clear();
    cachedAreaToMachines_.clear();
    cachedAreaNames_.clear();
    areaTotalCompetencies_.clear();
    areaTrainedCompetencies_.clear();
    totalCompetencies_ = 0;
    trainedCompetencies_ = 0;

    for (const Assessment& assessment : result.assessments) {
        cachedAssessmentScores_[assessmentKey(
            assessment.engineerId(),
            assessment.productionAreaId(),
            assessment.machineId(),
            assessment.competencyId())] = assessment.score();
    }

    DataCache& cache = DataCache::instance();
    if (!cache.isLoaded()) {
        cache.load();
    }
    int competenciesPerEngineer = 0;

    for (const ProductionArea& area : result.areas) {
        cachedAreaNames_[area.id()] = area.name();
        QList<MachineData> machineDataList;

        const QList<Machine> machines = cache.getMachinesByArea(area.id());
        for (const Machine& machine : machines) {
            MachineData machineData;
            machineData.machine = machine;
            machineData.competencies = cache.getCompetenciesByMachine(machine.id());
            if (!machineData.competencies.isEmpty()) {
                competenciesPerEngineer += machineData.competencies.size();
                machineDataList.append(machineData);
            }
        }

        if (!machineDataList.isEmpty()) {
            cachedAreaToMachines_[area.id()] = machineDataList;
        }
    }

    QList<Engineer> sortedEngineers = result.engineers;
    std::sort(sortedEngineers.begin(), sortedEngineers.end(), [](const Engineer& lhs, const Engineer& rhs) {
        return lhs.name() < rhs.name();
    });

    QVector<AssessmentTableRow> rows;
    rows.reserve(sortedEngineers.size() * std::max(1, competenciesPerEngineer));

    for (const Engineer& engineer : sortedEngineers) {
        for (auto areaIt = cachedAreaToMachines_.cbegin(); areaIt != cachedAreaToMachines_.cend(); ++areaIt) {
            const int areaId = areaIt.key();
            const QString areaName = cachedAreaNames_.value(areaId, "Unknown Area");
            const QList<MachineData>& machines = areaIt.value();

            for (const MachineData& machineData : machines) {
                for (const Competency& competency : machineData.competencies) {
                    const QString key = assessmentKey(engineer.id(), areaId, machineData.machine.id(), competency.id());
                    const int score = cachedAssessmentScores_.value(key, 0);

                    AssessmentTableRow row;
                    row.engineerId = engineer.id();
                    row.engineerName = engineer.name();
                    row.areaId = areaId;
                    row.areaName = areaName;
                    row.machineId = machineData.machine.id();
                    row.machineName = machineData.machine.name();
                    row.competencyId = competency.id();
                    row.competencyName = competency.name();
                    row.score = score;
                    rows.append(row);

                    ++totalCompetencies_;
                    ++areaTotalCompetencies_[areaId];
                    if (score > 0) {
                        ++trainedCompetencies_;
                        ++areaTrainedCompetencies_[areaId];
                    }
                }
            }
        }
    }

    tableModel_->setRows(std::move(rows));
    proxyModel_->setAreaFilterId(areaFilterCombo_->currentData().toInt());
    assessmentsTable_->sortByColumn(AssessmentTableModel::EngineerColumn, Qt::AscendingOrder);
}

void AssessmentWidget::onBackgroundLoadFinished()
{
    const AssessmentLoadResult result = loadWatcher_->result();
    isLoading_ = false;

    if (!result.success) {
        loadingLabel_->setVisible(false);
        const QString errorText = result.error.isEmpty() ? "Unknown database error" : result.error;
        Logger::instance().error("AssessmentWidget", QString("Background load failed: %1").arg(errorText));
        QMessageBox::warning(this, "Load Failed", QString("Failed to load assessment data.\n%1").arg(errorText));
        return;
    }

    applyLoadedData(result);
    loadingLabel_->setVisible(false);
    updateSummary();

    Logger::instance().info(
        "AssessmentWidget",
        QString("Loaded %1 assessment rows in %2 ms (background)")
            .arg(tableModel_->rowCount())
            .arg(loadTimer_.elapsed()));
}

QString AssessmentWidget::assessmentKey(const QString& engineerId, int areaId, int machineId, int competencyId) const
{
    return QString("%1_%2_%3_%4")
        .arg(engineerId)
        .arg(areaId)
        .arg(machineId)
        .arg(competencyId);
}

void AssessmentWidget::updateSummary()
{
    const int filterAreaId = areaFilterCombo_->currentData().toInt();
    const int visibleRows = proxyModel_->rowCount();

    int total = totalCompetencies_;
    int trained = trainedCompetencies_;
    if (filterAreaId != 0) {
        total = areaTotalCompetencies_.value(filterAreaId, 0);
        trained = areaTrainedCompetencies_.value(filterAreaId, 0);
    }

    summaryLabel_->setText(
        QString("%1 visible rows | %2/%3 trained")
            .arg(visibleRows)
            .arg(trained)
            .arg(total));
}

void AssessmentWidget::onAreaFilterChanged(int index)
{
    Q_UNUSED(index);
    proxyModel_->setAreaFilterId(areaFilterCombo_->currentData().toInt());
    updateSummary();
}

void AssessmentWidget::onRefreshClicked()
{
    if (isLoading_) {
        return;
    }
    loadAssessments();
}

void AssessmentWidget::onScoreEdited(int sourceRow, const QString& engineerId, int areaId, int machineId, int competencyId, int oldScore, int newScore)
{
    if (isApplyingRollback_) {
        return;
    }

    const QString key = assessmentKey(engineerId, areaId, machineId, competencyId);
    const bool expectedHasScore = cachedAssessmentScores_.contains(key);
    Session* session = Application::instance().session();
    if (!session || !session->isAdmin()) {
        isApplyingRollback_ = true;
        tableModel_->setScoreForRow(sourceRow, oldScore, false);
        isApplyingRollback_ = false;
        QMessageBox::warning(this, "Permission Denied",
            "An authenticated manager is required to update an official score.");
        return;
    }
    const QString managerUserId = session ? session->userId() : QString();
    const QString managerName = session ? session->username() : QString();

    if (!assessmentWorkflowRepo_.saveOfficialProduction(
            engineerId,
            areaId,
            machineId,
            competencyId,
            expectedHasScore,
            oldScore,
            newScore,
            managerUserId,
            managerName)) {
        Logger::instance().error(
            "AssessmentWidget",
            QString("Failed to save assessment: %1").arg(assessmentWorkflowRepo_.lastError()));
        isApplyingRollback_ = true;
        tableModel_->setScoreForRow(sourceRow, oldScore, false);
        isApplyingRollback_ = false;
        QMessageBox::warning(this, "Save Failed",
            assessmentWorkflowRepo_.lastError() + "\n\nThe displayed change has been reverted.");
        if (assessmentWorkflowRepo_.lastError().startsWith("Conflict:")) {
            loadAssessments();
        }
        return;
    }

    cachedAssessmentScores_[key] = newScore;

    const bool wasTrained = oldScore > 0;
    const bool nowTrained = newScore > 0;
    if (wasTrained != nowTrained) {
        const int delta = nowTrained ? 1 : -1;
        trainedCompetencies_ += delta;
        areaTrainedCompetencies_[areaId] = areaTrainedCompetencies_.value(areaId, 0) + delta;
        updateSummary();
    }
}
