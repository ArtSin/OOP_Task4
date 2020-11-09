#pragma once

#include <array>
#include <QListWidget>
#include <QTableWidget>
#include "RenderArea.h"

bool operator<(const QPoint& a, const QPoint& b);

class ElectricalElementsManager
{
public:
	static ElectricalElementsManager& getInstance()
	{
		static ElectricalElementsManager instance;
		return instance;
	}
	~ElectricalElementsManager();

	const ElectricalElement* getExampleElement(int i, Qt::Orientation orientation) const;
	void fillElementsList(QListWidget* lw) const;
	void fillPropertiesTable(QTableWidget* tw, int i) const;
	ElectricalElementFactory* getFactory(int i) const;

	void addElement(ElectricalElement* element);
	ElectricalElement* getElement(const QPoint& loc, Qt::Orientation orientation);
	void removeElement(const QPoint& loc, Qt::Orientation orientation);
	std::array<QMap<QPoint, ElectricalElement*>, 2>& getElements() { return elements; }
	void clearElements();

	bool readElementsFromJson(const QJsonObject& json);
	void writeElementsToJson(QJsonObject& json) const;

private:
	ElectricalElementsManager();

	QVector<ElectricalElement*> exampleElements;
	QVector<ElectricalElement*> exampleElementsVertical;
	QStringList exampleElementsNames;
	QVector<QIcon*> exampleElementsIcons;
	QVector<ElectricalElementFactory*> elementsFactories;
	QMap<QString, int> elementsJsonNames;
	
	std::array<QMap<QPoint, ElectricalElement*>, 2> elements;
};

