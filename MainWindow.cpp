#include <QFileDialog>
#include <QMessageBox>
#include <QJsonDocument>
#include "MainWindow.h"
#include "ElectricalElementsManager.h"
#include "AddElementDialog.h"

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent)
{
    ui.setupUi(this);

    connect(ui.zoomInPushButton, &QPushButton::clicked, ui.renderArea, &RenderArea::zoomIn);
    connect(ui.zoomOutPushButton, &QPushButton::clicked, ui.renderArea, &RenderArea::zoomOut);
    connect(ui.toggleElementPushButton, &QPushButton::clicked, ui.renderArea, &RenderArea::startTogglingElement);
    connect(ui.removeElementPushButton, &QPushButton::clicked, ui.renderArea, &RenderArea::startRemovingElement);
    connect(ui.cancelPushButton, &QPushButton::clicked, ui.renderArea, &RenderArea::cancelActions);
}

void MainWindow::on_newFileAction_triggered()
{
    ElectricalElementsManager::getInstance().clearElements();
    ui.renderArea->update();
}

void MainWindow::on_openFileAction_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(this,
        QString(u8"Открыть"), QString(u8""), QString(u8"JSON-файлы (*.json)"));
    if (fileName.isEmpty())
        return;
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly))
    {
        QMessageBox::critical(this, QString(u8"Невозможно открыть файл"), file.errorString());
        return;
    }

    QByteArray data = file.readAll();
    QJsonParseError* error = nullptr;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(data, error);
    if (jsonDoc.isNull() && error != nullptr)
    {
        QMessageBox::critical(this, QString(u8"Ошибка разбора файла"), error->errorString());
        return;
    }

    if (!ElectricalElementsManager::getInstance().readElementsFromJson(jsonDoc.object()))
    {
        QMessageBox::critical(this, QString(u8"Ошибка разбора файла"), QString(u8"Ошибка разбора файла!"));
        return;
    }

    ui.renderArea->update();
}

void MainWindow::on_saveFileAction_triggered()
{
    QString fileName = QFileDialog::getSaveFileName(this,
        QString(u8"Сохранить"), QString(u8""), QString(u8"JSON-файлы (*.json)"));
    if (fileName.isEmpty())
        return;

    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly))
    {
        QMessageBox::critical(this, QString(u8"Невозможно открыть файл"), file.errorString());
        return;
    }

    QJsonObject jsonObj;
    ElectricalElementsManager::getInstance().writeElementsToJson(jsonObj);
    file.write(QJsonDocument(jsonObj).toJson());
}

void MainWindow::on_addElementPushButton_clicked()
{
    auto& dialog = AddElementDialog::getInstance(this);
    connect(&dialog, &AddElementDialog::accepted, [&]()
        {
            ui.renderArea->startPlacingElement(dialog.getElementIndex());
        });
    dialog.setModal(true);
    dialog.open();
}

void MainWindow::on_renderArea_elementPlaced(QPoint location, Qt::Orientation orientation)
{
    auto& dialog = AddElementDialog::getInstance();
    int elIndex = dialog.getElementIndex();
    const auto& elProperties = dialog.getElementProperties();
    auto& elManager = ElectricalElementsManager::getInstance();
    auto el = elManager.getFactory(elIndex)->create(location, orientation, elProperties);
    elManager.addElement(el);
}