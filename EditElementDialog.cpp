#include "EditElementDialog.h"
#include "ElectricalElementsManager.h"

// Конструктор
EditElementDialog::EditElementDialog(QWidget *parent) : QDialog(parent) {
    ui.setupUi(this);
}

// Установка элемента
void EditElementDialog::setElement(ElectricalElement *el) {
    el->fillPropertiesTable(ui.elementPropertiesTableWidget);
}

// Нажатие кнопки "ОК"
void EditElementDialog::on_okButton_clicked() {
    // Заполнение свойств элемента
    elementProperties.clear();
    elementProperties.reserve(ui.elementPropertiesTableWidget->rowCount());
    for (int i = 0; i < ui.elementPropertiesTableWidget->rowCount(); i++)
        elementProperties.push_back(
            ui.elementPropertiesTableWidget->item(i, 1)->text());
    accept();
}