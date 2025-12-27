#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStackedWidget>
#include <QListWidget>

class DashboardWidget;
class AnalyticsWidget;
class ImportExportDialog;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

protected:
    void closeEvent(QCloseEvent* event) override;

private slots:
    void onNavigationItemClicked(int index);
    void onThemeToggled();
    void onChangePasswordClicked();
    void onLogoutClicked();

private:
    void setupUI();
    void setupMenuBar();
    void setupNavigationSidebar();
    void setupCentralWidget();
    void setupStatusBar();
    void restoreSettings();
    void saveSettings();

private:
    QListWidget* navigationList_;
    QStackedWidget* contentStack_;

    // Widget placeholders
    DashboardWidget* dashboardWidget_;
    QWidget* engineersWidget_;
    QWidget* usersWidget_;
    QWidget* productionAreasWidget_;
    QWidget* assessmentWidget_;
    QWidget* coreSkillsWidget_;
    QWidget* reportsWidget_;
    AnalyticsWidget* analyticsWidget_;
    QWidget* certificationsWidget_;
    QWidget* snapshotsWidget_;
    QWidget* auditLogWidget_;
    ImportExportDialog* importExportWidget_;
};

#endif // MAINWINDOW_H
