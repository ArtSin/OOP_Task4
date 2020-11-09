#pragma once

#include "ui_AddElementWindow.h"

class AddElementDialog : public QDialog
{
    Q_OBJECT

public:
    AddElementDialog(QWidget* parent = nullptr);

    static AddElementDialog& getInstance(QWidget* parent = nullptr)
    {
        static AddElementDialog* instance = new AddElementDialog(parent);
        return *instance;
    }

    int getElementIndex() const { return elementIndex; }
    const QStringList& getElementProperties() const { return elementProperties; }

private:
    Ui::AddElementDialog ui;

    int elementIndex = -1;
    QStringList elementProperties;

private slots:
    void on_elementsListWidget_itemSelectionChanged();
    void on_okButton_clicked();
};