#pragma once

#include "../ElectricalElement.h"

class Wire : public ElectricalElement
{
public:
	Wire(QPoint location, Qt::Orientation orientation);

	void fillPropertiesTable(QTableWidget* tw) const;
	void writeJson(QJsonObject& json) const;

	void render(QPainter& painter, RenderingState state) const;
};

class WireFactory : public ElectricalElementFactory
{
public:
	ElectricalElement* create(QPoint location, Qt::Orientation orientation, const QStringList& properties) const;
	ElectricalElement* readJsonAndCreate(const QJsonObject& json) const;
};