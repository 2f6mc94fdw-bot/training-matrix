#ifndef AUDITLOGWIDGET_H
#define AUDITLOGWIDGET_H

#include <QWidget>
#include <QTableWidget>
#include <QPushButton>
#include <QList>
#include "../database/AuditLogRepository.h"

class QLineEdit;

class AuditLogWidget : public QWidget
{
    Q_OBJECT

public:
    explicit AuditLogWidget(QWidget* parent = nullptr);
    ~AuditLogWidget();

private slots:
    void onRefreshClicked();
    void onExportCSVClicked();
    void onFiltersChanged();

private:
    void setupUI();
    void loadAuditLogs();
    void applyFilters();

private:
    QTableWidget* auditTable_;
    QPushButton* refreshButton_;
    QPushButton* exportCSVButton_;
    QLineEdit* userFilterEdit_;
    QLineEdit* actionFilterEdit_;
    QLineEdit* searchFilterEdit_;

    AuditLogRepository auditLogRepo_;
    QList<AuditLog> allLogs_;
};

#endif // AUDITLOGWIDGET_H
