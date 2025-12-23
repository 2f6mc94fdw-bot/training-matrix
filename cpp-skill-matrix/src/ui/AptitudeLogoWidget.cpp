#include "AptitudeLogoWidget.h"
#include <QPainter>
#include <QPainterPath>
#include <QLinearGradient>
#include <QtMath>

AptitudeLogoWidget::AptitudeLogoWidget(QWidget* parent)
    : QWidget(parent)
    , logoSize_(160)
{
    setMinimumSize(logoSize_, logoSize_ + 30);
    setMaximumSize(logoSize_, logoSize_ + 30);
}

void AptitudeLogoWidget::setSize(int size)
{
    logoSize_ = size;
    setMinimumSize(logoSize_, logoSize_ + 30);
    setMaximumSize(logoSize_, logoSize_ + 30);
    update();
}

QPolygonF AptitudeLogoWidget::createHexagon(const QPointF& center, double size)
{
    QPolygonF hexagon;
    for (int i = 0; i < 6; ++i) {
        double angle = M_PI / 3.0 * i - M_PI / 6.0; // Start from top
        double x = center.x() + size * qCos(angle);
        double y = center.y() + size * qSin(angle);
        hexagon << QPointF(x, y);
    }
    return hexagon;
}

void AptitudeLogoWidget::drawHexagon(QPainter& painter, const QPointF& center, double size, const QColor& color, double zOffset)
{
    QPolygonF hexagon = createHexagon(center, size);

    // Apply 3D offset for depth
    if (zOffset != 0) {
        for (int i = 0; i < hexagon.size(); ++i) {
            hexagon[i] = QPointF(hexagon[i].x() + zOffset, hexagon[i].y() - zOffset);
        }
    }

    painter.setBrush(color);
    painter.setPen(Qt::NoPen);
    painter.drawPolygon(hexagon);
}

void AptitudeLogoWidget::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    double hexSize = logoSize_ * 0.35;
    QPointF center(logoSize_ / 2.0, logoSize_ / 2.0);

    // Draw 3D hexagonal logo with gradient colors (back to front for depth)
    // Each layer slightly offset to create 3D effect

    // Back layers (darker, more offset)
    drawHexagon(painter, center, hexSize, QColor("#1E3A8A").darker(140), -8);  // Dark blue back
    drawHexagon(painter, center, hexSize, QColor("#FB923C").darker(120), -6);  // Orange
    drawHexagon(painter, center, hexSize, QColor("#10B981").darker(120), -4);  // Green
    drawHexagon(painter, center, hexSize, QColor("#22D3EE").darker(110), -2);  // Cyan

    // Front layer with gradient
    QLinearGradient gradient(center.x() - hexSize, center.y() - hexSize,
                             center.x() + hexSize, center.y() + hexSize);
    gradient.setColorAt(0.0, QColor("#60A5FA"));   // Light Blue
    gradient.setColorAt(0.33, QColor("#22D3EE"));  // Cyan
    gradient.setColorAt(0.66, QColor("#10B981"));  // Green
    gradient.setColorAt(1.0, QColor("#FB923C"));   // Orange

    QPolygonF frontHex = createHexagon(center, hexSize);
    painter.setBrush(gradient);
    painter.setPen(QPen(QColor(255, 255, 255, 50), 2));
    painter.drawPolygon(frontHex);

    // Add highlight for glossy 3D effect
    QRadialGradient highlight(center.x() - hexSize * 0.3, center.y() - hexSize * 0.3, hexSize * 0.8);
    highlight.setColorAt(0.0, QColor(255, 255, 255, 80));
    highlight.setColorAt(0.5, QColor(255, 255, 255, 20));
    highlight.setColorAt(1.0, QColor(255, 255, 255, 0));

    painter.setBrush(highlight);
    painter.setPen(Qt::NoPen);
    painter.drawPolygon(frontHex);

    // Draw "APTITUDE" text in white below the hexagon
    QFont font = painter.font();
    font.setFamily("Arial");
    font.setPointSize(logoSize_ / 10);
    font.setBold(true);
    font.setLetterSpacing(QFont::AbsoluteSpacing, 2);
    painter.setFont(font);

    painter.setPen(QColor(255, 255, 255));  // White text
    QRectF textRect(0, logoSize_ * 0.75, logoSize_, 30);
    painter.drawText(textRect, Qt::AlignCenter, "APTITUDE");
}
