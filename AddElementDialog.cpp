#include "AddElementDialog.h"
#include "ElectricalElementsManager.h"

// Конструктор
AddElementDialog::AddElementDialog(QWidget *parent) : QDialog(parent) {
    ui.setupUi(this);
    ElectricalElementsManager::getInstance().fillElementsList(
        ui.elementsListWidget);
}

// Выбор элемента в списке
void AddElementDialog::on_elementsListWidget_itemSelectionChanged() {
    // Очистка таблицы свойств
    ui.elementPropertiesTableWidget->clear();
    const auto &v = ui.elementsListWidget->selectedItems();
    if (!v.empty()) {
        // Если есть выбранный элемент, то заполнить таблицу свойств
        int row = ui.elementsListWidget->row(v[0]);
        ElectricalElementsManager::getInstance().fillPropertiesTable(
            ui.elementPropertiesTableWidget, row);
    }
}

// Нажатие кнопки "ОК"
void AddElementDialog::on_okButton_clicked() {
    const auto &v = ui.elementsListWidget->selectedItems();
    // Если нет выбранного элемента
    if (v.empty()) {
        elementIndex = -1;
        elementProperties.clear();
    } else {
        // Если есть, то заполнить номер и свойства
        elementIndex = ui.elementsListWidget->row(v[0]);
        elementProperties.clear();
        elementProperties.reserve(ui.elementPropertiesTableWidget->rowCount());
        for (int i = 0; i < ui.elementPropertiesTableWidget->rowCount(); i++)
            elementProperties.push_back(
                ui.elementPropertiesTableWidget->item(i, 1)->text());
    }
    accept();
}