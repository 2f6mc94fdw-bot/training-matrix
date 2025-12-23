#include "AptitudeLogoWidget.h"
#include <QPainter>
#include <QPainterPath>
#include <QLinearGradient>
#include <QtMath>

AptitudeLogoWidget::AptitudeLogoWidget(QWidget* parent)
    : QWidget(parent)
    , logoSize_(160)
{
    setMinimumSize(logoSize_, logoSize_ + 40);
    setMaximumSize(logoSize_, logoSize_ + 40);
}

void AptitudeLogoWidget::setSize(int size)
{
    logoSize_ = size;
    setMinimumSize(logoSize_, logoSize_ + 40);
    setMaximumSize(logoSize_, logoSize_ + 40);
    update();
}

QPolygonF AptitudeLogoWidget::createHexagon(const QPointF& center, double size)
{
    QPolygonF hexagon;
    for (int i = 0; i < 6; ++i) {
        double angle = M_PI / 3.0 * i - M_PI / 2.0; // Start from top
        double x = center.x() + size * qCos(angle);
        double y = center.y() + size * qSin(angle);
        hexagon << QPointF(x, y);
    }
    return hexagon;
}

void AptitudeLogoWidget::drawHexagon(QPainter& painter, const QPointF& center, double size, const QColor& color, double zOffset)
{
    Q_UNUSED(zOffset);
    QPolygonF hexagon = createHexagon(center, size);
    painter.setBrush(color);
    painter.setPen(Qt::NoPen);
    painter.drawPolygon(hexagon);
}

void AptitudeLogoWidget::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    double ribbonWidth = logoSize_ * 0.14;  // Width of each ribbon segment
    double hexRadius = logoSize_ * 0.28;    // Distance from center to vertices
    QPointF center(logoSize_ / 2.0, logoSize_ / 2.0 - 5);

    // Define the 6 segments with their colors (matching the provided logo)
    // Order: top, upper-right, lower-right, bottom, lower-left, upper-left
    QColor segmentColors[6] = {
        QColor("#6DB3F2"),  // Light blue (top)
        QColor("#1ABC9C"),  // Teal/cyan (upper-right)
        QColor("#F39C12"),  // Orange (lower-right)
        QColor("#E67E22"),  // Dark orange (bottom)
        QColor("#52B788"),  // Green (lower-left)
        QColor("#2E5090")   // Dark blue (upper-left)
    };

    // Draw each hexagon segment as a 3D ribbon piece
    for (int i = 0; i < 6; ++i) {
        double angle1 = M_PI / 3.0 * i - M_PI / 2.0;
        double angle2 = M_PI / 3.0 * (i + 1) - M_PI / 2.0;

        // Outer points
        QPointF outer1(center.x() + hexRadius * qCos(angle1),
                       center.y() + hexRadius * qSin(angle1));
        QPointF outer2(center.x() + hexRadius * qCos(angle2),
                       center.y() + hexRadius * qSin(angle2));

        // Inner points (for the ribbon effect)
        double innerRadius = hexRadius - ribbonWidth;
        QPointF inner1(center.x() + innerRadius * qCos(angle1),
                       center.y() + innerRadius * qSin(angle1));
        QPointF inner2(center.x() + innerRadius * qCos(angle2),
                       center.y() + innerRadius * qSin(angle2));

        // Create the ribbon segment with 3D effect
        QPainterPath segment;
        segment.moveTo(outer1);
        segment.lineTo(outer2);
        segment.lineTo(inner2);
        segment.lineTo(inner1);
        segment.closeSubpath();

        // Draw top face (lighter)
        QLinearGradient topGradient(outer1, inner1);
        topGradient.setColorAt(0.0, segmentColors[i].lighter(120));
        topGradient.setColorAt(1.0, segmentColors[i]);
        painter.fillPath(segment, topGradient);

        // Draw 3D depth/side faces for realism
        QPainterPath sideFace;
        double depthOffset = 3;
        sideFace.moveTo(outer1);
        sideFace.lineTo(outer1.x(), outer1.y() + depthOffset);
        sideFace.lineTo(outer2.x(), outer2.y() + depthOffset);
        sideFace.lineTo(outer2);
        sideFace.closeSubpath();

        painter.fillPath(sideFace, segmentColors[i].darker(130));
    }

    // Draw "APTITUDE" text in SOLID WHITE below the hexagon
    QFont font = painter.font();
    font.setFamily("Arial");
    font.setPointSize(logoSize_ / 9);
    font.setBold(true);
    font.setLetterSpacing(QFont::AbsoluteSpacing, 3);
    painter.setFont(font);

    painter.setPen(QColor(255, 255, 255));  // Solid white text
    QRectF textRect(0, logoSize_ * 0.72, logoSize_, 40);
    painter.drawText(textRect, Qt::AlignCenter, "APTITUDE");
}
