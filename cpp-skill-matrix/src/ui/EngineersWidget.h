#ifndef ENGINEERSWIDGET_H
#define ENGINEERSWIDGET_H

#include <QWidget>
#include <QTableWidget>
#include <QPushButton>
#include "../database/EngineerRepository.h"

class EngineersWidget : public QWidget
{
    Q_OBJECT

public:
    explicit EngineersWidget(QWidget* parent = nullptr);
    ~EngineersWidget();

private slots:
    void onAddClicked();
    void onEditClicked();
    void onDeleteClicked();
    void onRefreshClicked();
    void onTableDoubleClicked(int row, int column);

private:
    void setupUI();
    void loadEngineers();
    void showEngineerDialog(const Engineer* engineer = nullptr);

private:
    QTableWidget* tableWidget_;
    QPushButton* addButton_;
    QPushButton* editButton_;
    QPushButton* deleteButton_;
    QPushButton* refreshButton_;

    EngineerRepository repository_;
};

#endif // ENGINEERSWIDGET_H
