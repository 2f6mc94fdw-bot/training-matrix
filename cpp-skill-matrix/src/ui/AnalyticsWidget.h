#ifndef ANALYTICSWIDGET_H
#define ANALYTICSWIDGET_H

#include <QWidget>

class AnalyticsWidget : public QWidget
{
    Q_OBJECT

public:
    explicit AnalyticsWidget(QWidget* parent = nullptr);
    ~AnalyticsWidget();
};

#endif // ANALYTICSWIDGET_H
