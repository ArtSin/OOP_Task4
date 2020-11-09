#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_MainWindow.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);

private:
    Ui::MainWindowClass ui;

private slots:
    void on_newFileAction_triggered();
    void on_openFileAction_triggered();
    void on_saveFileAction_triggered();

    void on_addElementPushButton_clicked();
    void on_renderArea_elementPlaced(QPoint location, Qt::Orientation orientation);
};
