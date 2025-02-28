#pragma once

#include "../ElectricalElement.h"

// Класс источника тока
class CurrentSource : public ElectricalElement {
  public:
    // Конструктор, принимающий позицию центра, ориентацию, сопротивление и ток
    CurrentSource(QPoint location, Qt::Orientation orientation,
                  double resistance, double current);

    // Обновление свойств источника тока по списку
    bool updateFromProperties(const QStringList &properties);
    // Заполнение таблицы свойств
    void fillPropertiesTable(QTableWidget *tw) const;
    // Запись источника тока в JSON-документ
    void writeJson(QJsonObject &json) const;

    // Отрисовка источника тока в нужном состоянии
    void render(QPainter &painter, RenderingState state) const;

    // Получение тока
    double getCurrent() const { return current; }

  private:
    // Ток
    double current;
};

// Фабрика для источников тока
class CurrentSourceFactory : public ElectricalElementFactory {
  public:
    // Создание источника тока по позиции, ориентации и списку свойств
    ElectricalElement *create(QPoint location, Qt::Orientation orientation,
                              const QStringList &properties) const;
    // Создание источника тока из JSON-объекта
    ElectricalElement *readJsonAndCreate(const QJsonObject &json) const;
};