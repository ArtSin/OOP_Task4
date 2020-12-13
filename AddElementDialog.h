#pragma once

#include "ui_AddElementWindow.h"

// Диалог добавления элемента
class AddElementDialog : public QDialog
{
    Q_OBJECT

public:
    // Конструктор
    AddElementDialog(QWidget* parent = nullptr);
    // Получение единственного экземпляра класса
    static AddElementDialog& getInstance(QWidget* parent = nullptr)
    {
        static AddElementDialog* instance = new AddElementDialog(parent);
        return *instance;
    }

    // Геттеры для номера элемента и его свойств
    int getElementIndex() const { return elementIndex; }
    const QStringList& getElementProperties() const { return elementProperties; }

private:
    // Элементы интерфейса
    Ui::AddElementDialog ui;

    // Номер элемента
    int elementIndex = -1;
    // Свойства элемента
    QStringList elementProperties;

private slots:
    // Выбор элемента в списке
    void on_elementsListWidget_itemSelectionChanged();
    // Нажатие кнопки "ОК"
    void on_okButton_clicked();
};