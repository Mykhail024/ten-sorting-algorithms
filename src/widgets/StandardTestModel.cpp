#include <QThread>

#include "sort.h"

#include "AbstractSourceModel.h"
#include "AlgorithmHandler.h"

#include "AbstractTestModel.h"
#include "StandardTestModel.h"

using namespace std::chrono;

StandardTestModel::StandardTestModel(QObject *parent) : AbstractTestModel(parent)
{
}

int StandardTestModel::progressMaxVal() const 
{
    return m_progress_max;
}

int StandardTestModel::progressVal() const
{
    return m_progress;
}

QString StandardTestModel::progressText() const
{
    return m_progress_text;
}

void StandardTestModel::run()
{
    m_results.clear();
    const auto alg_count = algorithmHandler().algorithmsCount();
    for(size_t i = 0; i < alg_count; ++i) {
        if(algorithmHandler().isEnabled(i)) {
            testAlgorithm(algorithmHandler().algorithm(i));
        }
    }
    setProgressText("Test complete");
    setProgressVal(m_progress_max);

    if(m_results.size() > 0) {
        emit onTestFinished(m_results);
    }
}

void StandardTestModel::testAlgorithm(AlgorithmHandler::Algorithm alg)
{
    QVector<double> swaps;
    QVector<double> times;

    const auto it_count = iterationCount();
    const auto vec_count = sourceModel()->count();

    for(size_t i = 0; i < vec_count; ++i) {
        double _swaps = 0; 
        double _times = 0;
        setProgressVal(++m_progress);
        setProgressText(QString(AbstractTestModel::tr("Testing %1 (%2 of %3)").arg(alg.name).arg(i+1).arg(vec_count)));
        for(size_t j = 0; j < it_count && !isInterruptionRequested(); ++j) {
            auto vec = sourceModel()->array(i);

            const auto start = high_resolution_clock::now();

            _swaps += alg.func(vec); 

            const auto stop = high_resolution_clock::now();
            _times += duration_cast<nanoseconds>(stop - start).count();
        }
        swaps.push_back(_swaps/it_count);
        times.push_back(_times/it_count);
    }

    addResult({.times = times, .swaps = swaps, .algorithm = alg});
}

bool StandardTestModel::startTest()
{
    if (sourceModel() == nullptr) {
        Log_Warning("No source model");
        return false;
    }
    if(sourceModel()->keys().empty()) {
        Log_Warning("Keys vector empty");
        return false;
    }
    if(sourceModel()->arrays().empty()) {
        Log_Warning("Test data empty");
        return false;
    }
    if(sourceModel()->arrays().size() != sourceModel()->keys().size()) {
        Log_Warning("Data size != keys size");
        return false;
    }

    setProgressMaxVal((sourceModel()->arrays().size() * iterationCount()) * algorithmHandler().enabledCount());
    setProgressVal(0);

    m_results.clear();

    QThread::start();
    emit onTestStart();

    return true;
}

bool StandardTestModel::cancelTest()
{
    requestInterruption();
    if(!wait(3000)) {
        Log_Warning("Cancel thread deadline ended! Terminating...");
        terminateTest();
        Log_Warning("Terminated");
        emit onTestTerminate();
        return true;
    }
    emit onTestCancel();
    return true;
}

void StandardTestModel::terminateTest()
{
    QThread::terminate();
    emit onTestTerminate();
}

size_t StandardTestModel::resultsCount() const
{
    return m_results.size();
}

AbstractTestModel::_result StandardTestModel::result(const size_t &index) const
{
    return m_results[index];
}

void StandardTestModel::setProgressMaxVal(const int &val)
{
    m_mutex.lock();
    m_progress_max = val;
    m_mutex.unlock();
    emit progressMaxValueUpdate(val);
}

void StandardTestModel::setProgressVal(const int &val)
{
    m_mutex.lock();
    m_progress = val;
    m_mutex.unlock();
    emit progressValueUpdate(val);
}

void StandardTestModel::setProgressText(const QString &text)
{
    m_mutex.lock();
    m_progress_text = text;
    m_mutex.unlock();
    emit progressTextUpdate(m_progress_text);
}

void StandardTestModel::addResult(_result r)
{
    m_mutex.lock();
    m_results.push_back(r);
    m_mutex.unlock();
}
