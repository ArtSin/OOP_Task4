#pragma once

#include "ui_MainWindow.h"
#include <QtWidgets/QMainWindow>

// Главное окно
class MainWindow : public QMainWindow {
    Q_OBJECT

  public:
    // Конструктор
    MainWindow(QWidget *parent = nullptr);

  private:
    // Элементы интерфейса
    Ui::MainWindowClass ui;

  private slots:
    // Создание новой схемы
    void on_newFileAction_triggered();
    // Открытие схемы из файла
    void on_openFileAction_triggered();
    // Сохранение схемы в файл
    void on_saveFileAction_triggered();

    // Нажатие кнопки добавления элемента
    void on_addElementPushButton_clicked();
    // Нажатие кнопки вычисления характеристик схемы
    void on_calculateCircuitPushButton_clicked();
    // Выбрано место для добавляемого элемента в виджете отрисовки
    void on_renderArea_elementPlaced(QPoint location,
                                     Qt::Orientation orientation);
    // Выбран изменяемый элемент в виджете отрисовки
    void on_renderArea_elementEdited(QPoint location,
                                     Qt::Orientation orientation);
    // Выбраны вершины для измерения напряжения в виджете отрисовки
    void on_renderArea_voltageMeasured(QPoint location1, QPoint location2);
    // Выбран элемент для измерения тока в виджете отрисовки
    void on_renderArea_currentMeasured(QPoint location,
                                       Qt::Orientation orientation);
    // Изменилось состояние виджета отрисовки
    void on_renderArea_renderingStateChanged(RenderingState state);
};
