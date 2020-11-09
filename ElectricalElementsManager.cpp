#include <QJsonArray>
#include "ElectricalElementsManager.h"
#include "ElectricalElements/Wire.h"
#include "ElectricalElements/Resistor.h"
#include "ElectricalElements/Switch.h"
#include "ElectricalElements/ElectricHeater.h"

bool operator<(const QPoint& a, const QPoint& b)
{
	return (a.x() != b.x()) ? (a.x() < b.x()) : (a.y() < b.y());
}

ElectricalElementsManager::ElectricalElementsManager()
{
	exampleElements.push_back(new Wire(QPoint(), Qt::Horizontal));
	exampleElements.push_back(new Resistor(QPoint(), Qt::Horizontal, 1.0));
	exampleElements.push_back(new Switch(QPoint(), Qt::Horizontal, false));
	exampleElements.push_back(new ElectricHeater(QPoint(), Qt::Horizontal, 1.0));

	exampleElementsVertical.push_back(new Wire(QPoint(), Qt::Vertical));
	exampleElementsVertical.push_back(new Resistor(QPoint(), Qt::Vertical, 1.0));
	exampleElementsVertical.push_back(new Switch(QPoint(), Qt::Vertical, false));
	exampleElementsVertical.push_back(new ElectricHeater(QPoint(), Qt::Vertical, 1.0));

	exampleElementsNames.push_back(QString(u8"Провод"));
	exampleElementsNames.push_back(QString(u8"Резистор"));
	exampleElementsNames.push_back(QString(u8"Выключатель"));
	exampleElementsNames.push_back(QString(u8"Электрический нагреватель"));

	for (auto el : exampleElements)
		exampleElementsIcons.push_back(el->getIcon());

	elementsFactories.push_back(new WireFactory());
	elementsFactories.push_back(new ResistorFactory());
	elementsFactories.push_back(new SwitchFactory());
	elementsFactories.push_back(new ElectricHeaterFactory());

	elementsJsonNames["wire"] = 0;
	elementsJsonNames["resistor"] = 1;
	elementsJsonNames["switch"] = 2;
	elementsJsonNames["electricHeater"] = 3;
}

ElectricalElementsManager::~ElectricalElementsManager()
{
	for (auto el : exampleElements)
		delete el;
	for (auto icon : exampleElementsIcons)
		delete icon;
	for (auto f : elementsFactories)
		delete f;
	for (int i = 0; i < 2; i++)
		for (auto el : elements[i])
			delete el;
}

const ElectricalElement* ElectricalElementsManager::getExampleElement(int i, Qt::Orientation orientation) const
{
	return (orientation == Qt::Horizontal) ? exampleElements[i] : exampleElementsVertical[i];
}

void ElectricalElementsManager::fillElementsList(QListWidget* lw) const
{
	lw->clear();
	lw->setIconSize(QSize(RenderArea::GRID_POINTS_DISTANCE * 2, RenderArea::GRID_POINTS_DISTANCE));
	for (int i = 0; i < exampleElements.size(); i++)
		lw->addItem(new QListWidgetItem(*exampleElementsIcons[i], exampleElementsNames[i]));
}

void ElectricalElementsManager::fillPropertiesTable(QTableWidget* tw, int i) const
{
	exampleElements[i]->fillPropertiesTable(tw);
}

ElectricalElementFactory* ElectricalElementsManager::getFactory(int i) const
{
	return elementsFactories[i];
}

void ElectricalElementsManager::addElement(ElectricalElement* element)
{
	elements[element->getOrientation() - 1][element->getLocation()] = element;
}

ElectricalElement* ElectricalElementsManager::getElement(const QPoint& loc, Qt::Orientation orientation)
{
	auto it = elements[orientation - 1].find(loc);
	return (it != elements[orientation - 1].end()) ? it.value() : nullptr;
}

void ElectricalElementsManager::removeElement(const QPoint& loc, Qt::Orientation orientation)
{
	auto it = elements[orientation - 1].find(loc);
	if (it != elements[orientation - 1].end())
	{
		delete it.value();
		elements[orientation - 1].erase(it);
	}
}

void ElectricalElementsManager::clearElements()
{
	for (int i = 0; i < 2; i++)
	{
		for (auto el : elements[i])
			delete el;
		elements[i].clear();
	}
}

bool ElectricalElementsManager::readElementsFromJson(const QJsonObject& json)
{
	if (!json.contains("elements") || !json["elements"].isArray())
		return false;
	auto elArr = json["elements"].toArray();

	clearElements();
	for (const auto& lol : elArr)
	{
		auto elObj = lol.toObject();
		if (!elObj.contains("type") || !elObj["type"].isString())
			return false;
		auto key = elObj["type"].toString();
		if (!elementsJsonNames.contains(key))
			return false;
		auto el = elementsFactories[elementsJsonNames[key]]->readJsonAndCreate(elObj);
		addElement(el);
	}
	return true;
}

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