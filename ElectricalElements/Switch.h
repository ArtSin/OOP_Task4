#pragma once

#include "../ElectricalElement.h"

// Класс выключателя
class Switch : public ElectricalElement {
  public:
    // Конструктор, принимающий позицию центра, ориентацию и состояние
    Switch(QPoint location, Qt::Orientation orientation, bool toggled);

    // Обновление свойств выключателя по списку
    bool updateFromProperties(const QStringList &properties);
    // Заполнение таблицы свойств
    void fillPropertiesTable(QTableWidget *tw) const;
    // Запись выключателя в JSON-документ
    void writeJson(QJsonObject &json) const;

    // Отрисовка выключателя в нужном состоянии
    void render(QPainter &painter, RenderingState state) const;

    // Состояние выключателя
    bool isToggled() { return toggled; }
    // Переключение выключателя
    void toggle() {
        toggled ^= 1;
        resistance = (toggled ? 0.0 : std::numeric_limits<double>().infinity());
    }

  private:
    bool toggled;
};

// Фабрика для выключателей
class SwitchFactory : public ElectricalElementFactory {
  public:
    // Создание выключателя по позиции, ориентации и списку свойств
    ElectricalElement *create(QPoint location, Qt::Orientation orientation,
                              const QStringList &properties) const;
    // Создание выключателя из JSON-объекта
    ElectricalElement *readJsonAndCreate(const QJsonObject &json) const;
};