#ifndef CERTIFICATIONSWIDGET_H
#define CERTIFICATIONSWIDGET_H

#include <QWidget>
#include <QTableWidget>
#include <QPushButton>
#include <QComboBox>
#include "../database/CertificationRepository.h"
#include "../database/EngineerRepository.h"

class CertificationsWidget : public QWidget
{
    Q_OBJECT

public:
    explicit CertificationsWidget(QWidget* parent = nullptr);
    ~CertificationsWidget();

private slots:
    void onEngineerFilterChanged(int index);
    void onAddClicked();
    void onDeleteClicked();
    void onRefreshClicked();

private:
    void setupUI();
    void loadEngineers();
    void loadCertifications();
    void showCertificationDialog();

private:
    QComboBox* engineerFilterCombo_;
    QTableWidget* certificationsTable_;
    QPushButton* addButton_;
    QPushButton* deleteButton_;
    QPushButton* refreshButton_;

    CertificationRepository certificationRepo_;
    EngineerRepository engineerRepo_;
};

#endif // CERTIFICATIONSWIDGET_H
