#include "ElectricHeater.h"

// Конструктор, принимающий позицию центра, ориентацию и сопротивление
ElectricHeater::ElectricHeater(QPoint location, Qt::Orientation orientation,
                               double resistance)
    : ElectricalElement(location, orientation, resistance), activated(false) {}

// Обновление свойств электронагревателя по списку
bool ElectricHeater::updateFromProperties(const QStringList &properties) {
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
void ElectricHeater::fillPropertiesTable(QTableWidget *tw) const {
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

// Запись электронагревателя в JSON-документ
void ElectricHeater::writeJson(QJsonObject &json) const {
    json["type"] = "electricHeater";
    json["x"] = location.x();
    json["y"] = location.y();
    json["orientation"] =
        (orientation == Qt::Horizontal) ? "horizontal" : "vertical";
    json["resistance"] = resistance;
}

// Отрисовка электронагревателя в нужном состоянии
void ElectricHeater::render(QPainter &painter, RenderingState state) const {
    // Переход к левому/верхнему краю и горизонтальной ориентации
    if (orientation == Qt::Horizontal)
        painter.translate(-RenderArea::GRID_POINTS_DISTANCE, 0);
    else {
        painter.translate(0, -RenderArea::GRID_POINTS_DISTANCE);
        painter.rotate(90.0);
    }

    // Провода по краям электронагревателя
    painter.drawRect(0, -1, 4 * RenderArea::GRID_POINTS_DISTANCE / 10, 2);
    painter.drawRect(16 * RenderArea::GRID_POINTS_DISTANCE / 10, -1,
                     4 * RenderArea::GRID_POINTS_DISTANCE / 10, 2);

    // Основная часть - прямоугольник (красный, если нагрет)
    painter.save();
    painter.setBrush(activated ? RenderArea::RED_BRUSH
                               : RenderArea::WHITE_BRUSH);
    painter.setPen(RenderArea::findPen(state));
    painter.drawRect(4 * RenderArea::GRID_POINTS_DISTANCE / 10,
                     -RenderArea::GRID_POINTS_DISTANCE / 4,
                     12 * RenderArea::GRID_POINTS_DISTANCE / 10,
                     RenderArea::GRID_POINTS_DISTANCE / 2);
    painter.restore();

    // Полоски
    for (int i = 0; i < 3; i++)
        painter.drawRect((7 + 3 * i) * RenderArea::GRID_POINTS_DISTANCE / 10 -
                             1,
                         -RenderArea::GRID_POINTS_DISTANCE / 4, 2,
                         RenderArea::GRID_POINTS_DISTANCE / 2);
}

// Создание электронагревателя по позиции, ориентации и списку свойств
ElectricalElement *
ElectricHeaterFactory::create(QPoint location, Qt::Orientation orientation,
                              const QStringList &properties) const {
    // Сопротивление
    bool ok = false;
    double resistance = properties[0].toDouble(&ok);
    if (!ok || resistance <= 0)
        return nullptr;
    // Создание
    return new ElectricHeater(location, orientation, resistance);
}

// Создание электронагревателя из JSON-объекта
ElectricalElement *
ElectricHeaterFactory::readJsonAndCreate(const QJsonObject &json) const {
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
    return new ElectricHeater(QPoint(x, y), orientation, resistance);
}