#ifndef CHARTWIDGET_H
#define CHARTWIDGET_H

#include <QWidget>
#include <QMap>
#include <QString>
#include <QColor>

/**
 * @brief Simple chart widget for visualizing data
 *
 * Provides basic bar chart and pie chart visualization using QPainter.
 * For production use, consider using Qt Charts library for more advanced features.
 */
class ChartWidget : public QWidget
{
    Q_OBJECT

public:
    enum ChartType {
        BarChart,
        PieChart,
        HorizontalBarChart
    };

    explicit ChartWidget(QWidget* parent = nullptr);
    ~ChartWidget();

    /**
     * @brief Set the chart type
     */
    void setChartType(ChartType type);

    /**
     * @brief Set chart data
     * @param data Map of labels to values
     */
    void setData(const QMap<QString, double>& data);

    /**
     * @brief Set chart title
     */
    void setTitle(const QString& title);

    /**
     * @brief Set color palette
     */
    void setColors(const QList<QColor>& colors);

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    void drawBarChart(QPainter& painter, const QRect& rect);
    void drawPieChart(QPainter& painter, const QRect& rect);
    void drawHorizontalBarChart(QPainter& painter, const QRect& rect);

    ChartType chartType_;
    QMap<QString, double> data_;
    QString title_;
    QList<QColor> colors_;
};

#endif // CHARTWIDGET_H
