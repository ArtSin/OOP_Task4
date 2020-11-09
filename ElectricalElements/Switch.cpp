#include "Switch.h"

Switch::Switch(QPoint location, Qt::Orientation orientation, bool toggled = false)
	: ElectricalElement(location, orientation, toggled ? 0.0 : std::numeric_limits<double>().infinity()),
	toggled(toggled)
{
}

void Switch::fillPropertiesTable(QTableWidget* tw) const
{
	ElectricalElement::fillPropertiesTable(tw);
	tw->setRowCount(1);

	auto item00 = new QTableWidgetItem(QString(u8"Включен (0/1)"));
	item00->setFlags(item00->flags() & ~Qt::ItemFlag::ItemIsEditable);
	tw->setItem(0, 0, item00);
	auto item01 = new QTableWidgetItem(QString(u8"0"));
	item01->setFlags(item01->flags() | Qt::ItemFlag::ItemIsEditable);
	tw->setItem(0, 1, item01);
}

void Switch::writeJson(QJsonObject& json) const
{
	json["type"] = "switch";
	json["x"] = location.x();
	json["y"] = location.y();
	json["orientation"] = (orientation == Qt::Horizontal) ? "horizontal" : "vertical";
	json["toggled"] = toggled;
}

void Switch::render(QPainter& painter, RenderingState state) const
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
	painter.drawRect(4 * RenderArea::GRID_POINTS_DISTANCE / 10, -RenderArea::GRID_POINT_SIZE / 2,
					12 * RenderArea::GRID_POINTS_DISTANCE / 10, RenderArea::GRID_POINT_SIZE);
	painter.restore();

	painter.translate(4 * RenderArea::GRID_POINTS_DISTANCE / 10, 0);

	painter.drawRect(-RenderArea::GRID_POINT_SIZE / 2, -RenderArea::GRID_POINT_SIZE / 2,
					RenderArea::GRID_POINT_SIZE, RenderArea::GRID_POINT_SIZE);
	painter.drawRect(12 * RenderArea::GRID_POINTS_DISTANCE / 10 - RenderArea::GRID_POINT_SIZE / 2, -RenderArea::GRID_POINT_SIZE / 2,
					RenderArea::GRID_POINT_SIZE, RenderArea::GRID_POINT_SIZE);

	if (!toggled)
		painter.rotate(-20.0);
	painter.drawRect(0, -1, 12 * RenderArea::GRID_POINTS_DISTANCE / 10, 2);
}

ElectricalElement* SwitchFactory::create(QPoint location, Qt::Orientation orientation, const QStringList& properties) const
{
	bool ok = false;
	int state = properties[0].toInt(&ok);
	if (!ok || state < 0 || state > 1)
		return nullptr;
	return new Switch(location, orientation, state);
}

ElectricalElement* SwitchFactory::readJsonAndCreate(const QJsonObject& json) const
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

	if (!json.contains("toggled") || !json["toggled"].isBool())
		return nullptr;
	bool toggled = json["toggled"].toBool();

	return new Switch(QPoint(x, y), orientation, toggled);
}