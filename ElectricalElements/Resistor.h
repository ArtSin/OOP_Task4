#pragma once

#include "../ElectricalElement.h"

// Класс резистора
class Resistor : public ElectricalElement {
  public:
    // Конструктор, принимающий позицию центра, ориентацию и сопротивление
    Resistor(QPoint location, Qt::Orientation orientation, double resistance);

    // Обновление свойств резистора по списку
    bool updateFromProperties(const QStringList &properties);
    // Заполнение таблицы свойств
    void fillPropertiesTable(QTableWidget *tw) const;
    // Запись резистора в JSON-документ
    void writeJson(QJsonObject &json) const;

    // Отрисовка резистора в нужном состоянии
    void render(QPainter &painter, RenderingState state) const;
};

// Фабрика для резисторов
class ResistorFactory : public ElectricalElementFactory {
  public:
    // Создание резистора по позиции, ориентации и списку свойств
    ElectricalElement *create(QPoint location, Qt::Orientation orientation,
                              const QStringList &properties) const;
    // Создание резистора из JSON-объекта
    ElectricalElement *readJsonAndCreate(const QJsonObject &json) const;
};