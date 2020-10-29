#include <QPainter>
#include "RenderArea.h"

RenderArea::RenderArea(QWidget* parent) : QWidget(parent)
{
    setBackgroundRole(QPalette::Base);
    setAutoFillBackground(true);
}

void RenderArea::paintEvent(QPaintEvent* e)
{
    QPainter painter(this);
    QBrush brush(QColor::fromRgb(0, 0, 0));
    QPen pen(brush, 1.0);
    painter.setBrush(brush);
    painter.setPen(pen);

    QRect rect(10, 10, 30, 20);
    painter.drawRect(rect);


}