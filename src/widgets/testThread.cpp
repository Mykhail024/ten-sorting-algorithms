#include <chrono>

#include "sort.h"
#include "MainWindow.h"

#include "testThread.h"

using namespace std::chrono;



TestThread::TestThread(const MainWindow *window, QObject *parent) : QThread(parent)
{
    m_window = window;
}

void TestThread::run()
{
    emit started();
    emit(startTest());
}

testResults startTest()
{

}

testResult TestThread::testAlg(const QVector<double> &keys, const testAlgorithm &alg, int &progress)
{
    const int itCount = m_window->m_iterationsCount->text().toInt();
    const int min = m_window->m_min->text().toInt();
    const int max = m_window->m_max->text().toInt();

    QVector<double> swaps;
    QVector<double> times;

    for(const auto &i : keys) {
        double averageSwaps = 0;
        double averageTime = 0;
        progressTextChanged(QString(tr("Testing %1 (%2 of %3)")).arg(alg.name).arg(i).arg(keys.last()));
        progressValChanged(++progress);
        for(size_t j = 0; j < itCount; ++j) {
            auto vec = generateVec(i, min, max);

            const auto start = high_resolution_clock::now();
            averageSwaps += alg.func(vec);
            const auto stop = high_resolution_clock::now();

            averageTime += duration_cast<nanoseconds>(stop - start).count();
        }
        swaps.push_back(averageSwaps/itCount);
        times.push_back(averageTime/itCount);
    }
    return {.swaps = swaps, .time = times, .name = alg.name};
}

void TestThread::setData(std::vector<int> vectors)
{
    if(!this->isRunning()) {
        m_vectors = vectors;
    }
}
