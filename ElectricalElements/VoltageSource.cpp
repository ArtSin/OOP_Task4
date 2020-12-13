#include "VoltageSource.h"

// Конструктор, принимающий позицию центра, ориентацию, сопротивление и напряжение
VoltageSource::VoltageSource(QPoint location, Qt::Orientation orientation, double resistance, double voltage)
    : ElectricalElement(location, orientation, resistance), voltage(voltage)
{
}

// Обновление свойств источника напряжения по списку
bool VoltageSource::updateFromProperties(const QStringList& properties)
{
    // Напряжение
    bool ok;
    double newVoltage = properties[0].toDouble(&ok);
    if (!ok || newVoltage <= 0)
        return false;
    // Сопротивление
    double newResistance = properties[1].toDouble(&ok);
    if (!ok || newResistance < 0)
        return false;
    // Если корректно, то установка нового напряжения и сопротивления
    voltage = newVoltage;
    resistance = newResistance;
    return true;
}

// Заполнение таблицы свойств
void VoltageSource::fillPropertiesTable(QTableWidget* tw) const
{
    // Создание 2 столбцов и 2 строк
    ElectricalElement::fillPropertiesTable(tw);
    tw->setRowCount(2);

    // Строка для напряжения
    auto item00 = new QTableWidgetItem(QString(u8"Напряжение (В)"));
    item00->setFlags(item00->flags() & ~Qt::ItemFlag::ItemIsEditable);
    tw->setItem(0, 0, item00);
    auto item01 = new QTableWidgetItem(QString::number(voltage));
    item01->setFlags(item01->flags() | Qt::ItemFlag::ItemIsEditable);
    tw->setItem(0, 1, item01);

    // Строка для сопротивления
    auto item10 = new QTableWidgetItem(QString(u8"Внутреннее сопротивление (Ом)"));
    item10->setFlags(item10->flags() & ~Qt::ItemFlag::ItemIsEditable);
    tw->setItem(1, 0, item10);
    auto item11 = new QTableWidgetItem(QString::number(resistance));
    item11->setFlags(item11->flags() | Qt::ItemFlag::ItemIsEditable);
    tw->setItem(1, 1, item11);
}

// Запись источника напряжения в JSON-документ
void VoltageSource::writeJson(QJsonObject& json) const
{
    json["type"] = "voltageSource";
    json["x"] = location.x();
    json["y"] = location.y();
    json["orientation"] = (orientation == Qt::Horizontal) ? "horizontal" : "vertical";
    json["voltage"] = voltage;
    json["resistance"] = resistance;
}

// Отрисовка источника напряжения в нужном состоянии
void VoltageSource::render(QPainter& painter, RenderingState state) const
{
    // Переход к левому/верхнему краю и горизонтальной ориентации
    if (orientation == Qt::Horizontal)
        painter.translate(-RenderArea::GRID_POINTS_DISTANCE, 0);
    else
    {
        painter.translate(0, -RenderArea::GRID_POINTS_DISTANCE);
        painter.rotate(90.0);
    }

    // Провода по краям источника напряжения
    painter.drawRect(0, -1, RenderArea::GRID_POINTS_DISTANCE / 2 + 1, 2);
    painter.drawRect(3 * RenderArea::GRID_POINTS_DISTANCE / 2, -1, RenderArea::GRID_POINTS_DISTANCE / 2 + 1, 2);

    // Основная часть - круг
    painter.save();
    painter.setBrush(RenderArea::WHITE_BRUSH);
    painter.setPen(RenderArea::findPen(state));
    painter.drawEllipse(RenderArea::GRID_POINTS_DISTANCE / 2 + 1, -RenderArea::GRID_POINTS_DISTANCE / 2 + 1,
        RenderArea::GRID_POINTS_DISTANCE - 2, RenderArea::GRID_POINTS_DISTANCE - 2);
    painter.restore();

    // Знак "+"
    painter.drawRect(RenderArea::GRID_POINTS_DISTANCE / 2 + 4, -1,
        RenderArea::GRID_POINTS_DISTANCE / 4, 2);
    painter.drawRect(5 * RenderArea::GRID_POINTS_DISTANCE / 8 + 3, -RenderArea::GRID_POINTS_DISTANCE / 8,
        2, RenderArea::GRID_POINTS_DISTANCE / 4);

    // Знак "-"
    if (orientation == Qt::Horizontal)
        painter.drawRect(10 * RenderArea::GRID_POINTS_DISTANCE / 8 - 4, -1,
            RenderArea::GRID_POINTS_DISTANCE / 4, 2);
    else
        painter.drawRect(10 * RenderArea::GRID_POINTS_DISTANCE / 8, -RenderArea::GRID_POINTS_DISTANCE / 8,
            2, RenderArea::GRID_POINTS_DISTANCE / 4);
}

// Создание источника напряжения по позиции, ориентации и списку свойств
ElectricalElement* VoltageSourceFactory::create(QPoint location, Qt::Orientation orientation, const QStringList& properties) const
{
    // Напряжение
    bool ok;
    double voltage = properties[0].toDouble(&ok);
    if (!ok || voltage <= 0)
        return nullptr;
    // Сопротивление
    double resistance = properties[1].toDouble(&ok);
    if (!ok || resistance < 0)
        return nullptr;
    // Создание
    return new VoltageSource(location, orientation, resistance, voltage);
}

// Создание источника напряжения из JSON-объекта
ElectricalElement* VoltageSourceFactory::readJsonAndCreate(const QJsonObject& json) const
{
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
    Qt::Orientation orientation = (orientationStr == "horizontal") ? Qt::Horizontal : Qt::Vertical;

    // Напряжение
    if (!json.contains("voltage") || !json["voltage"].isDouble())
        return nullptr;
    double voltage = json["voltage"].toDouble();
    if (voltage <= 0)
        return nullptr;

    // Сопротивление
    if (!json.contains("resistance") || !json["resistance"].isDouble())
        return nullptr;
    double resistance = json["resistance"].toDouble();
    if (resistance < 0)
        return nullptr;

    // Создание
    return new VoltageSource(QPoint(x, y), orientation, resistance, voltage);
}