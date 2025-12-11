#ifndef REPORTSWIDGET_H
#define REPORTSWIDGET_H

#include <QWidget>
#include <QPushButton>
#include <QTextEdit>

class ReportsWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ReportsWidget(QWidget* parent = nullptr);
    ~ReportsWidget();

private slots:
    void onGenerateEngineerReportClicked();
    void onGenerateAreaReportClicked();
    void onGenerateComplianceReportClicked();

private:
    void setupUI();

private:
    QTextEdit* reportDisplay_;
    QPushButton* engineerReportButton_;
    QPushButton* areaReportButton_;
    QPushButton* complianceReportButton_;
};

#endif // REPORTSWIDGET_H
