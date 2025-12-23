#ifndef PRODUCTIONAREASWIDGET_H
#define PRODUCTIONAREASWIDGET_H

#include <QWidget>
#include <QTreeWidget>
#include <QPushButton>
#include <QComboBox>
#include "../database/ProductionRepository.h"

enum ItemType {
    AreaItem = 0,
    MachineItem = 1,
    CompetencyItem = 2
};

class ProductionAreasWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ProductionAreasWidget(QWidget* parent = nullptr);
    ~ProductionAreasWidget();

private slots:
    void onAddClicked();
    void onEditClicked();
    void onDeleteClicked();
    void onRefreshClicked();
    void onTreeItemDoubleClicked(QTreeWidgetItem* item, int column);
    void onAreaFilterChanged(int index);

private:
    void setupUI();
    void loadProductionAreas();
    void loadAreaFilter();
    void loadMachinesForArea(int areaId);
    void showAreaDialog(const ProductionArea* area = nullptr);
    void showMachineDialog(int parentAreaId, const Machine* machine = nullptr);
    void showCompetencyDialog(int parentMachineId, const Competency* competency = nullptr);

private:
    QComboBox* areaFilterCombo_;
    QTreeWidget* treeWidget_;
    QPushButton* addButton_;
    QPushButton* editButton_;
    QPushButton* deleteButton_;
    QPushButton* refreshButton_;
    QPushButton* manageAreasButton_;

    ProductionRepository repository_;
    QList<ProductionArea> allAreas_;
};

#endif // PRODUCTIONAREASWIDGET_H
