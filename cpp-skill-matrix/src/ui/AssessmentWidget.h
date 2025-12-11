#ifndef ASSESSMENTWIDGET_H
#define ASSESSMENTWIDGET_H

#include <QWidget>
#include <QComboBox>
#include <QTableWidget>
#include <QPushButton>
#include "../database/EngineerRepository.h"
#include "../database/ProductionRepository.h"
#include "../database/AssessmentRepository.h"

class AssessmentWidget : public QWidget
{
    Q_OBJECT

public:
    explicit AssessmentWidget(QWidget* parent = nullptr);
    ~AssessmentWidget();

private slots:
    void onEngineerChanged(int index);
    void onProductionAreaChanged(int index);
    void onSaveClicked();
    void onLoadClicked();

private:
    void setupUI();
    void loadEngineers();
    void loadProductionAreas();
    void loadAssessmentData();

private:
    QComboBox* engineerCombo_;
    QComboBox* productionAreaCombo_;
    QTableWidget* assessmentTable_;
    QPushButton* saveButton_;
    QPushButton* loadButton_;

    EngineerRepository engineerRepo_;
    ProductionRepository productionRepo_;
    AssessmentRepository assessmentRepo_;
};

#endif // ASSESSMENTWIDGET_H
