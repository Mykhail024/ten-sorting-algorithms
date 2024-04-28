#include <chrono>

#include "sort.h"
#include "Log.h"

#include "testThread.h"

using namespace std::chrono;



TestThread::TestThread(QObject *parent) : QThread(parent)
{
}

void TestThread::run()
{
    emit started();
    emit finished(startTest());
}

std::vector<testResult> TestThread::startTest()
{
    Log_Debug("Test started");
    if(m_vectors.empty()) {
        Log_Error("Test data is empty");
    }

    std::vector<testResult> results;
    int progress = 0;

    emit progressMaxValChanged(m_vectors.size()*m_enabled.size());
    
    for(const auto alg : m_enabled) {
        if(alg.second) {
            results.push_back(testAlg(alg.first, progress));
        }
    }

    return results;
}

testResult TestThread::testAlg(ALGORITHMS algorithm, int &progress)
{
    QVector<double> swaps;
    QVector<double> times;

    for(auto it = m_vectors.begin(); it != m_vectors.end(); ++it) {
        double _swaps = 0; 
        double _times = 0;
        emit progressValChanged(++progress);
        emit progressTextChanged(QString(tr("Testing %1 (%2 of %3)")).arg(algorithmName(algorithm)).arg(it - m_vectors.begin()).arg(m_vectors.size()-1));
        for(size_t i = 0; i < m_itCount; ++i) {
            const auto func = algorithmFunction(algorithm);
            auto vec = *it;

            const auto start = high_resolution_clock::now();
            _swaps += func(vec); 
            const auto stop = high_resolution_clock::now();
            _times += duration_cast<nanoseconds>(stop - start).count();
        }
        swaps.push_back(_swaps/m_itCount);
        times.push_back(_times/m_itCount);
    }

    return {swaps, times, algorithm};
}

void TestThread::setData(std::vector<std::vector<int>> vectors)
{
    if(!this->isRunning() && !vectors.empty()) {
        m_vectors = vectors;
    }
}

void TestThread::enableAlgorithm(ALGORITHMS algorithm, const bool enable)
{
   m_enabled[algorithm] = enable; 
}

QString TestThread::algorithmName(ALGORITHMS algorithm)
{
    switch (algorithm) {
        case BUBLE:
            return "Buble";
        case INSERTION:
            return "Insertion";
        case SELECTION:
            return "Selection";
        case CYCLE:
            return "Cycle";
        case QUICK:
            return "Quick";
        case MERGE:
            return "Merge";
        case GNOME:
            return "Gnome";
        case STOOGE:
            return "Stooge";
        case HEAP:
            return "Heap";
        case PIGEONHOLE:
            return "Pigeonhole";
        case ERROR:
            return "Error";
    }
}

SortFunction TestThread::algorithmFunction(ALGORITHMS algorithm)
{
    switch (algorithm) {
        case BUBLE:
            return bubleSort;
        case INSERTION:
            return insertionSort;
        case SELECTION:
            return selectionSort;
        case CYCLE:
            return cycleSort;
        case QUICK:
            return quickSort;
        case MERGE:
            return mergeSort;
        case GNOME:
            return gnomeSort;
        case STOOGE:
            return stoogeSort;
        case HEAP:
            return heapSort;
        case PIGEONHOLE:
            return pigeonholeSort;
        case ERROR:
            return nullptr;
    }
}

void TestThread::setIterationsCount(const int &count)
{
    m_itCount = count;
}

void TestThread::setDelay(const int &delay)
{
    m_delay = delay;
}

ALGORITHMS TestThread::algorithmFromString(const QString &name)
{
    if(name == "Buble")
        return BUBLE;
    else if(name == "Insertion")
        return INSERTION;
    else if(name == "Selection")
        return SELECTION;
    else if(name == "Cycle")
        return CYCLE;
    else if(name == "Quick")
        return QUICK;
    else if(name == "Merge")
        return MERGE;
    else if(name == "Gnome")
        return GNOME;
    else if(name == "Stooge")
        return STOOGE;
    else if(name == "Heap")
        return HEAP;
    else if(name == "Pigeonhole")
        return PIGEONHOLE;
    else return ERROR;
}
