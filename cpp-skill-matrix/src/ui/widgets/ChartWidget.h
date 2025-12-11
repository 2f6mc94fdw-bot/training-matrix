#ifndef CHARTWIDGET_H
#define CHARTWIDGET_H

#include <QWidget>

class ChartWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ChartWidget(QWidget* parent = nullptr);
    ~ChartWidget();
};

#endif // CHARTWIDGET_H
