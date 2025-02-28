#pragma once

#include "ElectricalElement.h"
#include "ui_EditElementWindow.h"

// Диалог изменения элемента
class EditElementDialog : public QDialog {
    Q_OBJECT

  public:
    // Конструктор
    EditElementDialog(QWidget *parent = nullptr);
    // Получение единственного экземпляра класса
    static EditElementDialog &getInstance(QWidget *parent = nullptr) {
        static EditElementDialog *instance = new EditElementDialog(parent);
        return *instance;
    }

    // Установка элемента
    void setElement(ElectricalElement *el);
    // Получение свойств элемента
    const QStringList &getElementProperties() const {
        return elementProperties;
    }

  private:
    // Элементы интерфейса
    Ui::EditElementDialog ui;

    // Свойства элемента
    QStringList elementProperties;

  private slots:
    // Нажатие кнопки "ОК"
    void on_okButton_clicked();
};