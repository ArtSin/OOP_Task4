#pragma once

#include "../ElectricalElement.h"

class ElectricHeater : public ElectricalElement
{
public:
	ElectricHeater(QPoint location, Qt::Orientation orientation, double resistance);

	void fillPropertiesTable(QTableWidget* tw) const;
	void writeJson(QJsonObject& json) const;

	void render(QPainter& painter, RenderingState state) const;
};

class ElectricHeaterFactory : public ElectricalElementFactory
{
public:
	ElectricalElement* create(QPoint location, Qt::Orientation orientation, const QStringList& properties) const;
	ElectricalElement* readJsonAndCreate(const QJsonObject& json) const;
};