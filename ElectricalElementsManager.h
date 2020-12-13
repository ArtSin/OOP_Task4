#pragma once

#include <array>
#include <QListWidget>
#include <QTableWidget>
#include "RenderArea.h"

// Оператор "меньше" для точек
bool operator<(const QPoint& a, const QPoint& b);

// Класс управления электрическими элементами и схемой
class ElectricalElementsManager
{
public:
    // Получение единственного экземпляра класса
    static ElectricalElementsManager& getInstance()
    {
        static ElectricalElementsManager instance;
        return instance;
    }
    // Деструктор
    ~ElectricalElementsManager();

    // Получение элемента-образца по номеру и ориентации
    const ElectricalElement* getExampleElement(int i, Qt::Orientation orientation) const;
    // Заполнение списка элементами-образцами
    void fillElementsList(QListWidget* lw) const;
    // Заполнение таблицы свойств элементом-образцом с заданным номером
    void fillPropertiesTable(QTableWidget* tw, int i) const { exampleElements[i]->fillPropertiesTable(tw); }
    // Получение фабрики для элемента с заданным номером
    ElectricalElementFactory* getFactory(int i) const { return elementsFactories[i]; }

    // Добавление элемента в схему
    void addElement(ElectricalElement* element);
    // Получение элемента с заданной позицией и ориентацией
    ElectricalElement* getElement(const QPoint& loc, Qt::Orientation orientation);
    // Удаление элемента из схемы
    void removeElement(const QPoint& loc, Qt::Orientation orientation, bool deleteElement);
    // Получение всех элементов
    std::array<QMap<QPoint, ElectricalElement*>, 2>& getElements() { return elements; }
    // Удаление всех элементов
    void clearElements();

    // Чтение всех элементов из JSON-документа
    bool readElementsFromJson(const QJsonObject& json);
    // Запись всех элементов в JSON-документ
    void writeElementsToJson(QJsonObject& json) const;

private:
    // Конструктор
    ElectricalElementsManager();

    // Элементы-образцы с горизонтальной и вертикальной ориентацией
    QVector<ElectricalElement*> exampleElements;
    QVector<ElectricalElement*> exampleElementsVertical;
    // Названия элементов-образцов
    QStringList exampleElementsNames;
    // Изображения элементов-образцов
    QVector<QIcon*> exampleElementsIcons;
    // Фабрики элементов
    QVector<ElectricalElementFactory*> elementsFactories;
    // Номера элементов по их названию в JSON-документах
    QMap<QString, int> elementsJsonNames;
    // Все элементы схемы (по ориентации и позиции)
    std::array<QMap<QPoint, ElectricalElement*>, 2> elements;
};

