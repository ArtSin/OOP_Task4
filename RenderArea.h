#pragma once

#include "ElectricalElement.h"
#include <QMouseEvent>
#include <QPainter>
#include <QWidget>
#include <cmath>

class ElectricalElement;

// Состояние отрисовки: обычное, размещение, перемещение, изменение,
// переключение, удаление элемента, измерение напряжения, тока
enum class RenderingState {
    Normal,
    Placing,
    Moving,
    Editing,
    Toggling,
    Removing,
    MeasuringVoltage,
    MeasuringCurrent
};

// Виджет отрисовки
class RenderArea : public QWidget {
    Q_OBJECT

  public:
    // Расстояние между вершинами сетки
    const static int GRID_POINTS_DISTANCE = 40;
    // Отступ от края сетки
    const static int GRID_BORDER_OFFSET = GRID_POINTS_DISTANCE / 2;
    // Количество вершин по каждой стороне
    const static int GRID_POINTS_COUNT = 50;
    // Длина стороны квадрата - вершины сетки
    const static int GRID_POINT_SIZE = 4;
    // Длина стороны всей сетки
    const static int GRID_SIZE =
        2 * GRID_BORDER_OFFSET + GRID_POINTS_COUNT * GRID_POINTS_DISTANCE;

    // Кисти
    const static QBrush BLACK_BRUSH;
    const static QBrush WHITE_BRUSH;
    const static QBrush RED_BRUSH;
    const static QBrush MAGENTA_BRUSH;
    const static QBrush YELLOW_BRUSH;
    const static QBrush GREEN_BRUSH;
    const static QBrush BLUE_BRUSH;
    // Перья
    const static QPen BLACK_PEN;
    const static QPen RED_PEN;
    const static QPen MAGENTA_PEN;
    const static QPen YELLOW_PEN;
    const static QPen GREEN_PEN;
    const static QPen BLUE_PEN;
    // Перья с пунктирной линией
    const static QPen RED_DASH_PEN;
    const static QPen MAGENTA_DASH_PEN;
    const static QPen YELLOW_DASH_PEN;
    const static QPen GREEN_DASH_PEN;
    const static QPen BLUE_DASH_PEN;

    // Получение вершины сетки по данным координатам
    inline static QPoint getGridCoordinates(const QPoint &p) {
        return QPoint((int)std::round(double(p.x() - GRID_BORDER_OFFSET) /
                                      GRID_POINTS_DISTANCE),
                      (int)std::round(double(p.y() - GRID_BORDER_OFFSET) /
                                      GRID_POINTS_DISTANCE));
    }
    // Получение координат вершины сетки
    inline static QPoint getRealCoordinates(const QPoint &p) {
        return p * GRID_POINTS_DISTANCE +
               QPoint(GRID_BORDER_OFFSET, GRID_BORDER_OFFSET);
    }

    // Нахождение кисти в зависимости от состояния
    static const QBrush &findBrush(RenderingState state);
    // Нахождение пера в зависимости от состояния
    static const QPen &findPen(RenderingState state);
    // Нахождение пера (с пунктирной линией) в зависимости от состояния
    static const QPen &findDashPen(RenderingState state);

    // Конструктор
    RenderArea(QWidget *parent = nullptr);

    // Размер виджета
    QSize sizeHint() const;
    // Минимальный размер виджета
    QSize minimumSizeHint() const { return sizeHint(); }

    // Получение текущего состояния отрисовки
    RenderingState getRenderingState() const { return renderingState; }

  public slots:
    // Начало размещения элемента
    void startPlacingElement(int elementIndex);
    // Начало перемещения элемента
    void startMovingElement();
    // Начало изменения элемента
    void startEditingElement();
    // Начало переключения элемента
    void startTogglingElement();
    // Начало удаления элемента
    void startRemovingElement();
    // Начало измерения напряжения
    void startMeasuringVoltage();
    // Начало измерения тока
    void startMeasuringCurrent();

    // Приблизить
    void zoomIn();
    // Отдалить
    void zoomOut();

  signals:
    // Выбрано место для размещаемого элемента
    void elementPlaced(QPoint location, Qt::Orientation orientation);
    // Выбран изменяемый элемент
    void elementEdited(QPoint location, Qt::Orientation orientation);
    // Выбраны вершины для измерения напряжения
    void voltageMeasured(QPoint location1, QPoint location2);
    // Выбран элемент для измерения тока
    void currentMeasured(QPoint location, Qt::Orientation orientation);
    // Изменилось состояние отрисовки
    void renderingStateChanged(RenderingState state);

  private:
    // Максимальный масштаб
    const int MAX_ZOOM_LOG = 2;
    // Минимальный масштаб
    const int MIN_ZOOM_LOG = -2;

    // Текущий масштаб
    int zoomLog = 0;

    // Приведение масштабированных координат к изначальному масштабу
    QPoint calcZoomedCoordinates(const QPoint &p);

    // Состояние отрисовки
    RenderingState renderingState;
    // Происходит нажатие кнопки мыши
    bool clicking = false;

    // Номер выбранного элемента
    int elementIndex = -1;
    // Текущая вершина сетки для выбора элемента
    QPoint elementLocation;
    // Текущая ориентация элемента
    Qt::Orientation elementOrientation = Qt::Horizontal;
    // Перемещаемый элемент
    ElectricalElement *movingElement = nullptr;
    // Выбранная вершина сетки для вычисления напряжения
    QPoint selectedNode;

    // Нажатие кнопки мыши
    void mousePressEvent(QMouseEvent *e);
    // Перемещение мыши
    void mouseMoveEvent(QMouseEvent *e);
    // Отпускание кнопки мыши
    void mouseReleaseEvent(QMouseEvent *e);
    // Отрисовка виджета
    void paintEvent(QPaintEvent *e);
};