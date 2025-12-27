#ifndef DATABASECONNECTIONDIALOG_H
#define DATABASECONNECTIONDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QSpinBox>
#include <QPushButton>
#include <QLabel>

class DatabaseConnectionDialog : public QDialog
{
    Q_OBJECT

public:
    explicit DatabaseConnectionDialog(QWidget* parent = nullptr);
    ~DatabaseConnectionDialog();

    QString server() const;
    QString database() const;
    QString username() const;
    QString password() const;
    int port() const;

private slots:
    void onTestConnection();
    void onConnect();
    void onCancel();

private:
    void setupUI();
    void loadSavedSettings();
    void saveSettings();

private:
    QLineEdit* serverEdit_;
    QLineEdit* databaseEdit_;
    QLineEdit* usernameEdit_;
    QLineEdit* passwordEdit_;
    QSpinBox* portSpinBox_;
    QPushButton* testButton_;
    QPushButton* connectButton_;
    QPushButton* cancelButton_;
    QLabel* statusLabel_;
};

#endif // DATABASECONNECTIONDIALOG_H
