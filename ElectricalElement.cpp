#include "ElectricalElement.h"

// Конструктор, принимающий позицию центра, ориентацию и сопротивление
ElectricalElement::ElectricalElement(QPoint location,
                                     Qt::Orientation orientation,
                                     double resistance)
    : location(location), orientation(orientation), resistance(resistance) {}

// Заполнение таблицы свойств
void ElectricalElement::fillPropertiesTable(QTableWidget *tw) const {
    // Очищение и создание 2 столбцов
    tw->clear();
    tw->setColumnCount(2);
    tw->setHorizontalHeaderItem(0, new QTableWidgetItem(QString(u8"Свойство")));
    tw->setHorizontalHeaderItem(1, new QTableWidgetItem(QString(u8"Значение")));
}

// Получение крайних точек элемента
QVector<QPoint> ElectricalElement::getEndPoints() const {
    if (orientation == Qt::Horizontal)
        return {location - QPoint(1, 0), location + QPoint(1, 0)};
    else
        return {location - QPoint(0, 1), location + QPoint(0, 1)};
}

// Получение изображения элемента
QIcon *ElectricalElement::getIcon() const {
    // Создание изображения и отрисовка в него элемента
    QPixmap pixmap(RenderArea::GRID_POINTS_DISTANCE * 2,
                   RenderArea::GRID_POINTS_DISTANCE);
    pixmap.fill();
    QPainter painter(&pixmap);
    painter.setRenderHint(QPainter::RenderHint::Antialiasing);
    painter.setBrush(RenderArea::BLACK_BRUSH);
    painter.setPen(Qt::NoPen);
    painter.translate(RenderArea::GRID_POINTS_DISTANCE,
                      RenderArea::GRID_POINTS_DISTANCE / 2);
    render(painter, RenderingState::Normal);
    return new QIcon(pixmap);
}

// Оператор сравнения
bool ElectricalElement::operator==(const ElectricalElement &other) const {
    // Сравнение по позиции и ориентации
    return location == other.location && orientation == other.orientation;
}