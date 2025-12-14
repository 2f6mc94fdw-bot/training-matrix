#ifndef IMPORTEXPORTDIALOG_H
#define IMPORTEXPORTDIALOG_H

#include <QWidget>
#include <QPushButton>
#include <QTextEdit>

class ImportExportDialog : public QWidget
{
    Q_OBJECT

public:
    explicit ImportExportDialog(QWidget* parent = nullptr);
    ~ImportExportDialog();

private slots:
    void onExportCSVClicked();
    void onExportJSONClicked();
    void onImportCSVClicked();
    void onImportJSONClicked();
    void onBackupClicked();
    void onRestoreClicked();

private:
    void setupUI();

private:
    QPushButton* exportCSVButton_;
    QPushButton* exportJSONButton_;
    QPushButton* importCSVButton_;
    QPushButton* importJSONButton_;
    QPushButton* backupButton_;
    QPushButton* restoreButton_;
    QTextEdit* statusDisplay_;
};

#endif // IMPORTEXPORTDIALOG_H
