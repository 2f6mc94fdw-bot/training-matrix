#include "ChartWidget.h"
#include <QPainter>
#include <QPaintEvent>
#include <QFontMetrics>
#include <QtMath>

ChartWidget::ChartWidget(QWidget* parent)
    : QWidget(parent)
    , chartType_(BarChart)
{
    // Default color palette
    colors_ = {
        QColor("#3498db"),  // Blue
        QColor("#2ecc71"),  // Green
        QColor("#e74c3c"),  // Red
        QColor("#f39c12"),  // Orange
        QColor("#9b59b6"),  // Purple
        QColor("#1abc9c"),  // Turquoise
        QColor("#34495e"),  // Dark gray
        QColor("#e67e22")   // Carrot
    };

    setMinimumSize(300, 250);
}

ChartWidget::~ChartWidget() {}

void ChartWidget::setChartType(ChartType type)
{
    chartType_ = type;
    update();
}

void ChartWidget::setData(const QMap<QString, double>& data)
{
    data_ = data;
    update();
}

void ChartWidget::setTitle(const QString& title)
{
    title_ = title;
    update();
}

void ChartWidget::setColors(const QList<QColor>& colors)
{
    colors_ = colors;
    update();
}

void ChartWidget::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    QRect rect = event->rect();

    // Draw title if set
    if (!title_.isEmpty()) {
        QFont titleFont = painter.font();
        titleFont.setPointSize(12);
        titleFont.setBold(true);
        painter.setFont(titleFont);

        painter.drawText(rect.adjusted(10, 10, -10, 0), Qt::AlignTop | Qt::AlignHCenter, title_);

        // Adjust rect to exclude title area
        rect.adjust(0, 40, 0, 0);
    }

    // Draw chart based on type
    if (data_.isEmpty()) {
        painter.drawText(rect, Qt::AlignCenter, "No data to display");
        return;
    }

    switch (chartType_) {
        case BarChart:
            drawBarChart(painter, rect);
            break;
        case PieChart:
            drawPieChart(painter, rect);
            break;
        case HorizontalBarChart:
            drawHorizontalBarChart(painter, rect);
            break;
    }
}

void ChartWidget::drawBarChart(QPainter& painter, const QRect& rect)
{
    if (data_.isEmpty()) return;

    int margin = 40;
    QRect chartRect = rect.adjusted(margin, margin, -margin, -margin);

    // Find max value
    double maxValue = 0;
    for (auto value : data_.values()) {
        maxValue = qMax(maxValue, value);
    }

    if (maxValue == 0) maxValue = 1; // Avoid division by zero

    // Calculate bar width
    int barCount = data_.size();
    int barWidth = (chartRect.width() - (barCount + 1) * 10) / barCount;
    barWidth = qMax(20, qMin(barWidth, 100));

    int x = chartRect.left() + 10;
    int colorIndex = 0;

    // Draw axes
    painter.setPen(QPen(Qt::gray, 2));
    painter.drawLine(chartRect.bottomLeft(), chartRect.bottomRight());
    painter.drawLine(chartRect.bottomLeft(), chartRect.topLeft());

    // Draw bars
    QMapIterator<QString, double> it(data_);
    while (it.hasNext()) {
        it.next();

        double value = it.value();
        int barHeight = static_cast<int>((value / maxValue) * (chartRect.height() - 40));

        QRect barRect(x, chartRect.bottom() - barHeight, barWidth, barHeight);

        // Draw bar
        QColor color = colors_[colorIndex % colors_.size()];
        painter.fillRect(barRect, color);
        painter.setPen(Qt::black);
        painter.drawRect(barRect);

        // Draw value on top
        painter.drawText(barRect.adjusted(0, -20, 0, 0),
                        Qt::AlignCenter | Qt::AlignBottom,
                        QString::number(value, 'f', 1));

        // Draw label
        QFont labelFont = painter.font();
        labelFont.setPointSize(8);
        painter.setFont(labelFont);

        // Rotate and draw label
        painter.save();
        painter.translate(x + barWidth / 2, chartRect.bottom() + 5);
        painter.rotate(-45);
        painter.drawText(0, 0, it.key());
        painter.restore();

        x += barWidth + 10;
        colorIndex++;
    }
}

void ChartWidget::drawPieChart(QPainter& painter, const QRect& rect)
{
    if (data_.isEmpty()) return;

    int margin = 20;
    int legendMargin = 150;
    QRect chartRect = rect.adjusted(margin, margin, -legendMargin, -margin);

    // Calculate total
    double total = 0;
    for (auto value : data_.values()) {
        total += value;
    }

    if (total == 0) total = 1; // Avoid division by zero

    // Calculate circle rect (make it square)
    int size = qMin(chartRect.width(), chartRect.height());
    QRect circleRect(chartRect.left(), chartRect.top(), size, size);

    // Center the circle
    circleRect.moveCenter(chartRect.center());

    // Draw pie slices
    int startAngle = 0;
    int colorIndex = 0;

    QMapIterator<QString, double> it(data_);
    int legendY = chartRect.top();

    while (it.hasNext()) {
        it.next();

        double value = it.value();
        int spanAngle = static_cast<int>((value / total) * 360 * 16); // Qt uses 1/16th degrees

        QColor color = colors_[colorIndex % colors_.size()];

        // Draw pie slice
        painter.setBrush(color);
        painter.setPen(Qt::white);
        painter.drawPie(circleRect, startAngle, spanAngle);

        // Draw legend
        QRect legendColorRect(rect.right() - legendMargin + 10, legendY, 15, 15);
        painter.fillRect(legendColorRect, color);
        painter.setPen(Qt::black);
        painter.drawRect(legendColorRect);

        QFont legendFont = painter.font();
        legendFont.setPointSize(9);
        painter.setFont(legendFont);

        QString legendText = QString("%1 (%2%)").arg(it.key())
            .arg(QString::number(value / total * 100, 'f', 1));
        painter.drawText(legendColorRect.right() + 5, legendY + 12, legendText);

        startAngle += spanAngle;
        legendY += 25;
        colorIndex++;
    }
}

void ChartWidget::drawHorizontalBarChart(QPainter& painter, const QRect& rect)
{
    if (data_.isEmpty()) return;

    int margin = 40;
    int labelWidth = 150;
    QRect chartRect = rect.adjusted(labelWidth, margin, -margin, -margin);

    // Find max value
    double maxValue = 0;
    for (auto value : data_.values()) {
        maxValue = qMax(maxValue, value);
    }

    if (maxValue == 0) maxValue = 1;

    // Calculate bar height
    int barCount = data_.size();
    int barHeight = (chartRect.height() - (barCount + 1) * 10) / barCount;
    barHeight = qMax(20, qMin(barHeight, 60));

    int y = chartRect.top() + 10;
    int colorIndex = 0;

    // Draw bars
    QMapIterator<QString, double> it(data_);
    while (it.hasNext()) {
        it.next();

        double value = it.value();
        int barWidth = static_cast<int>((value / maxValue) * (chartRect.width() - 60));

        QRect barRect(chartRect.left(), y, barWidth, barHeight);

        // Draw bar
        QColor color = colors_[colorIndex % colors_.size()];
        painter.fillRect(barRect, color);
        painter.setPen(Qt::black);
        painter.drawRect(barRect);

        // Draw value
        painter.drawText(barRect.adjusted(0, 0, 50, 0),
                        Qt::AlignLeft | Qt::AlignVCenter,
                        "  " + QString::number(value, 'f', 1));

        // Draw label on left
        painter.drawText(QRect(margin, y, labelWidth - 10, barHeight),
                        Qt::AlignRight | Qt::AlignVCenter,
                        it.key());

        y += barHeight + 10;
        colorIndex++;
    }
}
