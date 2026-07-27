#ifndef ASSESSMENTWIDGET_H
#define ASSESSMENTWIDGET_H

#include <QAbstractTableModel>
#include <QComboBox>
#include <QElapsedTimer>
#include <QFutureWatcher>
#include <QLabel>
#include <QLineEdit>
#include <QMap>
#include <QSortFilterProxyModel>
#include <QTableView>
#include <QVBoxLayout>
#include <QWidget>

#include "../database/AssessmentSubmissionRepository.h"
#include "../database/DatabaseManager.h"
#include "../database/EngineerRepository.h"
#include "../database/ProductionRepository.h"
#include "../models/Assessment.h"
#include "../models/Competency.h"
#include "../models/Engineer.h"
#include "../models/ProductionArea.h"

struct AssessmentTableRow {
    QString engineerId;
    QString engineerName;
    int areaId = 0;
    QString areaName;
    int machineId = 0;
    QString machineName;
    int competencyId = 0;
    QString competencyName;
    int score = 0;
};

struct AssessmentLoadResult {
    bool success = false;
    QString error;
    QList<Engineer> engineers;
    QList<Assessment> assessments;
    QList<ProductionArea> areas;
};

class AssessmentTableModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    enum Column {
        EngineerColumn = 0,
        AreaColumn,
        MachineColumn,
        CompetencyColumn,
        ScoreColumn,
        ColumnCount
    };

    enum Role {
        EngineerIdRole = Qt::UserRole + 1,
        AreaIdRole,
        MachineIdRole,
        CompetencyIdRole,
        ScoreRole
    };

    explicit AssessmentTableModel(QObject* parent = nullptr);

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    Qt::ItemFlags flags(const QModelIndex& index) const override;
    bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole) override;

    void setRows(QVector<AssessmentTableRow> rows);
    bool setScoreForRow(int row, int score, bool emitChangeSignal);

signals:
    void scoreEdited(int sourceRow, const QString& engineerId, int areaId, int machineId, int competencyId, int oldScore, int newScore);

private:
    QVector<AssessmentTableRow> rows_;
};

class AssessmentFilterProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT

public:
    explicit AssessmentFilterProxyModel(QObject* parent = nullptr);
    void setAreaFilterId(int areaId);
    int areaFilterId() const { return areaFilterId_; }
    void setEngineerFilterText(const QString& text);

protected:
    bool filterAcceptsRow(int sourceRow, const QModelIndex& sourceParent) const override;

private:
    int areaFilterId_;
    QString engineerFilterText_;
};

class AssessmentWidget : public QWidget
{
    Q_OBJECT

public:
    explicit AssessmentWidget(QWidget* parent = nullptr);
    ~AssessmentWidget();

protected:
    void showEvent(QShowEvent* event) override;

private slots:
    void onAreaFilterChanged(int index);
    void onRefreshClicked();
    void onScoreEdited(int sourceRow, const QString& engineerId, int areaId, int machineId, int competencyId, int oldScore, int newScore);
    void onBackgroundLoadFinished();

private:
    struct MachineData {
        Machine machine;
        QList<Competency> competencies;
    };

    void setupUI();
    void loadAssessments();
    void applyLoadedData(const AssessmentLoadResult& result);
    QString assessmentKey(const QString& engineerId, int areaId, int machineId, int competencyId) const;
    void updateSummary();

private:
    QComboBox* areaFilterCombo_;
    QLineEdit* engineerSearchEdit_;
    QLabel* loadingLabel_;
    QLabel* summaryLabel_;
    QTableView* assessmentsTable_;
    QVBoxLayout* mainLayout_;

    AssessmentTableModel* tableModel_;
    AssessmentFilterProxyModel* proxyModel_;
    QFutureWatcher<AssessmentLoadResult>* loadWatcher_;

    EngineerRepository engineerRepo_;
    ProductionRepository productionRepo_;
    AssessmentSubmissionRepository assessmentWorkflowRepo_;

    bool isFirstShow_;
    bool isLoading_;
    bool isApplyingRollback_;
    QElapsedTimer loadTimer_;

    QMap<QString, int> cachedAssessmentScores_;
    QMap<int, QList<MachineData>> cachedAreaToMachines_;
    QMap<int, QString> cachedAreaNames_;

    int totalCompetencies_;
    int trainedCompetencies_;
    QMap<int, int> areaTotalCompetencies_;
    QMap<int, int> areaTrainedCompetencies_;
};

#endif // ASSESSMENTWIDGET_H
