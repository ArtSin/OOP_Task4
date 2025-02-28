#include "Wire.h"

// Конструктор, принимающий позицию центра и ориентацию
Wire::Wire(QPoint location, Qt::Orientation orientation)
    : ElectricalElement(location, orientation, 0.0) {}

// Заполнение таблицы свойств
void Wire::fillPropertiesTable(QTableWidget *tw) const {
    ElectricalElement::fillPropertiesTable(tw);
    tw->setRowCount(0);
}

// Запись провода в JSON-документ
void Wire::writeJson(QJsonObject &json) const {
    json["type"] = "wire";
    json["x"] = location.x();
    json["y"] = location.y();
    json["orientation"] =
        (orientation == Qt::Horizontal) ? "horizontal" : "vertical";
}

// Отрисовка провода в нужном состоянии
void Wire::render(QPainter &painter, RenderingState) const {
    // Переход к левому/верхнему краю и горизонтальной ориентации
    if (orientation == Qt::Horizontal)
        painter.translate(-RenderArea::GRID_POINTS_DISTANCE, 0);
    else {
        painter.translate(0, -RenderArea::GRID_POINTS_DISTANCE);
        painter.rotate(90.0);
    }

    // Очистка белым цветом пространства под проводом (для перпендикулярных)
    painter.save();
    painter.setBrush(RenderArea::WHITE_BRUSH);
    painter.drawRect(
        RenderArea::GRID_POINT_SIZE / 2, -RenderArea::GRID_POINT_SIZE / 2,
        2 * RenderArea::GRID_POINTS_DISTANCE - RenderArea::GRID_POINT_SIZE,
        RenderArea::GRID_POINT_SIZE);
    painter.restore();
    // Линия-провод
    painter.drawRect(0, -1, 2 * RenderArea::GRID_POINTS_DISTANCE, 2);
}

// Создание провода по позиции, ориентации и списку свойств
ElectricalElement *WireFactory::create(QPoint location,
                                       Qt::Orientation orientation,
                                       const QStringList &) const {
    return new Wire(location, orientation);
}

// Создание провода из JSON-объекта
ElectricalElement *
WireFactory::readJsonAndCreate(const QJsonObject &json) const {
    // Позиция
    if (!json.contains("x") || !json.contains("y"))
        return nullptr;
    int x = json["x"].toInt(-1), y = json["y"].toInt(-1);
    if (x < 0 || y < 0)
        return nullptr;

    // Ориентация
    if (!json.contains("orientation"))
        return nullptr;
    QString orientationStr = json["orientation"].toString("");
    if (orientationStr != "horizontal" && orientationStr != "vertical")
        return nullptr;
    Qt::Orientation orientation =
        (orientationStr == "horizontal") ? Qt::Horizontal : Qt::Vertical;

    // Создание
    return new Wire(QPoint(x, y), orientation);
}