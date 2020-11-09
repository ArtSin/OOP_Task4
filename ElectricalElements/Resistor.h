#pragma once

#include "../ElectricalElement.h"

class Resistor : public ElectricalElement
{
public:
	Resistor(QPoint location, Qt::Orientation orientation, double resistance);

	void fillPropertiesTable(QTableWidget* tw) const;
	void writeJson(QJsonObject& json) const;

	void render(QPainter& painter, RenderingState state) const;
};

class ResistorFactory : public ElectricalElementFactory
{
public:
	ElectricalElement* create(QPoint location, Qt::Orientation orientation, const QStringList& properties) const;
	ElectricalElement* readJsonAndCreate(const QJsonObject& json) const;
};