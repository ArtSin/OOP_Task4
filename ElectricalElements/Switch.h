#pragma once

#include "../ElectricalElement.h"

class Switch : public ElectricalElement
{
public:
	Switch(QPoint location, Qt::Orientation orientation, bool toggled);

	void fillPropertiesTable(QTableWidget* tw) const;
	void writeJson(QJsonObject& json) const;

	void render(QPainter& painter, RenderingState state) const;

	bool isToggled() { return toggled; }
	void toggle()
	{
		toggled ^= 1;
		resistance = (toggled ? 0 : std::numeric_limits<double>().infinity());
	}

private:
	bool toggled;
};

class SwitchFactory : public ElectricalElementFactory
{
public:
	ElectricalElement* create(QPoint location, Qt::Orientation orientation, const QStringList& properties) const;
	ElectricalElement* readJsonAndCreate(const QJsonObject& json) const;
};