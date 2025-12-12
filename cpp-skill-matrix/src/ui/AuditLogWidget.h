#ifndef AUDITLOGWIDGET_H
#define AUDITLOGWIDGET_H

#include <QWidget>
#include <QTableWidget>
#include <QPushButton>
#include "../database/AuditLogRepository.h"

class AuditLogWidget : public QWidget
{
    Q_OBJECT

public:
    explicit AuditLogWidget(QWidget* parent = nullptr);
    ~AuditLogWidget();

private slots:
    void onRefreshClicked();

private:
    void setupUI();
    void loadAuditLogs();

private:
    QTableWidget* auditTable_;
    QPushButton* refreshButton_;

    AuditLogRepository auditLogRepo_;
};

#endif // AUDITLOGWIDGET_H
