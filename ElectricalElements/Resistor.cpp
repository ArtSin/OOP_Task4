#include "Resistor.h"

// Конструктор, принимающий позицию центра, ориентацию и сопротивление
Resistor::Resistor(QPoint location, Qt::Orientation orientation,
                   double resistance)
    : ElectricalElement(location, orientation, resistance) {}

// Обновление свойств резистора по списку
bool Resistor::updateFromProperties(const QStringList &properties) {
    // Сопротивление
    bool ok;
    double newResistance = properties[0].toDouble(&ok);
    if (!ok || newResistance <= 0)
        return false;
    // Если корректно, то установка нового сопротивления
    resistance = newResistance;
    return true;
}

// Заполнение таблицы свойств
void Resistor::fillPropertiesTable(QTableWidget *tw) const {
    // Создание 2 столбцов и 1 строки
    ElectricalElement::fillPropertiesTable(tw);
    tw->setRowCount(1);

    // Строка для сопротивления
    auto item00 = new QTableWidgetItem(QString(u8"Сопротивление (Ом)"));
    item00->setFlags(item00->flags() & ~Qt::ItemFlag::ItemIsEditable);
    tw->setItem(0, 0, item00);
    auto item01 = new QTableWidgetItem(QString::number(resistance));
    item01->setFlags(item01->flags() | Qt::ItemFlag::ItemIsEditable);
    tw->setItem(0, 1, item01);
}

// Запись резистора в JSON-документ
void Resistor::writeJson(QJsonObject &json) const {
    json["type"] = "resistor";
    json["x"] = location.x();
    json["y"] = location.y();
    json["orientation"] =
        (orientation == Qt::Horizontal) ? "horizontal" : "vertical";
    json["resistance"] = resistance;
}

// Отрисовка резистора в нужном состоянии
void Resistor::render(QPainter &painter, RenderingState state) const {
    // Переход к левому/верхнему краю и горизонтальной ориентации
    if (orientation == Qt::Horizontal)
        painter.translate(-RenderArea::GRID_POINTS_DISTANCE, 0);
    else {
        painter.translate(0, -RenderArea::GRID_POINTS_DISTANCE);
        painter.rotate(90.0);
    }

    // Провода по краям резистора
    painter.drawRect(0, -1, 4 * RenderArea::GRID_POINTS_DISTANCE / 10, 2);
    painter.drawRect(16 * RenderArea::GRID_POINTS_DISTANCE / 10, -1,
                     4 * RenderArea::GRID_POINTS_DISTANCE / 10, 2);

    // Прямоугольник-резистор
    painter.setBrush(RenderArea::WHITE_BRUSH);
    painter.setPen(RenderArea::findPen(state));
    painter.drawRect(4 * RenderArea::GRID_POINTS_DISTANCE / 10,
                     -RenderArea::GRID_POINTS_DISTANCE / 4,
                     12 * RenderArea::GRID_POINTS_DISTANCE / 10,
                     RenderArea::GRID_POINTS_DISTANCE / 2);
}

// Создание резистора по позиции, ориентации и списку свойств
ElectricalElement *
ResistorFactory::create(QPoint location, Qt::Orientation orientation,
                        const QStringList &properties) const {
    // Сопротивление
    bool ok = false;
    double resistance = properties[0].toDouble(&ok);
    if (!ok || resistance <= 0)
        return nullptr;
    // Создание
    return new Resistor(location, orientation, resistance);
}

// Создание резистора из JSON-объекта
ElectricalElement *
ResistorFactory::readJsonAndCreate(const QJsonObject &json) const {
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

    // Сопротивление
    if (!json.contains("resistance") || !json["resistance"].isDouble())
        return nullptr;
    double resistance = json["resistance"].toDouble();
    if (resistance <= 0)
        return nullptr;

    // Создание
    return new Resistor(QPoint(x, y), orientation, resistance);
}