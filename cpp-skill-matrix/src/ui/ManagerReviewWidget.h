#ifndef MANAGERREVIEWWIDGET_H
#define MANAGERREVIEWWIDGET_H

#include <QComboBox>
#include <QHash>
#include <QLabel>
#include <QLineEdit>
#include <QPlainTextEdit>
#include <QTableWidget>
#include <QWidget>

#include "../database/AssessmentSubmissionRepository.h"

class QPushButton;

class ManagerReviewWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ManagerReviewWidget(QWidget* parent = nullptr);
    ~ManagerReviewWidget();

public slots:
    void refresh();

private slots:
    void applyFilters();
    void onSelectionChanged();

private:
    void setupUI();
    void loadData();
    void rebuildTable();
    void applyDecision(const QString& decision);
    QString submissionIdForRow(int row) const;
    bool matchesFilters(const AssessmentSubmission& submission) const;

    QComboBox* typeFilterCombo_;
    QComboBox* areaFilterCombo_;
    QComboBox* focusFilterCombo_;
    QLineEdit* searchEdit_;
    QLabel* summaryLabel_;
    QTableWidget* table_;
    QPlainTextEdit* decisionNoteEdit_;
    QLabel* selectedItemLabel_;
    QPushButton* approveButton_;
    QPushButton* rejectButton_;

    AssessmentSubmissionRepository submissionRepo_;
    QList<AssessmentSubmission> submissions_;
    QHash<QString, AssessmentSubmission> submissionsById_;
};

#endif // MANAGERREVIEWWIDGET_H
