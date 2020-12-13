#pragma once

#include "../ElectricalElement.h"

// Класс электрического нагревателя
class ElectricHeater : public ElectricalElement
{
public:
    // Конструктор, принимающий позицию центра, ориентацию и сопротивление
    ElectricHeater(QPoint location, Qt::Orientation orientation, double resistance);

    // Обновление свойств электронагревателя по списку
    bool updateFromProperties(const QStringList& properties);
    // Заполнение таблицы свойств
    void fillPropertiesTable(QTableWidget* tw) const;
    // Запись электронагревателя в JSON-документ
    void writeJson(QJsonObject& json) const;

    // Отрисовка электронагревателя в нужном состоянии
    void render(QPainter& painter, RenderingState state) const;

    // Обновление состояния электронагревателя при прохождении через него тока
    void onCurrentFlow(double current) { activated = current >= 0.1; }

private:
    // Активация при нагреве
    bool activated;
};

// Фабрика для электронагревателей
class ElectricHeaterFactory : public ElectricalElementFactory
{
public:
    // Создание электронагревателя по позиции, ориентации и списку свойств
    ElectricalElement* create(QPoint location, Qt::Orientation orientation, const QStringList& properties) const;
    // Создание электронагревателя из JSON-объекта
    ElectricalElement* readJsonAndCreate(const QJsonObject& json) const;
};