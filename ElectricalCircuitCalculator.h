#pragma once

#include "ElectricalElements/CurrentSource.h"
#include "ElectricalElements/VoltageSource.h"
#include <QMap>
#include <QPoint>

// Класс для вычисления характеристик электрической схемы
class ElectricalCircuitCalculator {
  public:
    // Получение единственного экземпляра класса
    static ElectricalCircuitCalculator &getInstance() {
        static ElectricalCircuitCalculator instance;
        return instance;
    }

    // Вычисление характеристик схемы
    bool calculateCircuit(QString &error);
    // Нахождение напряжения между двумя точками
    bool getVoltage(const QPoint &loc1, const QPoint &loc2, double &res);
    // Нахождение тока через элемент
    bool getCurrent(const QPoint &loc, Qt::Orientation orientation,
                    double &res);

  private:
    // Конструктор
    ElectricalCircuitCalculator() {}

    // Предки в СНМ
    QMap<QPoint, QPoint> dsuParent;
    // Размеры компонент в СНМ
    QMap<QPoint, int> dsuCompSz;
    // Точки-лидеры для каждой компоненты
    QVector<QPoint> compLeader;
    // Номер компоненты по точке-лидеру
    QMap<QPoint, int> compByLeader;
    // Количество вершин в схеме
    int nodesCount;
    // Элементы и номера вершин, к которым они подключены
    QVector<QPair<ElectricalElement *, QPair<int, int>>> elements;
    // Источники напряжения
    QVector<QPair<VoltageSource *, std::tuple<int, int, int>>> voltageSources;
    // Источники тока
    QVector<QPair<CurrentSource *, QPair<int, int>>> currentSources;
    // Потенциалы в вершинах схемы
    QVector<double> nodesPotential;
    // Токи через источники напряжения
    QVector<double> voltageSourcesCurrents;

    // Нахождение лидера компоненты в СНМ
    QPoint dsuLeader(QPoint u);
    // Объединение двух компонент в СНМ
    void dsuUnite(QPoint u, QPoint v);
    // Нахождение вершин и элементов схемы
    bool findNodesAndElements(QString &error);
    // Нахождение напряжений и токов
    bool findVoltagesAndCurrents(QString &error);
    // Решение системы линейных уравнений
    int solveLinearEquations(QVector<QVector<double>> a, QVector<double> &res);
};
