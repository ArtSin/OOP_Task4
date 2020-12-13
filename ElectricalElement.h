#pragma once

#include <QString>
#include <QPainter>
#include <QIcon>
#include <QTableWidget>
#include <QJsonObject>
#include "RenderArea.h"

enum class RenderingState;

// Класс электрического элемента
class ElectricalElement
{
public:
    // Конструктор, принимающий позицию центра, ориентацию и сопротивление
    ElectricalElement(QPoint location, Qt::Orientation orientation, double resistance);
    // Деструктор
    virtual ~ElectricalElement() {}

    // Обновление свойств элемента по списку
    virtual bool updateFromProperties(const QStringList& properties) = 0;
    // Заполнение таблицы свойств
    virtual void fillPropertiesTable(QTableWidget* tw) const;
    // Запись элемента в JSON-документ
    virtual void writeJson(QJsonObject& json) const = 0;

    // Геттеры и сеттеры для позиции и ориентации элемента
    QPoint getLocation() const { return location; }
    Qt::Orientation getOrientation() const { return orientation; }
    void setLocation(QPoint newLocation) { location = newLocation; }
    void setOrientation(Qt::Orientation newOrientation) { orientation = newOrientation; }
    // Получение крайних точек элемента
    virtual QVector<QPoint> getEndPoints() const;
    // Отрисовка элемента в нужном состоянии
    virtual void render(QPainter& painter, RenderingState state) const = 0;
    // Получение изображения элемента
    virtual QIcon* getIcon() const;

    // Геттер для сопротивления
    virtual double getResistance() const { return resistance; }
    // Обновление состояния элемента при прохождении через него тока
    virtual void onCurrentFlow(double current) { }

    // Оператор сравнения
    virtual bool operator==(const ElectricalElement& other) const;

protected:
    // Позиция центра
    QPoint location;
    // Ориентация элемента
    Qt::Orientation orientation;

    // Сопротивление элемента
    double resistance;
};

// Фабрика для электрических элементов
class ElectricalElementFactory
{
public:
    // Создание элемента по позиции, ориентации и списку свойств
    virtual ElectricalElement* create(QPoint location, Qt::Orientation orientation, const QStringList& properties) const = 0;
    // Создание элемента из JSON-объекта
    virtual ElectricalElement* readJsonAndCreate(const QJsonObject& json) const = 0;
};

