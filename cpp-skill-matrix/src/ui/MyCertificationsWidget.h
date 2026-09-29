#ifndef MYCERTIFICATIONSWIDGET_H
#define MYCERTIFICATIONSWIDGET_H

#include <QWidget>
#include <QTableWidget>
#include <QPushButton>
#include <QLabel>
#include "../database/CertificationRepository.h"

class QShowEvent;

class MyCertificationsWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MyCertificationsWidget(const QString& engineerId, QWidget* parent = nullptr);
    ~MyCertificationsWidget();

protected:
    void showEvent(QShowEvent* event) override;

private slots:
    void onRefreshClicked();
    void onUploadClicked();
    void onOpenClicked();

private:
    void setupUI();
    void loadCertifications();

private:
    QString engineerId_;
    bool isFirstShow_;
    QLabel* summaryLabel_;
    QTableWidget* certificationsTable_;
    QPushButton* uploadButton_;
    QPushButton* openButton_;
    QPushButton* refreshButton_;
    CertificationRepository certificationRepo_;
};

#endif // MYCERTIFICATIONSWIDGET_H
