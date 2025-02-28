#pragma once

#include "../ElectricalElement.h"

// Класс источника напряжения
class VoltageSource : public ElectricalElement {
  public:
    // Конструктор, принимающий позицию центра, ориентацию, сопротивление и
    // напряжение
    VoltageSource(QPoint location, Qt::Orientation orientation,
                  double resistance, double voltage);

    // Обновление свойств источника напряжения по списку
    bool updateFromProperties(const QStringList &properties);
    // Заполнение таблицы свойств
    void fillPropertiesTable(QTableWidget *tw) const;
    // Запись источника напряжения в JSON-документ
    void writeJson(QJsonObject &json) const;

    // Отрисовка источника напряжения в нужном состоянии
    void render(QPainter &painter, RenderingState state) const;

    // Получение напряжения
    double getVoltage() const { return voltage; }

  private:
    // Напряжение
    double voltage;
};

// Фабрика для источников напряжения
class VoltageSourceFactory : public ElectricalElementFactory {
  public:
    // Создание источника напряжения по позиции, ориентации и списку свойств
    ElectricalElement *create(QPoint location, Qt::Orientation orientation,
                              const QStringList &properties) const;
    // Создание источника напряжения из JSON-объекта
    ElectricalElement *readJsonAndCreate(const QJsonObject &json) const;
};