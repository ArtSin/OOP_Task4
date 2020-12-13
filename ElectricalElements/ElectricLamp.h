#pragma once

#include "../ElectricalElement.h"

// Класс электрической лампы
class ElectricLamp : public ElectricalElement
{
public:
    // Конструктор, принимающий позицию центра, ориентацию и сопротивление
    ElectricLamp(QPoint location, Qt::Orientation orientation, double resistance);

    // Обновление свойств лампы по списку
    bool updateFromProperties(const QStringList& properties);
    // Заполнение таблицы свойств
    void fillPropertiesTable(QTableWidget* tw) const;
    // Запись лампы в JSON-документ
    void writeJson(QJsonObject& json) const;

    // Отрисовка лампы в нужном состоянии
    void render(QPainter& painter, RenderingState state) const;

    // Обновление состояния лампы при прохождении через него тока
    void onCurrentFlow(double current) { activated = current >= 0.1; }

private:
    // Активация при нагреве
    bool activated;
};

// Фабрика для ламп
class ElectricLampFactory : public ElectricalElementFactory
{
public:
    // Создание лампы по позиции, ориентации и списку свойств
    ElectricalElement* create(QPoint location, Qt::Orientation orientation, const QStringList& properties) const;
    // Создание лампы из JSON-объекта
    ElectricalElement* readJsonAndCreate(const QJsonObject& json) const;
};