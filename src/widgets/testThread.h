#pragma once

#include <QVector>
#include <QVector>
#include <QThread>

#include "sort.h"

class MainWindow;

typedef const size_t (*SortFunction)(std::vector<int>&);

const int ALGORITHMS_COUNT =  10;
enum ALGORITHMS {
    ERROR,
    BUBLE,
    INSERTION,
    SELECTION,
    CYCLE,
    QUICK,
    MERGE,
    GNOME,
    STOOGE,
    HEAP,
    PIGEONHOLE
};

struct testResult {
    const QVector<double> swaps;
    const QVector<double> times;
    ALGORITHMS algorithm;
};

class TestThread : public QThread
{
    Q_OBJECT
    public:
        TestThread(QObject *parent = nullptr);
        void setData(std::vector<std::vector<int>> vectors);
        void enableAlgorithm(ALGORITHMS algorithm, const bool enable = true);
        void setIterationsCount(const int &count);
        void setDelay(const int &delay);
        static QString algorithmName(ALGORITHMS algorithm);
        static SortFunction algorithmFunction(ALGORITHMS algorithm);
        static ALGORITHMS algorithmFromString(const QString &name);

    signals:
        void started();
        void finished(std::vector<testResult> results);

        void progressValChanged(const int &val);
        void progressTextChanged(const QString &text);
        void progressMaxValChanged(const int &val);

    protected:
        void run() override;
    private:
        std::vector<testResult> startTest();
        testResult testAlg(ALGORITHMS algorithm, int &progress);

        int m_delay = 0;
        int m_itCount = 0;
        std::vector<std::vector<int>> m_vectors;
        std::map<ALGORITHMS, bool> m_enabled;
};
