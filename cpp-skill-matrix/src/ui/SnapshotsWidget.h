#ifndef SNAPSHOTSWIDGET_H
#define SNAPSHOTSWIDGET_H

#include <QWidget>
#include <QListWidget>
#include <QPushButton>
#include "../database/SnapshotRepository.h"

class SnapshotsWidget : public QWidget
{
    Q_OBJECT

public:
    explicit SnapshotsWidget(QWidget* parent = nullptr);
    ~SnapshotsWidget();

private slots:
    void onCreateSnapshotClicked();
    void onDeleteSnapshotClicked();
    void onRefreshClicked();

private:
    void setupUI();
    void loadSnapshots();

private:
    QListWidget* snapshotList_;
    QPushButton* createButton_;
    QPushButton* deleteButton_;
    QPushButton* refreshButton_;

    SnapshotRepository snapshotRepo_;
};

#endif // SNAPSHOTSWIDGET_H
