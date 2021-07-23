#include <QQueue>
#include "ElectricalCircuitCalculator.h"
#include "ElectricalElementsManager.h"

// Нахождение лидера компоненты в СНМ
QPoint ElectricalCircuitCalculator::dsuLeader(QPoint u)
{
    // Нахождение лидера с эвристикой сжатия путей
    return (dsuParent[u] == u) ? u : (dsuParent[u] = dsuLeader(dsuParent[u]));
}

// Объединение двух компонент в СНМ
void ElectricalCircuitCalculator::dsuUnite(QPoint u, QPoint v)
{
    // Нахождение лидеров компонент
    u = dsuLeader(u);
    v = dsuLeader(v);
    // Если это одна и та же компонента, то не нужно объединять
    if (u == v)
        return;

    // Добавление меньшей к большей
    if (dsuCompSz[u] > dsuCompSz[v])
        std::swap(u, v);
    dsuParent[u] = v;
    dsuCompSz[v] += dsuCompSz[u];
}

// Нахождение вершин схемы
bool ElectricalCircuitCalculator::findNodesAndElements(QString& error)
{
    // Очищение СНМ
    dsuParent.clear();
    dsuCompSz.clear();
    // Очищение компонент
    nodesCount = 0;
    compLeader.clear();
    compByLeader.clear();
    // Очищение элементов и источников напряжения и тока
    elements.clear();
    voltageSources.clear();
    currentSources.clear();
    // Очищение потенциалов и токов через источники напряжения
    nodesPotential.clear();
    voltageSourcesCurrents.clear();

    // Каждой точке сетки сопоставляются элементы, подключённые к ней
    QMap<QPoint, QVector<ElectricalElement*>> allPoints;
    for (int orient = 0; orient < 2; orient++) // 2 ориентации
        for (auto el : ElectricalElementsManager::getInstance().getElements()[orient]) // Все элементы с данной ориентацией
            for (auto p : el->getEndPoints()) // Все крайние точки элемента
                allPoints[p].push_back(el);

    // Инициализация СНМ
    for (auto it = allPoints.cbegin(); it != allPoints.cend(); it++) // Все используемые точки сетки
    {
        dsuParent[it.key()] = it.key();
        dsuCompSz[it.key()] = 1;
    }

    // Объединение точек сетки, образующих компоненты связности (по проводам и замкнутым выключателям)
    for (auto it = allPoints.cbegin(); it != allPoints.cend(); it++)
        for (auto el : it.value())  // Все элементы в данной точке
            if (el->getResistance() == 0.0 && dynamic_cast<VoltageSource*>(el) == nullptr)  // Элемент - провод или замкнутый выключатель
                for (auto p : el->getEndPoints()) // Все крайние точки провода
                    if (p != it.key())  // Объединение со второй точкой провода
                        dsuUnite(it.key(), p);

    // Нахождение компонент по точкам-лидерам и наоборот
    for (auto it = allPoints.cbegin(); it != allPoints.cend(); it++) // Все используемые точки сетки
        if (dsuLeader(it.key()) == it.key())  // Если точка и есть лидер своей компоненты
        {
            compByLeader[it.key()] = compLeader.size();
            compLeader.push_back(it.key());
        }
    nodesCount = compLeader.size();

    // Заполнение информации о вершинах схемы
    for (auto it = allPoints.cbegin(); it != allPoints.cend(); it++) // Все используемые точки сетки
        for (auto el : it.value()) // Все элементы в данной точке
        {
            // Провода и выключатели не рассматриваются
            if ((el->getResistance() == 0.0 && dynamic_cast<VoltageSource*>(el) == nullptr) ||
                (std::isinf(el->getResistance()) && dynamic_cast<CurrentSource*>(el) == nullptr))
                continue;
            // Крайние точки элемента
            auto endPoints = el->getEndPoints();
            QPoint firstPoint = endPoints[0], secondPoint = endPoints[1];
            // Если элемент подключён обеими точками к одной вершине схемы
            if (dsuLeader(firstPoint) == dsuLeader(secondPoint))
            {
                error = QString(u8"Элемент замкнут проводником!");
                return false;
            }

            // Элемент - источник напряжения (рассмотрение один раз)
            auto vs = dynamic_cast<VoltageSource*>(el);
            if (it.key() == firstPoint && vs != nullptr)
            {
                // Добавление фиктивной вершины, если не идеальный источник (источник напряжения можно
                // представить как идеальный и. н. и резистор внутреннего сопротивления, подключённый последовательно)
                int midNode = -1;
                if (vs->getResistance() != 0.0)
                {
                    midNode = nodesCount;
                    nodesCount++;
                }
                // Добавление источника напряжения и вершин, к которым он подключён, в список
                voltageSources.append(QPair<VoltageSource*, std::tuple<int, int, int>>(vs, std::tuple<int, int, int>
                    (compByLeader[dsuLeader(firstPoint)], midNode, compByLeader[dsuLeader(secondPoint)])));
                continue;
            }

            // Элемент - источник тока (рассмотрение один раз)
            auto cs = dynamic_cast<CurrentSource*>(el);
            if (it.key() == firstPoint && cs != nullptr)
            {
                // Добавление источника тока и вершин, к которым он подключён, в список
                currentSources.append(QPair<CurrentSource*, QPair<int, int>>(cs, QPair<int, int>
                    (compByLeader[dsuLeader(firstPoint)], compByLeader[dsuLeader(secondPoint)])));
                continue;
            }

            // Добавление элемента и вершин, к которым он подключён, в список (один раз)
            if (it.key() == firstPoint)
                elements.append(QPair<ElectricalElement*, QPair<int, int>>(el, QPair<int, int>
                    (compByLeader[dsuLeader(firstPoint)], compByLeader[dsuLeader(secondPoint)])));
        }

    return true;
}

// Нахождение напряжений и токов
bool ElectricalCircuitCalculator::findVoltagesAndCurrents(QString& error)
{
    // Нахождение напряжений с помощью алгоритма MNA (https://lpsa.swarthmore.edu/Systems/Electrical/mna/MNA3.html)
    // Количество вершин схемы кроме 0 - земли
    int n = nodesCount - 1;
    // Количество идеальных источников напряжения
    int m = voltageSources.size();
    // Проверка количеств
    if (n <= 0)
    {
        error = QString(u8"Недостаточно вершин в схеме!");
        return false;
    }
    if (m == 0 && currentSources.size() == 0)
    {
        error = QString(u8"В схеме нет источников напряжения или тока!");
        return false;
    }
    // Матрица A (последний столбец - матрица z)
    QVector<QVector<double>> a(n + m, QVector<double>(n + m + 1, 0.0));
    // Подматрица G - определяется пассивными элементами (резисторами)
    for (const auto& pr : elements)
    {
        // Вершины, к которым подключён элемент
        int i = pr.second.first, j = pr.second.second;
        // Проводимость элемента
        double conductance = 1.0 / pr.first->getResistance();
        // Добавление на диагональ
        if (i != 0)
            a[i - 1][i - 1] += conductance;
        if (j != 0)
            a[j - 1][j - 1] += conductance;
        // Вычитание в (i, j) и (j, i)
        if (i != 0 && j != 0)
        {
            a[i - 1][j - 1] -= conductance;
            a[j - 1][i - 1] -= conductance;
        }
    }

    // Номер источника напряжения
    int vsInd = 0;
    // Все источники напряжения
    for (const auto& pr : voltageSources)
    {
        // Вершины, к которым подключён элемент
        int i, j, k;
        std::tie(i, j, k) = pr.second;
        // Если есть внутреннее сопротивление
        if (j != -1)
        {
            // Проводимость элемента
            double conductance = 1.0 / pr.first->getResistance();
            // Добавление на диагональ
            if (i != 0)
                a[i - 1][i - 1] += conductance;
            if (j != 0)
                a[j - 1][j - 1] += conductance;
            // Вычитание в (i, j) и (j, i)
            if (i != 0 && j != 0)
            {
                a[i - 1][j - 1] -= conductance;
                a[j - 1][i - 1] -= conductance;
            }
        }

        // Заполнение подматриц B и C
        if (j == -1) // Если нет внутреннего сопротивления
        {
            if (i != 0)
                a[i - 1][n + vsInd] = a[n + vsInd][i - 1] = 1;
        }
        else
        {
            if (j != 0)
                a[j - 1][n + vsInd] = a[n + vsInd][j - 1] = 1;
        }
        if (k != 0)
            a[k - 1][n + vsInd] = a[n + vsInd][k - 1] = -1;

        // Заполнение матрицы z
        a[n + vsInd][n + m] = pr.first->getVoltage();

        vsInd++;
    }

    // Номер источника тока
    int csInd = 0;
    // Все источники тока
    for (const auto& pr : currentSources)
    {
        // Вершины, к которым подключён элемент
        int i = pr.second.first, j = pr.second.second;
        // Если есть внутреннее сопротивление
        if (!std::isinf(pr.first->getResistance()))
        {
            // Проводимость элемента
            double conductance = 1.0 / pr.first->getResistance();
            // Добавление на диагональ
            if (i != 0)
                a[i - 1][i - 1] += conductance;
            if (j != 0)
                a[j - 1][j - 1] += conductance;
            // Вычитание в (i, j) и (j, i)
            if (i != 0 && j != 0)
            {
                a[i - 1][j - 1] -= conductance;
                a[j - 1][i - 1] -= conductance;
            }
        }

        // Заполнение матрицы z
        if (i != 0)
            a[i - 1][n + m] += pr.first->getCurrent();
        if (j != 0)
            a[j - 1][n + m] -= pr.first->getCurrent();

        csInd++;
    }

    // Матрица x - результат
    QVector<double> x(n + m, 0.0);
    if (solveLinearEquations(a, x) != 1)
    {
        error = QString(u8"Невозможно вычислить характеристики схемы (нет единственного решения)!");
        return false;
    }
    // Заполнение потенциалов для вершин и токов через источники напряжения
    nodesPotential.resize(nodesCount);
    voltageSourcesCurrents.resize(m);
    for (int i = 0; i < n; i++)
        nodesPotential[i + 1] = x[i];
    for (int i = 0; i < m; i++)
        voltageSourcesCurrents[i] = abs(x[n + i]);

    // Обновление состояний элементов в зависимости от тока
    for (const auto& pr : elements)
    {
        // Вычисление тока
        double current = abs(nodesPotential[pr.second.first] - nodesPotential[pr.second.second]) / pr.first->getResistance();
        // Обновление состояния
        pr.first->onCurrentFlow(current);
    }
    return true;
}

// Решение системы линейных уравнений
int ElectricalCircuitCalculator::solveLinearEquations(QVector<QVector<double>> a, QVector<double>& res)
{
    // Погрешность
    const double EPS = 1e-9;

    // Количество уравнений
    int n = a.size();
    // Количество неизвестных
    int m = a[0].size() - 1;

    // Номер строки, в которой будет результат для i-й переменной
    QVector<int> where(m, -1);
    // Проход по столбцам
    for (int col = 0, row = 0; col < m && row < n; col++)
    {
        // Номер строки с максимальным в столбе числом
        int sel = row;
        for (int i = row; i < n; i++)
            if (std::abs(a[i][col]) > std::abs(a[sel][col]))
                sel = i;
        // Если все числа в столбце равны 0, то переход к следующему столбцу
        if (std::abs(a[sel][col]) < EPS)
            continue;
        // Обмен текущей строки и строки с максимумом
        for (int i = col; i <= m; i++)
            std::swap(a[sel][i], a[row][i]);
        // Решение для переменной - текущая строка
        where[col] = row;
        // Проход по всем строкам, кроме текущей
        for (int i = 0; i < n; i++)
            if (i != row)
            {
                // Вычитание текущей строки из i-й, чтобы коэффициенты обращались в нули
                double c = a[i][col] / a[row][col];
                for (int j = col; j <= m; ++j)
                    a[i][j] -= a[row][j] * c;
            }
        // Переход к следующей строке
        row++;
    }

    // Если для переменной определена строка с решением, то вычисление переменной
    for (int i = 0; i < m; i++)
        if (where[i] != -1)
            res[i] = a[where[i]][m] / a[where[i]][i];
    // Проверка правильности решения
    for (int i = 0; i < n; i++)
    {
        double sum = 0;
        for (int j = 0; j < m; j++)
            sum += res[j] * a[i][j];
        if (abs(sum - a[i][m]) > EPS)
            return 0; // Если не равно, то решения нет
    }

    // Если есть переменная, для которой не определено решение, то это независимая переменная
    for (int i = 0; i < m; i++)
        if (where[i] == -1)
            return -1; // Бесконечное число решений
    // Решение единственно
    return 1;
}

// Вычисление характеристик схемы
bool ElectricalCircuitCalculator::calculateCircuit(QString& error)
{
    // Нахождение вершин и элементов схемы
    if (!findNodesAndElements(error))
        return false;
    // Нахождение напряжений и токов
    if (!findVoltagesAndCurrents(error))
        return false;
    return true;
}

// Нахождение напряжения между двумя точками - разность потенциалов
bool ElectricalCircuitCalculator::getVoltage(const QPoint& loc1, const QPoint& loc2, double& res)
{
    // Проверки принадлежности точек схеме
    if (!dsuParent.contains(loc1) || !dsuParent.contains(loc2))
        return false;
    QPoint leader1 = dsuLeader(loc1), leader2 = dsuLeader(loc2);
    if (!compByLeader.contains(leader1) || !compByLeader.contains(leader2))
        return false;
    int comp1 = compByLeader[leader1], comp2 = compByLeader[leader2];
    // Вычисление результата
    res = nodesPotential[comp1] - nodesPotential[comp2];
    return true;
}

// Нахождение тока через элемент
bool ElectricalCircuitCalculator::getCurrent(const QPoint& loc, Qt::Orientation orientation, double& res)
{
    // Проверка существования элемента
    auto el = ElectricalElementsManager::getInstance().getElement(loc, orientation);
    if (el == nullptr)
        return false;
    // Если элемент - источник напряжения
    for (int i = 0; i < voltageSources.size(); i++)
        if (voltageSources[i].first == el)
        {
            res = voltageSourcesCurrents[i];
            return true;
        }
    // Если элемент - источник тока
    for (int i = 0; i < currentSources.size(); i++)
        if (currentSources[i].first == el)
        {
            // Если идеальный источник
            if (std::isinf(el->getResistance()))
                res = currentSources[i].first->getCurrent();
            else
            {
                // Иначе вычисление по закону Ома
                if (!getVoltage(el->getEndPoints()[0], el->getEndPoints()[1], res))
                    return false;
                res = currentSources[i].first->getCurrent() - abs(res / el->getResistance());
            }
            return true;
        }
    // Проверка сопротивления элемента
    if (el->getResistance() == 0.0 || std::isinf(el->getResistance()))
        return false;
    // Иначе вычисление по закону Ома
    if (!getVoltage(el->getEndPoints()[0], el->getEndPoints()[1], res))
        return false;
    res = abs(res / el->getResistance());
    return true;
}