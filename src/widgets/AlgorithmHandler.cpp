#include <QMutexLocker>

#include "sort.h"

#include "AlgorithmHandler.h"

AlgorithmHandler::AlgorithmHandler()
    : QObject()
    , m_algorithms({
        {{bubbleSort, tr("Bubble")}, false},
        {{insertionSort, tr("Insertion")}, false},
        {{selectionSort, tr("Selection")}, false},
        {{cycleSort, tr("Cycle")}, false},
        {{quickSort, tr("Quick")}, false},
        {{mergeSort, tr("Merge")}, false},
        {{heapSort, tr("Heap")}, false},
        {{gnomeSort, tr("Gnome")}, false},
        {{stoogeSort, tr("Stooge")}, false},
        {{pigeonholeSort, tr("Pigeonhole")}, false}
    })
{
}

AlgorithmHandler::Algorithm AlgorithmHandler::algorithm(const size_t &index) const
{
    return std::get<ALGORITHM>(m_algorithms[index]);
}

size_t AlgorithmHandler::algorithmsCount() const
{
    return m_algorithms.size();
}

bool AlgorithmHandler::registerAlgorithm(AlgorithmHandler::AlgorithmFunction func, const QString &name)
{
    QMutexLocker locker(&m_mutex);
    if(func == nullptr || name.isEmpty()) return false;

    Algorithm new_algorithm = {func, name};

    m_algorithms.push_back({new_algorithm, false});

    emit onNewAlgorithmRegister(new_algorithm);

    return true;
}

bool AlgorithmHandler::unregisterAlgorithm(const size_t &index)
{
    QMutexLocker locker(&m_mutex);
    if(index < 0 || index >= m_algorithms.size()) return false;

    auto [temp, enabled] = m_algorithms[index];

    m_algorithms.erase(m_algorithms.begin() + index);

    if(enabled) {
        emit onAlgorithmDisable(temp);
    }
    emit onNewAlgorithmUnregister(temp);

    return true;
}

size_t AlgorithmHandler::enabledCount() const
{
    size_t count = 0;
    for(const auto &i : m_algorithms) {
        count += std::get<IS_ENABLED>(i);
    }
    return count;
}

void AlgorithmHandler::setEnabled(const size_t &index, const bool &enabled)
{
    if(index >= m_algorithms.size()) {
        Log_Warning("No algorithm index: " + QString::number(index));
        return;
    }

    Log_Debug("Set algorithm " + std::get<ALGORITHM>(m_algorithms[index]).name + " enabled to: " + (enabled ? "\033[1;32mtrue\033[0m" : "\033[1;33mfalse\033[0m"));

    QMutexLocker locker(&m_mutex);
    std::get<IS_ENABLED>(m_algorithms[index]) = enabled;
}

void AlgorithmHandler::setEnabled(const AlgorithmHandler::Algorithm &algorithm, const bool &enabled)
{
    const auto finded = std::find_if(m_algorithms.begin(), m_algorithms.end(), [&](auto&& val){ return std::get<ALGORITHM>(val) == algorithm; });
    if(finded != m_algorithms.end()) {
        setEnabled(std::distance(m_algorithms.begin(), finded), enabled);
    } else {
        Log_Warning("No Algorithm: " + algorithm.name);
    }
}

void AlgorithmHandler::setEnabled(const QString &name, const bool &enabled)
{
    const auto finded = std::find_if(m_algorithms.begin(), m_algorithms.end(), [&](auto&& val){ return std::get<ALGORITHM>(val).name == name; });
    if(finded != m_algorithms.end()) {
        setEnabled(std::distance(m_algorithms.begin(), finded), enabled);
    } else {
        Log_Warning("No algorithm named " + name);
    }
}

bool AlgorithmHandler::isEnabled(const size_t &index) const
{
    if(index >= m_algorithms.size()) {
        Log_Warning("No algorithm index: " + QString::number(index));
        return false;
    }
    return std::get<IS_ENABLED>(m_algorithms[index]);
}

AlgorithmHandler& algorithmHandler()
{
    static AlgorithmHandler INSTANCE;
    return INSTANCE;
}
