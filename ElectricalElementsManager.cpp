#include <QJsonArray>
#include "ElectricalElementsManager.h"
#include "ElectricalElements/Wire.h"
#include "ElectricalElements/Resistor.h"
#include "ElectricalElements/Switch.h"
#include "ElectricalElements/ElectricHeater.h"
#include "ElectricalElements/ElectricLamp.h"
#include "ElectricalElements/VoltageSource.h"
#include "ElectricalElements/CurrentSource.h"

// Оператор "меньше" для точек
bool operator<(const QPoint& a, const QPoint& b)
{
    // Сравнение по x, затем по y
    return (a.x() != b.x()) ? (a.x() < b.x()) : (a.y() < b.y());
}

// Конструктор
ElectricalElementsManager::ElectricalElementsManager()
{
    // Элементы-образцы с горизонтальной ориентацией
    exampleElements.push_back(new Wire(QPoint(), Qt::Horizontal));
    exampleElements.push_back(new Resistor(QPoint(), Qt::Horizontal, 1.0));
    exampleElements.push_back(new Switch(QPoint(), Qt::Horizontal, false));
    exampleElements.push_back(new ElectricHeater(QPoint(), Qt::Horizontal, 1.0));
    exampleElements.push_back(new ElectricLamp(QPoint(), Qt::Horizontal, 1.0));
    exampleElements.push_back(new VoltageSource(QPoint(), Qt::Horizontal, 0.0, 1.0));
    exampleElements.push_back(new CurrentSource(QPoint(), Qt::Horizontal, std::numeric_limits<double>().infinity(), 1.0));

    // Элементы-образцы с вертикальной ориентацией
    exampleElementsVertical.push_back(new Wire(QPoint(), Qt::Vertical));
    exampleElementsVertical.push_back(new Resistor(QPoint(), Qt::Vertical, 1.0));
    exampleElementsVertical.push_back(new Switch(QPoint(), Qt::Vertical, false));
    exampleElementsVertical.push_back(new ElectricHeater(QPoint(), Qt::Vertical, 1.0));
    exampleElementsVertical.push_back(new ElectricLamp(QPoint(), Qt::Vertical, 1.0));
    exampleElementsVertical.push_back(new VoltageSource(QPoint(), Qt::Vertical, 0.0, 1.0));
    exampleElementsVertical.push_back(new CurrentSource(QPoint(), Qt::Vertical, std::numeric_limits<double>().infinity(), 1.0));

    // Названия элементов-образцов
    exampleElementsNames.push_back(QString(u8"Провод"));
    exampleElementsNames.push_back(QString(u8"Резистор"));
    exampleElementsNames.push_back(QString(u8"Выключатель"));
    exampleElementsNames.push_back(QString(u8"Электрический нагреватель"));
    exampleElementsNames.push_back(QString(u8"Электрическая лампа"));
    exampleElementsNames.push_back(QString(u8"Источник напряжения (ЭДС)"));
    exampleElementsNames.push_back(QString(u8"Источник тока"));

    // Получение изображений элементов-образцов
    for (auto el : exampleElements)
        exampleElementsIcons.push_back(el->getIcon());

    // Фабрики элементов
    elementsFactories.push_back(new WireFactory());
    elementsFactories.push_back(new ResistorFactory());
    elementsFactories.push_back(new SwitchFactory());
    elementsFactories.push_back(new ElectricHeaterFactory());
    elementsFactories.push_back(new ElectricLampFactory());
    elementsFactories.push_back(new VoltageSourceFactory());
    elementsFactories.push_back(new CurrentSourceFactory());

    // Номера элементов по их названию в JSON-документах
    elementsJsonNames["wire"] = 0;
    elementsJsonNames["resistor"] = 1;
    elementsJsonNames["switch"] = 2;
    elementsJsonNames["electricHeater"] = 3;
    elementsJsonNames["electricLamp"] = 4;
    elementsJsonNames["voltageSource"] = 5;
    elementsJsonNames["currentSource"] = 6;
}

// Деструктор
ElectricalElementsManager::~ElectricalElementsManager()
{
    // Уничтожение элементов-образцов с горизонтальной ориентацией
    for (auto el : exampleElements)
        delete el;
    // Уничтожение элементов-образцов с горизонтальной ориентацией
    for (auto el : exampleElementsVertical)
        delete el;
    // Уничтожение изображений элементов-образцов
    for (auto icon : exampleElementsIcons)
        delete icon;
    // Уничтожение фабрик элементов
    for (auto f : elementsFactories)
        delete f;
    // Уничтожение элементов
    for (int i = 0; i < 2; i++)
        for (auto el : elements[i])
            delete el;
}

// Получение элемента-образца по номеру и ориентации
const ElectricalElement* ElectricalElementsManager::getExampleElement(int i, Qt::Orientation orientation) const
{
    return (orientation == Qt::Horizontal) ? exampleElements[i] : exampleElementsVertical[i];
}

// Заполнение списка элементами-образцами
void ElectricalElementsManager::fillElementsList(QListWidget* lw) const
{
    // Очищение списка
    lw->clear();
    // Установка изображения
    lw->setIconSize(QSize(RenderArea::GRID_POINTS_DISTANCE * 2, RenderArea::GRID_POINTS_DISTANCE));
    // Добавление элементов-образцов
    for (int i = 0; i < exampleElements.size(); i++)
        lw->addItem(new QListWidgetItem(*exampleElementsIcons[i], exampleElementsNames[i]));
}

// Добавление элемента в схему
void ElectricalElementsManager::addElement(ElectricalElement* element)
{
    elements[element->getOrientation() - 1][element->getLocation()] = element;
}

// Получение элемента с заданной позицией и ориентацией
ElectricalElement* ElectricalElementsManager::getElement(const QPoint& loc, Qt::Orientation orientation)
{
    auto it = elements[orientation - 1].find(loc);
    return (it != elements[orientation - 1].end()) ? it.value() : nullptr;
}

// Удаление элемента из схемы
void ElectricalElementsManager::removeElement(const QPoint& loc, Qt::Orientation orientation, bool deleteElement)
{
    auto it = elements[orientation - 1].find(loc);
    if (it != elements[orientation - 1].end())
    {
        // Уничтожение удаляемого элемента
        if (deleteElement)
            delete it.value();
        elements[orientation - 1].erase(it);
    }
}

// Удаление всех элементов
void ElectricalElementsManager::clearElements()
{
    for (int i = 0; i < 2; i++)
    {
        // Уничтожение элементов
        for (auto el : elements[i])
            delete el;
        elements[i].clear();
    }
}

// Чтение всех элементов из JSON-документа
bool ElectricalElementsManager::readElementsFromJson(const QJsonObject& json)
{
    // Массив элементов
    if (!json.contains("elements") || !json["elements"].isArray())
        return false;
    auto elArr = json["elements"].toArray();

    // Удаление всех элементов
    clearElements();
    // Проход по массиву
    for (const auto& elObjRef : elArr)
    {
        // Получение объекта
        auto elObj = elObjRef.toObject();
        // Тип элемента
        if (!elObj.contains("type") || !elObj["type"].isString())
            return false;
        auto key = elObj["type"].toString();
        if (!elementsJsonNames.contains(key))
            return false;
        // Создание и добавление элемента
        auto el = elementsFactories[elementsJsonNames[key]]->readJsonAndCreate(elObj);
        addElement(el);
    }
    return true;
}

// Запись всех элементов в JSON-документ
void ElectricalElementsManager::writeElementsToJson(QJsonObject& json) const
{
    QJsonArray elArr;
    for (int i = 0; i < 2; i++)
        for (auto el : elements[i])
        {
            QJsonObject elObj;
            el->writeJson(elObj);
            elArr.append(elObj);
        }
    json["elements"] = elArr;
}