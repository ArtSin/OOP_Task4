#include "CurrentSource.h"

// Конструктор, принимающий позицию центра, ориентацию, сопротивление и ток
CurrentSource::CurrentSource(QPoint location, Qt::Orientation orientation,
                             double resistance, double current)
    : ElectricalElement(location, orientation, resistance), current(current) {}

// Обновление свойств источника тока по списку
bool CurrentSource::updateFromProperties(const QStringList &properties) {
    // Ток
    bool ok;
    double newCurrent = properties[0].toDouble(&ok);
    if (!ok || newCurrent <= 0)
        return false;
    // Сопротивление
    double newResistance = properties[1].toDouble(&ok);
    if (!ok || newResistance <= 0)
        return false;
    // Если корректно, то установка нового тока и сопротивления
    current = newCurrent;
    resistance = newResistance;
    return true;
}

// Заполнение таблицы свойств
void CurrentSource::fillPropertiesTable(QTableWidget *tw) const {
    // Создание 2 столбцов и 2 строк
    ElectricalElement::fillPropertiesTable(tw);
    tw->setRowCount(2);

    // Строка для тока
    auto item00 = new QTableWidgetItem(QString(u8"Ток (А)"));
    item00->setFlags(item00->flags() & ~Qt::ItemFlag::ItemIsEditable);
    tw->setItem(0, 0, item00);
    auto item01 = new QTableWidgetItem(QString::number(current));
    item01->setFlags(item01->flags() | Qt::ItemFlag::ItemIsEditable);
    tw->setItem(0, 1, item01);

    // Строка для сопротивления
    auto item10 =
        new QTableWidgetItem(QString(u8"Внутреннее сопротивление (Ом)"));
    item10->setFlags(item10->flags() & ~Qt::ItemFlag::ItemIsEditable);
    tw->setItem(1, 0, item10);
    auto item11 = new QTableWidgetItem(QString::number(resistance));
    item11->setFlags(item11->flags() | Qt::ItemFlag::ItemIsEditable);
    tw->setItem(1, 1, item11);
}

// Запись источника тока в JSON-документ
void CurrentSource::writeJson(QJsonObject &json) const {
    json["type"] = "currentSource";
    json["x"] = location.x();
    json["y"] = location.y();
    json["orientation"] =
        (orientation == Qt::Horizontal) ? "horizontal" : "vertical";
    json["current"] = current;
    json["resistance"] = resistance;
}

// Отрисовка источника тока в нужном состоянии
void CurrentSource::render(QPainter &painter, RenderingState state) const {
    // Переход к левому/верхнему краю и горизонтальной ориентации
    if (orientation == Qt::Horizontal)
        painter.translate(-RenderArea::GRID_POINTS_DISTANCE, 0);
    else {
        painter.translate(0, -RenderArea::GRID_POINTS_DISTANCE);
        painter.rotate(90.0);
    }

    // Провода по краям источника тока
    painter.drawRect(0, -1, RenderArea::GRID_POINTS_DISTANCE / 2 + 1, 2);
    painter.drawRect(3 * RenderArea::GRID_POINTS_DISTANCE / 2, -1,
                     RenderArea::GRID_POINTS_DISTANCE / 2 + 1, 2);

    // Основная часть - круг
    painter.save();
    painter.setBrush(RenderArea::WHITE_BRUSH);
    painter.setPen(RenderArea::findPen(state));
    painter.drawEllipse(RenderArea::GRID_POINTS_DISTANCE / 2 + 1,
                        -RenderArea::GRID_POINTS_DISTANCE / 2 + 1,
                        RenderArea::GRID_POINTS_DISTANCE - 2,
                        RenderArea::GRID_POINTS_DISTANCE - 2);
    painter.restore();

    // Основная часть стрелки
    painter.drawRect(RenderArea::GRID_POINTS_DISTANCE / 2 + 4, -1,
                     RenderArea::GRID_POINTS_DISTANCE - 8, 2);
    // Концы стрелки
    painter.save();
    painter.translate(RenderArea::GRID_POINTS_DISTANCE / 2 + 5, 0);
    painter.rotate(45);
    painter.drawRect(0, -1, RenderArea::GRID_POINTS_DISTANCE / 4, 2);
    painter.rotate(-90);
    painter.drawRect(0, -1, RenderArea::GRID_POINTS_DISTANCE / 4, 2);
    painter.restore();
}

// Создание источника тока по позиции, ориентации и списку свойств
ElectricalElement *
CurrentSourceFactory::create(QPoint location, Qt::Orientation orientation,
                             const QStringList &properties) const {
    // Ток
    bool ok;
    double current = properties[0].toDouble(&ok);
    if (!ok || current <= 0)
        return nullptr;
    // Сопротивление
    double resistance = properties[1].toDouble(&ok);
    if (!ok || resistance <= 0)
        return nullptr;
    // Создание
    return new CurrentSource(location, orientation, resistance, current);
}

// Создание источника тока из JSON-объекта
ElectricalElement *
CurrentSourceFactory::readJsonAndCreate(const QJsonObject &json) const {
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

    // Ток
    if (!json.contains("current") || !json["current"].isDouble())
        return nullptr;
    double current = json["current"].toDouble();
    if (current <= 0)
        return nullptr;

    // Сопротивление
    if (!json.contains("resistance") || !json["resistance"].isDouble())
        return nullptr;
    double resistance = json["resistance"].toDouble();
    if (resistance <= 0)
        return nullptr;

    // Создание
    return new CurrentSource(QPoint(x, y), orientation, resistance, current);
}