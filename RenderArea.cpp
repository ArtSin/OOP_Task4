#include "RenderArea.h"
#include "ElectricalElementsManager.h"
#include "ElectricalElements/Switch.h"
#include "EditElementDialog.h"

// Кисти
const QBrush RenderArea::BLACK_BRUSH = QBrush(Qt::GlobalColor::black);
const QBrush RenderArea::WHITE_BRUSH = QBrush(Qt::GlobalColor::white);
const QBrush RenderArea::RED_BRUSH = QBrush(Qt::GlobalColor::red);
const QBrush RenderArea::MAGENTA_BRUSH = QBrush(Qt::GlobalColor::magenta);
const QBrush RenderArea::YELLOW_BRUSH = QBrush(Qt::GlobalColor::yellow);
const QBrush RenderArea::GREEN_BRUSH = QBrush(Qt::GlobalColor::green);
const QBrush RenderArea::BLUE_BRUSH = QBrush(Qt::GlobalColor::blue);
// Перья
const QPen RenderArea::BLACK_PEN = QPen(RenderArea::BLACK_BRUSH, 2.0);
const QPen RenderArea::RED_PEN = QPen(RenderArea::RED_BRUSH, 2.0);
const QPen RenderArea::MAGENTA_PEN = QPen(RenderArea::MAGENTA_BRUSH, 2.0);
const QPen RenderArea::YELLOW_PEN = QPen(RenderArea::YELLOW_BRUSH, 2.0);
const QPen RenderArea::GREEN_PEN = QPen(RenderArea::GREEN_BRUSH, 2.0);
const QPen RenderArea::BLUE_PEN = QPen(RenderArea::BLUE_BRUSH, 2.0);
// Перья с пунктирной линией
const QPen RenderArea::RED_DASH_PEN = QPen(RenderArea::RED_BRUSH, 2.0, Qt::DashLine);
const QPen RenderArea::MAGENTA_DASH_PEN = QPen(RenderArea::MAGENTA_BRUSH, 2.0, Qt::DashLine);
const QPen RenderArea::YELLOW_DASH_PEN = QPen(RenderArea::YELLOW_BRUSH, 2.0, Qt::DashLine);
const QPen RenderArea::GREEN_DASH_PEN = QPen(RenderArea::GREEN_BRUSH, 2.0, Qt::DashLine);
const QPen RenderArea::BLUE_DASH_PEN = QPen(RenderArea::BLUE_BRUSH, 2.0, Qt::DashLine);

// Нахождение кисти в зависимости от состояния
const QBrush& RenderArea::findBrush(RenderingState state)
{
    if (state == RenderingState::Placing || state == RenderingState::Moving)
        return BLUE_BRUSH;
    else if (state == RenderingState::Editing)
        return YELLOW_BRUSH;
    else if (state == RenderingState::Toggling)
        return GREEN_BRUSH;
    else if (state == RenderingState::Removing)
        return RED_BRUSH;
    else if (state == RenderingState::MeasuringCurrent)
        return MAGENTA_BRUSH;
    else
        return BLACK_BRUSH;
}
// Нахождение пера в зависимости от состояния
const QPen& RenderArea::findPen(RenderingState state)
{
    if (state == RenderingState::Placing || state == RenderingState::Moving)
        return BLUE_PEN;
    else if (state == RenderingState::Editing)
        return YELLOW_PEN;
    else if (state == RenderingState::Toggling)
        return GREEN_PEN;
    else if (state == RenderingState::Removing)
        return RED_PEN;
    else if (state == RenderingState::MeasuringCurrent)
        return MAGENTA_PEN;
    else
        return BLACK_PEN;
}
// Нахождение пера (с пунктирной линией) в зависимости от состояния
const QPen& RenderArea::findDashPen(RenderingState state)
{
    if (state == RenderingState::Moving)
        return BLUE_DASH_PEN;
    else if (state == RenderingState::Editing)
        return YELLOW_DASH_PEN;
    else if (state == RenderingState::Toggling)
        return GREEN_DASH_PEN;
    else if (state == RenderingState::Removing)
        return RED_DASH_PEN;
    else
        return MAGENTA_DASH_PEN;
}

// Конструктор
RenderArea::RenderArea(QWidget* parent) : QWidget(parent)
{
    QPalette pal(palette());
    pal.setColor(QPalette::Base, Qt::white);
    setPalette(pal);
    setBackgroundRole(QPalette::Base);
    setAutoFillBackground(true);

    // Диалог изменения элемента
    auto& dialog = EditElementDialog::getInstance(this);
    connect(&dialog, &EditElementDialog::accepted, [&]()
        {
            // При закрытии диалога по кнопке "ОК" изменить элемент
            emit elementEdited(elementLocation, elementOrientation);
        });
}

// Размер виджета
QSize RenderArea::sizeHint() const
{
    // Вычисление размеров с учётом масштаба
    int sz = GRID_SIZE;
    if (zoomLog >= 0)
        sz <<= zoomLog;
    else
        sz >>= (-zoomLog);
    return QSize(sz, sz);
}

// Начало размещения элемента
void RenderArea::startPlacingElement(int elementIndex)
{
    // Изменение состояния или сброс до обычного
    renderingState = (elementIndex == -1) ? RenderingState::Normal : RenderingState::Placing;
    // Вызов события
    emit renderingStateChanged(renderingState);
    // Обновление номера выбранного элемента
    this->elementIndex = elementIndex;
    // Обновление виджета
    update();
}

// Начало перемещения элемента
void RenderArea::startMovingElement()
{
    // Изменение состояния или сброс до обычного
    renderingState = (renderingState == RenderingState::Moving && movingElement == nullptr) ? RenderingState::Normal : RenderingState::Moving;
    // Вызов события
    emit renderingStateChanged(renderingState);
    // Обновление виджета
    update();
}

// Начало изменения элемента
void RenderArea::startEditingElement()
{
    // Изменение состояния или сброс до обычного
    renderingState = (renderingState == RenderingState::Editing) ? RenderingState::Normal : RenderingState::Editing;
    // Вызов события
    emit renderingStateChanged(renderingState);
    // Обновление виджета
    update();
}

// Начало переключения элемента
void RenderArea::startTogglingElement()
{
    // Изменение состояния или сброс до обычного
    renderingState = (renderingState == RenderingState::Toggling) ? RenderingState::Normal : RenderingState::Toggling;
    // Вызов события
    emit renderingStateChanged(renderingState);
    // Обновление виджета
    update();
}

// Начало удаления элемента
void RenderArea::startRemovingElement()
{
    // Изменение состояния или сброс до обычного
    renderingState = (renderingState == RenderingState::Removing) ? RenderingState::Normal : RenderingState::Removing;
    // Вызов события
    emit renderingStateChanged(renderingState);
    // Обновление виджета
    update();
}

// Начало измерения напряжения
void RenderArea::startMeasuringVoltage()
{
    // Сброс выбранной вершины сетки
    selectedNode = QPoint(-1, -1);
    // Изменение состояния или сброс до обычного
    renderingState = (renderingState == RenderingState::MeasuringVoltage) ? RenderingState::Normal : RenderingState::MeasuringVoltage;
    // Вызов события
    emit renderingStateChanged(renderingState);
    // Обновление виджета
    update();
}

// Начало измерения тока
void RenderArea::startMeasuringCurrent()
{
    // Изменение состояния или сброс до обычного
    renderingState = (renderingState == RenderingState::MeasuringCurrent) ? RenderingState::Normal : RenderingState::MeasuringCurrent;
    // Вызов события
    emit renderingStateChanged(renderingState);
    // Обновление виджета
    update();
}

// Приблизить
void RenderArea::zoomIn()
{
    // Изменение масштаба
    zoomLog = std::min(zoomLog + 1, MAX_ZOOM_LOG);
    // Обновление виджета
    updateGeometry();
    update();
}

// Отдалить
void RenderArea::zoomOut()
{
    // Изменение масштаба
    zoomLog = std::max(zoomLog - 1, MIN_ZOOM_LOG);
    // Обновление виджета
    updateGeometry();
    update();
}

// Приведение масштабированных координат к изначальному масштабу
QPoint RenderArea::calcZoomedCoordinates(const QPoint& p)
{
    int x = p.x(), y = p.y();
    // Деление на 2^zoomLog
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

// Нажатие кнопки мыши
void RenderArea::mousePressEvent(QMouseEvent* e)
{
    // Левая кнопка мыши
    if (e->button() == Qt::MouseButton::LeftButton)
        clicking = true;
    // Правая кнопка мыши
    if (e->button() == Qt::MouseButton::RightButton)
    {
        // Изменение ориентации элемента
        elementOrientation = (elementOrientation == Qt::Horizontal) ? Qt::Vertical : Qt::Horizontal;
        // Обновление виджета
        update();
    }
}

// Перемещение мыши
void RenderArea::mouseMoveEvent(QMouseEvent* e)
{
    // Обновление вершины сетки для выбора элемента
    elementLocation = getGridCoordinates(calcZoomedCoordinates(e->pos()));
    // Обновление виджета
    update();
}

// Отпускание кнопки мыши
void RenderArea::mouseReleaseEvent(QMouseEvent* e)
{
    // Рассматривается случай отпускания нажатой левой кнопки мыши
    if (e->button() != Qt::MouseButton::LeftButton || !clicking)
        return;
    // Обновление вершины сетки для выбора элемента
    elementLocation = getGridCoordinates(calcZoomedCoordinates(e->pos()));

    // Обычное состояние не рассматривается
    if (renderingState == RenderingState::Normal)
        return;

    // Сохранение текущего состояния
    auto prevRenderingState = renderingState;
    // Установка обычного состояния
    renderingState = RenderingState::Normal;
    // Вызов события
    emit renderingStateChanged(renderingState);
    // Обновление виджета
    update();
    // Если элемент размещают
    if (prevRenderingState == RenderingState::Placing)
    {
        // Если выбранное место пустое, то вызвать событие размещения элемента
        if (ElectricalElementsManager::getInstance().getElement(elementLocation, elementOrientation) == nullptr)
            emit elementPlaced(elementLocation, elementOrientation);
    }
    // Если элемент перемещают
    else if (prevRenderingState == RenderingState::Moving)
    {
        // Элемент в выбранном месте
        auto el = ElectricalElementsManager::getInstance().getElement(elementLocation, elementOrientation);
        // Выбор перемещаемого элемента
        if (movingElement == nullptr)
        {
            // Если в выбранном месте есть элемент, то он выбирается для перемещения
            if (el != nullptr)
            {
                movingElement = el;
                ElectricalElementsManager::getInstance().removeElement(elementLocation, elementOrientation, false);
                // Возврат состояния
                renderingState = RenderingState::Moving;
                // Вызов события
                emit renderingStateChanged(renderingState);
                // Обновление виджета
                update();
            }
        }
        else // Выбор нового места для элемента
        {
            movingElement->setLocation(elementLocation);
            movingElement->setOrientation(elementOrientation);
            // Если выбранное место пустое, то закончить перемещение
            if (el == nullptr)
            {
                ElectricalElementsManager::getInstance().addElement(movingElement);
                movingElement = nullptr;
            }
            else // Иначе продолжить
            {
                // Возврат состояния
                renderingState = RenderingState::Moving;
                // Вызов события
                emit renderingStateChanged(renderingState);
                // Обновление виджета
                update();
            }
        }
    }
    // Если элемент изменяют
    else if (prevRenderingState == RenderingState::Editing)
    {
        // Получение элемента в выбранном месте
        auto el = ElectricalElementsManager::getInstance().getElement(elementLocation, elementOrientation);
        // Если он есть
        if (el != nullptr)
        {
            // Диалог изменения элемента
            auto& dialog = EditElementDialog::getInstance(this);
            dialog.setElement(el);
            // Открытие диалога
            dialog.setModal(true);
            dialog.open();
        }
    }
    // Если элемент переключают
    else if (prevRenderingState == RenderingState::Toggling)
    {
        // Попытка получения элемента в выбранном месте и приведения его к выключателю
        Switch* el = dynamic_cast<Switch*>(ElectricalElementsManager::getInstance().getElement(elementLocation, elementOrientation));
        // Если успешно, то переключить его
        if (el != nullptr)
            el->toggle();
    }
    // Если элемент удаляют
    else if (prevRenderingState == RenderingState::Removing)
        ElectricalElementsManager::getInstance().removeElement(elementLocation, elementOrientation, true);
    // Если измеряют напряжение
    else if (prevRenderingState == RenderingState::MeasuringVoltage)
    {
        // Сейчас выбрана одна вершина сетки
        if (selectedNode == QPoint(-1, -1))
        {
            // Обновление выбранной вершины
            selectedNode = elementLocation;
            // Возврат состояния
            renderingState = RenderingState::MeasuringVoltage;
            // Вызов события
            emit renderingStateChanged(renderingState);
            // Обновление виджета
            update();
        }
        else // Выбраны две вершины, вызов события
            emit voltageMeasured(selectedNode, elementLocation);
    }
    // Если измеряют ток
    else if (prevRenderingState == RenderingState::MeasuringCurrent)
    {
        // Если в выбранном месте есть элемент, то вызвать событие измерения тока
        if (ElectricalElementsManager::getInstance().getElement(elementLocation, elementOrientation) != nullptr)
            emit currentMeasured(elementLocation, elementOrientation);
    }
}

// Отрисовка виджета
void RenderArea::paintEvent(QPaintEvent* e)
{
    // Создание отрисовщика
    QPainter painter(this);
    painter.setRenderHint(QPainter::RenderHint::Antialiasing);
    // Задание кисти, пера, масштаба
    painter.setBrush(BLACK_BRUSH);
    painter.setPen(Qt::NoPen);
    painter.scale(pow(2.0, zoomLog), pow(2.0, zoomLog));

    // Отрисовка всех вершин сетки
    for (int i = 0; i < GRID_POINTS_COUNT; i++)
        for (int j = 0; j < GRID_POINTS_COUNT; j++)
            painter.drawRect(QRect(getRealCoordinates(QPoint(i, j)) + QPoint(-GRID_POINT_SIZE / 2, -GRID_POINT_SIZE / 2),
                QSize(GRID_POINT_SIZE, GRID_POINT_SIZE)));

    // Отрисовка всех элементов схемы (в обычном состоянии)
    for (int orient = 0; orient < 2; orient++)
        for (auto el : ElectricalElementsManager::getInstance().getElements()[orient])
        {
            // Сохранение состояния отрисовщика в стеке
            painter.save();
            // Перемещение к точке
            painter.translate(getRealCoordinates(el->getLocation()));
            // Отрисовка
            el->render(painter, RenderingState::Normal);
            // Возврат состояния
            painter.restore();
        }

    // Если размещается элемент, то нарисовать его в соответствующем состоянии
    if (renderingState == RenderingState::Placing)
    {
        painter.save();
        painter.setBrush(BLUE_BRUSH);
        painter.translate(getRealCoordinates(elementLocation));
        auto el = ElectricalElementsManager::getInstance().getExampleElement(elementIndex, elementOrientation);
        el->render(painter, renderingState);
        painter.restore();
    }

    // Если перемещается элемент, то нарисовать его в соответствующем состоянии
    if (renderingState == RenderingState::Moving && movingElement != nullptr)
    {
        painter.save();
        painter.setBrush(BLUE_BRUSH);
        painter.translate(getRealCoordinates(elementLocation));
        movingElement->render(painter, renderingState);
        painter.restore();
    }

    // Если выбирается вершина для измерения напряжения, то нарисовать её в соответствующем состоянии
    if (renderingState == RenderingState::MeasuringVoltage)
    {
        painter.save();
        painter.setBrush(MAGENTA_BRUSH);
        painter.translate(getRealCoordinates(elementLocation));
        painter.drawRect(-GRID_POINT_SIZE / 2, -GRID_POINT_SIZE / 2,
            GRID_POINT_SIZE, GRID_POINT_SIZE);
        painter.restore();
    }

    // Если элемент перемещается, изменяется, переключается, удаляется или измеряется ток
    if (renderingState == RenderingState::Moving || renderingState == RenderingState::Editing ||
        renderingState == RenderingState::Toggling || renderingState == RenderingState::Removing ||
        renderingState == RenderingState::MeasuringCurrent)
    {
        painter.save();
        painter.setBrush(findBrush(renderingState));
        painter.translate(getRealCoordinates(elementLocation));

        // Отрисовка рамки элемента пунктиром
        painter.save();
        if (elementOrientation == Qt::Horizontal)
            painter.translate(-RenderArea::GRID_POINTS_DISTANCE, 0);
        else
        {
            painter.translate(0, -RenderArea::GRID_POINTS_DISTANCE);
            painter.rotate(90.0);
        }
        painter.setBrush(Qt::NoBrush);
        painter.setPen(findDashPen(renderingState));
        painter.drawRect(0, -RenderArea::GRID_POINTS_DISTANCE / 2,
            2 * RenderArea::GRID_POINTS_DISTANCE, RenderArea::GRID_POINTS_DISTANCE);
        painter.restore();

        if (renderingState != RenderingState::Moving || (renderingState == RenderingState::Moving && movingElement == nullptr))
        {
            // Получение элемента в выбранном месте
            auto el = ElectricalElementsManager::getInstance().getElement(elementLocation, elementOrientation);
            // Если он есть, то отрисовать его в соответствующем состоянии
            if (el != nullptr)
                el->render(painter, renderingState);
        }
        painter.restore();
    }
}