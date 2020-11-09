#include "ElectricHeater.h"

ElectricHeater::ElectricHeater(QPoint location, Qt::Orientation orientation, double resistance)
	: ElectricalElement(location, orientation, resistance)
{
}

void ElectricHeater::fillPropertiesTable(QTableWidget* tw) const
{
	ElectricalElement::fillPropertiesTable(tw);
	tw->setRowCount(1);

	auto item00 = new QTableWidgetItem(QString(u8"Сопротивление (Ом)"));
	item00->setFlags(item00->flags() & ~Qt::ItemFlag::ItemIsEditable);
	tw->setItem(0, 0, item00);
	auto item01 = new QTableWidgetItem(QString(u8"1.0"));
	item01->setFlags(item01->flags() | Qt::ItemFlag::ItemIsEditable);
	tw->setItem(0, 1, item01);
}

void ElectricHeater::writeJson(QJsonObject& json) const
{
	json["type"] = "electricHeater";
	json["x"] = location.x();
	json["y"] = location.y();
	json["orientation"] = (orientation == Qt::Horizontal) ? "horizontal" : "vertical";
	json["resistance"] = resistance;
}

void ElectricHeater::render(QPainter& painter, RenderingState state) const
{
	if (orientation == Qt::Horizontal)
		painter.translate(-RenderArea::GRID_POINTS_DISTANCE, 0);
	else
	{
		painter.translate(0, -RenderArea::GRID_POINTS_DISTANCE);
		painter.rotate(90.0);
	}

	painter.drawRect(0, -1, 4 * RenderArea::GRID_POINTS_DISTANCE / 10, 2);
	painter.drawRect(16 * RenderArea::GRID_POINTS_DISTANCE / 10, -1, 4 * RenderArea::GRID_POINTS_DISTANCE / 10, 2);
	
	painter.save();
	painter.setBrush(RenderArea::WHITE_BRUSH);
	if (state == RenderingState::Normal)
		painter.setPen(RenderArea::BLACK_PEN);
	else if (state == RenderingState::Placing)
		painter.setPen(RenderArea::BLUE_PEN);
	else if (state == RenderingState::Toggling)
		painter.setPen(RenderArea::GREEN_PEN);
	else
		painter.setPen(RenderArea::RED_PEN);
	painter.drawRect(4 * RenderArea::GRID_POINTS_DISTANCE / 10, -RenderArea::GRID_POINTS_DISTANCE / 4,
		12 * RenderArea::GRID_POINTS_DISTANCE / 10, RenderArea::GRID_POINTS_DISTANCE / 2);
	painter.restore();

	for (int i = 0; i < 3; i++)
		painter.drawRect((7 + 3 * i) * RenderArea::GRID_POINTS_DISTANCE / 10 - 1, -RenderArea::GRID_POINTS_DISTANCE / 4,
			2, RenderArea::GRID_POINTS_DISTANCE / 2);
}

ElectricalElement* ElectricHeaterFactory::create(QPoint location, Qt::Orientation orientation, const QStringList& properties) const
{
	bool ok = false;
	double resistance = properties[0].toDouble(&ok);
	if (!ok || resistance < 0)
		return nullptr;
	return new ElectricHeater(location, orientation, resistance);
}

ElectricalElement* ElectricHeaterFactory::readJsonAndCreate(const QJsonObject& json) const
{
	if (!json.contains("x") || !json.contains("y"))
		return nullptr;
	int x = json["x"].toInt(-1), y = json["y"].toInt(-1);
	if (x < 0 || y < 0)
		return nullptr;

	if (!json.contains("orientation"))
		return nullptr;
	QString orientationStr = json["orientation"].toString("");
	if (orientationStr != "horizontal" && orientationStr != "vertical")
		return nullptr;
	Qt::Orientation orientation = (orientationStr == "horizontal") ? Qt::Horizontal : Qt::Vertical;

	if (!json.contains("resistance") || !json["resistance"].isDouble())
		return nullptr;
	double resistance = json["resistance"].toDouble();
	if (resistance < 0)
		return nullptr;

	return new ElectricHeater(QPoint(x, y), orientation, resistance);
}