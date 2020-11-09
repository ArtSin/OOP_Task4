#pragma once

#include <cmath>
#include <QWidget>
#include <QPainter>
#include <QMouseEvent>
#include "ElectricalElement.h"

class ElectricalElement;

enum class RenderingState { Normal, Placing, Toggling, Removing };

class RenderArea : public QWidget
{
	Q_OBJECT

public:
	const static int GRID_POINTS_DISTANCE = 40;
	const static int GRID_BORDER_OFFSET = GRID_POINTS_DISTANCE / 2;
	const static int GRID_POINTS_COUNT = 50;
	const static int GRID_POINT_SIZE = 4;
	const static int GRID_SIZE = 2 * GRID_BORDER_OFFSET + GRID_POINTS_COUNT * GRID_POINTS_DISTANCE;

	const static QBrush BLACK_BRUSH;
	const static QBrush WHITE_BRUSH;
	const static QBrush RED_BRUSH;
	const static QBrush GREEN_BRUSH;
	const static QBrush BLUE_BRUSH;
	const static QPen BLACK_PEN;
	const static QPen RED_PEN;
	const static QPen GREEN_PEN;
	const static QPen BLUE_PEN;

	inline static QPoint getGridCoordinates(const QPoint& p)
	{
		return QPoint((int)std::round(double(p.x() - GRID_BORDER_OFFSET) / GRID_POINTS_DISTANCE),
			(int)std::round(double(p.y() - GRID_BORDER_OFFSET) / GRID_POINTS_DISTANCE));
	}
	inline static QPoint getRealCoordinates(const QPoint& p)
	{
		return p * GRID_POINTS_DISTANCE + QPoint(GRID_BORDER_OFFSET, GRID_BORDER_OFFSET);
	}

	RenderArea(QWidget* parent = nullptr);

	QSize sizeHint() const;
	QSize minimumSizeHint() const { return sizeHint(); }

public slots:
	void startPlacingElement(int elementIndex);
	void startTogglingElement();
	void startRemovingElement();
	void cancelActions();

	void zoomIn();
	void zoomOut();

signals:
	void elementPlaced(QPoint location, Qt::Orientation orientation);

private:
	const int MAX_ZOOM_LOG = 2;
	const int MIN_ZOOM_LOG = -2;

	int zoomLog = 0;

	QPoint calcZoomedCoordinates(const QPoint& p);

	bool placingElement = false;
	bool togglingElement = false;
	bool removingElement = false;
	bool clicking = false;

	int elementIndex = -1;
	QPoint elementLocation;
	Qt::Orientation elementOrientation = Qt::Horizontal;

	void mousePressEvent(QMouseEvent* e);
	void mouseMoveEvent(QMouseEvent* e);
	void mouseReleaseEvent(QMouseEvent* e);
	void paintEvent(QPaintEvent* e);
};