#include "MainWindow.h"
#include <QtWidgets/QApplication>

int main(int argc, char* argv[])
{
    // Создание приложения
    QApplication a(argc, argv);
    // Создание главного окна
    MainWindow w;
    // Показ окна и запуск приложения
    w.show();
    return a.exec();
}
