#ifndef IMPORTEXPORTDIALOG_H
#define IMPORTEXPORTDIALOG_H

#include <QWidget>

class ImportExportDialog : public QWidget
{
    Q_OBJECT

public:
    explicit ImportExportDialog(QWidget* parent = nullptr);
    ~ImportExportDialog();
};

#endif // IMPORTEXPORTDIALOG_H
