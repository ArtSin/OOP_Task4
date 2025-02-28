#pragma once

#include "../ElectricalElement.h"

// Класс провода
class Wire : public ElectricalElement {
  public:
    // Конструктор, принимающий позицию центра и ориентацию
    Wire(QPoint location, Qt::Orientation orientation);

    // Обновление свойств провода по списку
    bool updateFromProperties(const QStringList &) { return true; }
    // Заполнение таблицы свойств
    void fillPropertiesTable(QTableWidget *tw) const;
    // Запись провода в JSON-документ
    void writeJson(QJsonObject &json) const;

    // Отрисовка элемента в нужном состоянии
    void render(QPainter &painter, RenderingState state) const;
};

// Фабрика для проводов
class WireFactory : public ElectricalElementFactory {
  public:
    // Создание провода по позиции, ориентации и списку свойств
    ElectricalElement *create(QPoint location, Qt::Orientation orientation,
                              const QStringList &properties) const;
    // Создание провода из JSON-объекта
    ElectricalElement *readJsonAndCreate(const QJsonObject &json) const;
};