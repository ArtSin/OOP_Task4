#include <QFileDialog>
#include <QMessageBox>
#include <QJsonDocument>
#include "MainWindow.h"
#include "ElectricalElementsManager.h"
#include "ElectricalCircuitCalculator.h"
#include "AddElementDialog.h"
#include "EditElementDialog.h"

// Конструктор
MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent)
{
    ui.setupUi(this);

    // Подключение событий нажатия кнопок к слотам в виджете отрисовки
    connect(ui.zoomInPushButton, &QPushButton::clicked, ui.renderArea, &RenderArea::zoomIn);
    connect(ui.zoomOutPushButton, &QPushButton::clicked, ui.renderArea, &RenderArea::zoomOut);
    connect(ui.moveElementPushButton, &QPushButton::clicked, ui.renderArea, &RenderArea::startMovingElement);
    connect(ui.editElementPushButton, &QPushButton::clicked, ui.renderArea, &RenderArea::startEditingElement);
    connect(ui.toggleElementPushButton, &QPushButton::clicked, ui.renderArea, &RenderArea::startTogglingElement);
    connect(ui.removeElementPushButton, &QPushButton::clicked, ui.renderArea, &RenderArea::startRemovingElement);
    connect(ui.measureVoltagePushButton, &QPushButton::clicked, ui.renderArea, &RenderArea::startMeasuringVoltage);
    connect(ui.measureCurrentPushButton, &QPushButton::clicked, ui.renderArea, &RenderArea::startMeasuringCurrent);

    // Диалог добавления элемента
    auto& dialog = AddElementDialog::getInstance(this);
    connect(&dialog, &AddElementDialog::accepted, [&]()
        {
            // При закрытии диалога по кнопке "ОК" начать размещать элемент в виджете отрисовки
            ui.renderArea->startPlacingElement(dialog.getElementIndex());
        });
}

// Создание новой схемы
void MainWindow::on_newFileAction_triggered()
{
    // Удаление всех элементов схемы
    ElectricalElementsManager::getInstance().clearElements();
    // Обновление виджета отрисовки
    ui.renderArea->update();
}

// Открытие схемы из файла
void MainWindow::on_openFileAction_triggered()
{
    // Открытие диалога для выбора файла
    QString fileName = QFileDialog::getOpenFileName(this,
        QString(u8"Открыть"), QString(u8""), QString(u8"JSON-файлы (*.json)"));
    if (fileName.isEmpty())
        return;
    // Открытие файла
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly))
    {
        QMessageBox::critical(this, QString(u8"Невозможно открыть файл"), file.errorString());
        return;
    }

    // Чтение всего файла и создание JSON-документа из него
    QByteArray data = file.readAll();
    QJsonParseError* error = nullptr;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(data, error);
    if (jsonDoc.isNull() && error != nullptr)
    {
        QMessageBox::critical(this, QString(u8"Ошибка разбора файла"), error->errorString());
        return;
    }

    // Получение элементов из JSON-документа
    if (!ElectricalElementsManager::getInstance().readElementsFromJson(jsonDoc.object()))
    {
        QMessageBox::critical(this, QString(u8"Ошибка разбора файла"), QString(u8"Ошибка разбора файла!"));
        return;
    }

    // Обновление виджета отрисовки
    ui.renderArea->update();
}

// Сохранение схемы в файл
void MainWindow::on_saveFileAction_triggered()
{
    // Открытие диалога для выбора файла
    QString fileName = QFileDialog::getSaveFileName(this,
        QString(u8"Сохранить"), QString(u8""), QString(u8"JSON-файлы (*.json)"));
    if (fileName.isEmpty())
        return;

    // Открытие файла
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly))
    {
        QMessageBox::critical(this, QString(u8"Невозможно открыть файл"), file.errorString());
        return;
    }

    // Запись всех элементов в JSON-документ
    QJsonObject jsonObj;
    ElectricalElementsManager::getInstance().writeElementsToJson(jsonObj);
    // Запись JSON-документа в файл
    file.write(QJsonDocument(jsonObj).toJson());
}

// Нажатие кнопки добавления элемента
void MainWindow::on_addElementPushButton_clicked()
{
    // Если сейчас размещается элемент, то перейти в обычное состояние
    if (ui.renderArea->getRenderingState() == RenderingState::Placing)
        ui.renderArea->startPlacingElement(-1);
    else
    {
        // Диалог добавления элемента
        auto& dialog = AddElementDialog::getInstance(this);
        // Открытие диалога
        dialog.setModal(true);
        dialog.open();
    }
}

// Нажатие кнопки вычисления характеристик схемы
void MainWindow::on_calculateCircuitPushButton_clicked()
{
    QString error;
    if (!ElectricalCircuitCalculator::getInstance().calculateCircuit(error))
        QMessageBox::critical(this, QString(u8"Ошибка"), error);
    else
        QMessageBox::information(this, QString(u8"Информация"), QString(u8"Успешно!"));
    // Обновление состояния виджета отрисовки
    ui.renderArea->update();
}

// Выбрано место для добавляемого элемента в виджете отрисовки
void MainWindow::on_renderArea_elementPlaced(QPoint location, Qt::Orientation orientation)
{
    // Диалог добавления элемента
    auto& dialog = AddElementDialog::getInstance();
    // Получение номера элемента и его свойств
    int elIndex = dialog.getElementIndex();
    const auto& elProperties = dialog.getElementProperties();
    // Создание элемента по номеру и свойствам
    auto& elManager = ElectricalElementsManager::getInstance();
    auto el = elManager.getFactory(elIndex)->create(location, orientation, elProperties);
    // Если не удалось создать элемент
    if (el == nullptr)
    {
        QMessageBox::critical(this, QString(u8"Ошибка"), QString(u8"Невозможно создать элемент (неверные параметры)!"));
        return;
    }
    // Добавление элемента в схему
    elManager.addElement(el);
}

// Выбран изменяемый элемент в виджете отрисовки
void MainWindow::on_renderArea_elementEdited(QPoint location, Qt::Orientation orientation)
{
    // Диалог изменения элемента
    auto& dialog = EditElementDialog::getInstance();
    // Получение свойств элемента
    const auto& elProperties = dialog.getElementProperties();
    // Получение элемента
    auto el = ElectricalElementsManager::getInstance().getElement(location, orientation);
    // Обновление его свойств
    if (!el->updateFromProperties(elProperties))
    {
        // Если не удалось обновить свойства
        QMessageBox::critical(this, QString(u8"Ошибка"), QString(u8"Невозможно изменить элемент (неверные параметры)!"));
    }
}

// Выбраны вершины для измерения напряжения в виджете отрисовки
void MainWindow::on_renderArea_voltageMeasured(QPoint location1, QPoint location2)
{
    // Вычисление напряжения между двумя точками
    double res;
    if (!ElectricalCircuitCalculator::getInstance().getVoltage(location1, location2, res))
    {
        QMessageBox::critical(this, QString(u8"Ошибка"), QString(u8"Точка не принадлежит схеме!"));
        return;
    }
    // Вывод значения
    ui.voltageLabel->setText(QString(u8"Напряжение: %1 В").arg(res));
}

// Выбран элемент для измерения тока в виджете отрисовки
void MainWindow::on_renderArea_currentMeasured(QPoint location, Qt::Orientation orientation)
{
    // Получение тока через элемент
    double res;
    if (!ElectricalCircuitCalculator::getInstance().getCurrent(location, orientation, res))
    {
        QMessageBox::critical(this, QString(u8"Ошибка"), QString(u8"Элемент не найден или неподходящего типа!"));
        return;
    }
    // Вывод значения
    ui.currentLabel->setText(QString(u8"Ток: %1 А").arg(res));
}

// Изменилось состояние виджета отрисовки
void MainWindow::on_renderArea_renderingStateChanged(RenderingState state)
{
    // В обычном состоянии все кнопки активны, иначе активна только кнопка текущего состояния (для перехода в обычное)
    ui.addElementPushButton->setEnabled(state == RenderingState::Normal || state == RenderingState::Placing);
    ui.moveElementPushButton->setEnabled(state == RenderingState::Normal || state == RenderingState::Moving);
    ui.editElementPushButton->setEnabled(state == RenderingState::Normal || state == RenderingState::Editing);
    ui.toggleElementPushButton->setEnabled(state == RenderingState::Normal || state == RenderingState::Toggling);
    ui.removeElementPushButton->setEnabled(state == RenderingState::Normal || state == RenderingState::Removing);
    ui.calculateCircuitPushButton->setEnabled(state == RenderingState::Normal);
    ui.measureVoltagePushButton->setEnabled(state == RenderingState::Normal || state == RenderingState::MeasuringVoltage);
    ui.measureCurrentPushButton->setEnabled(state == RenderingState::Normal || state == RenderingState::MeasuringCurrent);
}