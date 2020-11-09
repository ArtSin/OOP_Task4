#pragma once

#include <QString>
#include <QPainter>
#include <QIcon>
#include <QTableWidget>
#include <QJsonObject>
#include "RenderArea.h"

enum class RenderingState;

class ElectricalElement
{
public:
	ElectricalElement(QPoint location, Qt::Orientation orientation, double resistance);
	virtual ~ElectricalElement() {}

	virtual void fillPropertiesTable(QTableWidget* tw) const;
	virtual void writeJson(QJsonObject& json) const = 0;

	QPoint getLocation() const { return location; }
	Qt::Orientation getOrientation() const { return orientation; }
	virtual void render(QPainter& painter, RenderingState state) const = 0;
	virtual QIcon* getIcon() const;

	virtual double getResistance() { return resistance; }

	virtual bool operator==(const ElectricalElement& other) const;

protected:
	QPoint location;
	Qt::Orientation orientation;

	double resistance;
};

class ElectricalElementFactory
{
public:
	virtual ElectricalElement* create(QPoint location, Qt::Orientation orientation, const QStringList& properties) const = 0;
	virtual ElectricalElement* readJsonAndCreate(const QJsonObject& json) const = 0;
};

