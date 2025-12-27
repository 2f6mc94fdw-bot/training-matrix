#ifndef APTITUDELOGOWIDGET_H
#define APTITUDELOGOWIDGET_H

#include <QWidget>
#include <QPainter>
#include <QPolygonF>

class AptitudeLogoWidget : public QWidget
{
    Q_OBJECT

public:
    explicit AptitudeLogoWidget(QWidget* parent = nullptr);
    ~AptitudeLogoWidget() = default;

    void setSize(int size);

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    void drawHexagon(QPainter& painter, const QPointF& center, double size, const QColor& color, double zOffset = 0);
    QPolygonF createHexagon(const QPointF& center, double size);

    int logoSize_;
};

#endif // APTITUDELOGOWIDGET_H
