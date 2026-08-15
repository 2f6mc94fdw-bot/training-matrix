#ifndef NOTIFICATIONSWIDGET_H
#define NOTIFICATIONSWIDGET_H

#include <QWidget>

class QTableWidget;
class QPushButton;
class QLabel;
class QComboBox;
class QLineEdit;
class QCheckBox;

#include "../database/NotificationRepository.h"

class NotificationsWidget : public QWidget
{
    Q_OBJECT

public:
    explicit NotificationsWidget(QWidget* parent = nullptr);
    ~NotificationsWidget();

public slots:
    void refresh();

private slots:
    void onRefreshClicked();
    void onMarkReadClicked();
    void onFiltersChanged();

private:
    void setupUI();
    void loadNotifications();
    void applyFilters();

private:
    QTableWidget* table_;
    QPushButton* refreshButton_;
    QPushButton* markReadButton_;
    QLabel* summaryLabel_;
    QComboBox* typeFilterCombo_;
    QLineEdit* searchEdit_;
    QCheckBox* unreadOnlyCheck_;

    NotificationRepository notificationRepo_;
    QList<EngineerNotification> allNotifications_;
};

#endif // NOTIFICATIONSWIDGET_H
