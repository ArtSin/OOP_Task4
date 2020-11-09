#include "AddElementDialog.h"
#include "ElectricalElementsManager.h"

AddElementDialog::AddElementDialog(QWidget* parent) : QDialog(parent)
{
	ui.setupUi(this);
	ElectricalElementsManager::getInstance().fillElementsList(ui.elementsListWidget);
}

void AddElementDialog::on_elementsListWidget_itemSelectionChanged()
{
	ui.elementPropertiesTableWidget->clear();
	const auto& v = ui.elementsListWidget->selectedItems();
	if (!v.empty())
	{
		int row = ui.elementsListWidget->row(v[0]);
		ElectricalElementsManager::getInstance().fillPropertiesTable(ui.elementPropertiesTableWidget, row);
	}
}

void AddElementDialog::on_okButton_clicked()
{
	const auto& v = ui.elementsListWidget->selectedItems();
	if (v.empty())
	{
		elementIndex = -1;
		elementProperties.clear();
	}
	else
	{
		elementIndex = ui.elementsListWidget->row(v[0]);
		elementProperties.clear();
		elementProperties.reserve(ui.elementPropertiesTableWidget->rowCount());
		for (int i = 0; i < ui.elementPropertiesTableWidget->rowCount(); i++)
			elementProperties.push_back(ui.elementPropertiesTableWidget->item(i, 1)->text());
	}
	accept();
}