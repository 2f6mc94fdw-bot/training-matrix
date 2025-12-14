#ifndef PRODUCTIONAREASWIDGET_H
#define PRODUCTIONAREASWIDGET_H

#include <QWidget>
#include <QTreeWidget>
#include <QPushButton>
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

private:
    void setupUI();
    void loadProductionAreas();
    void showAreaDialog(const ProductionArea* area = nullptr);
    void showMachineDialog(int parentAreaId, const Machine* machine = nullptr);
    void showCompetencyDialog(int parentMachineId, const Competency* competency = nullptr);

private:
    QTreeWidget* treeWidget_;
    QPushButton* addButton_;
    QPushButton* editButton_;
    QPushButton* deleteButton_;
    QPushButton* refreshButton_;

    ProductionRepository repository_;
};

#endif // PRODUCTIONAREASWIDGET_H
