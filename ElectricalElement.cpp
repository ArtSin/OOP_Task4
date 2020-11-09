#include "ElectricalElement.h"

ElectricalElement::ElectricalElement(QPoint location, Qt::Orientation orientation, double resistance)
	: location(location), orientation(orientation), resistance(resistance)
{
}

void ElectricalElement::fillPropertiesTable(QTableWidget* tw) const
{
	tw->clear();
	tw->setColumnCount(2);
	tw->setHorizontalHeaderItem(0, new QTableWidgetItem(QString(u8"Свойство")));
	tw->setHorizontalHeaderItem(1, new QTableWidgetItem(QString(u8"Значение")));
}

QIcon* ElectricalElement::getIcon() const
{
	QPixmap pixmap(RenderArea::GRID_POINTS_DISTANCE * 2, RenderArea::GRID_POINTS_DISTANCE);
	pixmap.fill();
	QPainter painter(&pixmap);
	painter.setRenderHint(QPainter::RenderHint::Antialiasing);
	painter.setBrush(RenderArea::BLACK_BRUSH);
	painter.setPen(Qt::NoPen);
	painter.translate(RenderArea::GRID_POINTS_DISTANCE, RenderArea::GRID_POINTS_DISTANCE / 2);
	render(painter, RenderingState::Normal);
	return new QIcon(pixmap);
}

bool ElectricalElement::operator==(const ElectricalElement& other) const
{
	return location == other.location && orientation == other.orientation;
}