#include "RenderArea.h"
#include "ElectricalElementsManager.h"
#include "ElectricalElements/Switch.h"

const QBrush RenderArea::BLACK_BRUSH = QBrush(Qt::GlobalColor::black);
const QBrush RenderArea::WHITE_BRUSH = QBrush(Qt::GlobalColor::white);
const QBrush RenderArea::RED_BRUSH = QBrush(Qt::GlobalColor::red);
const QBrush RenderArea::GREEN_BRUSH = QBrush(Qt::GlobalColor::green);
const QBrush RenderArea::BLUE_BRUSH = QBrush(Qt::GlobalColor::blue);
const QPen RenderArea::BLACK_PEN = QPen(RenderArea::BLACK_BRUSH, 2.0);
const QPen RenderArea::RED_PEN = QPen(RenderArea::RED_BRUSH, 2.0);
const QPen RenderArea::GREEN_PEN = QPen(RenderArea::GREEN_BRUSH, 2.0);
const QPen RenderArea::BLUE_PEN = QPen(RenderArea::BLUE_BRUSH, 2.0);

RenderArea::RenderArea(QWidget* parent) : QWidget(parent)
{
    setBackgroundRole(QPalette::Base);
    setAutoFillBackground(true);
}

QSize RenderArea::sizeHint() const
{
    int sz = GRID_SIZE;
    if (zoomLog >= 0)
        sz <<= zoomLog;
    else
        sz >>= (-zoomLog);
    return QSize(sz, sz);
}

void RenderArea::startPlacingElement(int elementIndex)
{
    placingElement = true;
    togglingElement = false;
    removingElement = false;
    this->elementIndex = elementIndex;
    update();
}

void RenderArea::startTogglingElement()
{
    placingElement = false;
    togglingElement = true;
    removingElement = false;
    update();
}

void RenderArea::startRemovingElement()
{
    placingElement = false;
    togglingElement = false;
    removingElement = true;
    update();
}

void RenderArea::cancelActions()
{
    placingElement = false;
    togglingElement = false;
    removingElement = false;
    update();
}

void RenderArea::zoomIn()
{
    zoomLog = std::min(zoomLog + 1, MAX_ZOOM_LOG);
    updateGeometry();
    update();
}

void RenderArea::zoomOut()
{
    zoomLog = std::max(zoomLog - 1, MIN_ZOOM_LOG);
    updateGeometry();
    update();
}

QPoint RenderArea::calcZoomedCoordinates(const QPoint& p)
{
    int x = p.x(), y = p.y();
    if (zoomLog < 0)
    {
        x <<= (-zoomLog);
        y <<= (-zoomLog);
    }
    else
    {
        x >>= zoomLog;
        y >>= zoomLog;
    }
    return QPoint(x, y);
}

void RenderArea::mousePressEvent(QMouseEvent* e)
{
    if (e->button() == Qt::MouseButton::LeftButton)
        clicking = true;
    if (e->button() == Qt::MouseButton::RightButton)
    {
        elementOrientation = (elementOrientation == Qt::Horizontal) ? Qt::Vertical : Qt::Horizontal;
        update();
    }
}

void RenderArea::mouseMoveEvent(QMouseEvent* e)
{
    elementLocation = getGridCoordinates(calcZoomedCoordinates(e->pos()));
    update();
}

void RenderArea::mouseReleaseEvent(QMouseEvent* e)
{
    if (e->button() != Qt::MouseButton::LeftButton || !clicking)
        return;
    elementLocation = getGridCoordinates(calcZoomedCoordinates(e->pos()));

    if (placingElement)
    {
        placingElement = false;
        update();

        if (ElectricalElementsManager::getInstance().getElement(elementLocation, elementOrientation) == nullptr)
            emit elementPlaced(elementLocation, elementOrientation);
    }
    else if (togglingElement)
    {
        togglingElement = false;
        update();

        Switch* el = dynamic_cast<Switch*>(ElectricalElementsManager::getInstance().getElement(elementLocation, elementOrientation));
        if (el != nullptr)
            el->toggle();
    }
    else if (removingElement)
    {
        removingElement = false;
        update();

        ElectricalElementsManager::getInstance().removeElement(elementLocation, elementOrientation);
    }
}

void RenderArea::paintEvent(QPaintEvent* e)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::RenderHint::Antialiasing);
    painter.setBrush(BLACK_BRUSH);
    painter.setPen(Qt::NoPen);
    painter.scale(pow(2.0, zoomLog), pow(2.0, zoomLog));

    for (int i = 0; i < GRID_POINTS_COUNT; i++)
        for (int j = 0; j < GRID_POINTS_COUNT; j++)
            painter.drawRect(QRect(getRealCoordinates(QPoint(i, j)) + QPoint(-GRID_POINT_SIZE / 2, -GRID_POINT_SIZE / 2),
                QSize(GRID_POINT_SIZE, GRID_POINT_SIZE)));

    for (int or = 0; or < 2; or++)
        for (auto el : ElectricalElementsManager::getInstance().getElements()[or])
        {
            painter.save();
            painter.translate(getRealCoordinates(el->getLocation()));
            el->render(painter, RenderingState::Normal);
            painter.restore();
        }

    if (placingElement)
    {
        painter.save();
        painter.setBrush(BLUE_BRUSH);
        painter.translate(getRealCoordinates(elementLocation));
        auto el = ElectricalElementsManager::getInstance().getExampleElement(elementIndex, elementOrientation);
        el->render(painter, RenderingState::Placing);
        painter.restore();
    }

    const auto& elements = ElectricalElementsManager::getInstance().getElements()[elementOrientation - 1];
    auto it = elements.find(elementLocation);
    if (togglingElement && it != elements.end())
    {
        painter.save();
        painter.setBrush(GREEN_BRUSH);
        painter.translate(getRealCoordinates(elementLocation));
        it.value()->render(painter, RenderingState::Toggling);
        painter.restore();
    }

    if (removingElement && it != elements.end())
    {
        painter.save();
        painter.setBrush(RED_BRUSH);
        painter.translate(getRealCoordinates(elementLocation));
        it.value()->render(painter, RenderingState::Removing);
        painter.restore();
    }
}