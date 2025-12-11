#ifndef AUDITLOGWIDGET_H
#define AUDITLOGWIDGET_H

#include <QWidget>

class AuditLogWidget : public QWidget
{
    Q_OBJECT

public:
    explicit AuditLogWidget(QWidget* parent = nullptr);
    ~AuditLogWidget();
};

#endif // AUDITLOGWIDGET_H
