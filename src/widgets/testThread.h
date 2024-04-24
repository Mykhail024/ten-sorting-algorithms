#pragma once

#include <QVector>
#include <QThread>

#include "sort.h"

class MainWindow;

typedef const size_t (*SortFunction)(std::vector<int>&);

struct testResult {
    QVector<double> swaps;
    QVector<double> time;
    QString name;
};
struct testResults {
    QVector<double> keys;
    QVector<testResult> results;
};
struct testAlgorithm {
    QString name;
    SortFunction func;
};

const static QVector<testAlgorithm> algorithms {
    {"Buble", bubleSort},
        {"Insertion", insertionSort},
        {"Selection", selectSort},
        {"Cycle", cycleSort},
        {"Quick", quickSort},
        {"Merge", mergeSort},
        {"Gnome", gnomeSort},
        {"Stooge", stoogeSort},
        {"Heap", heapSort},
        {"Pigeonhole", pigeonholeSort}
};

class TestThread : public QThread
{
    Q_OBJECT
    public:
        TestThread(const MainWindow *window, QObject *parent = nullptr);
        void setData(std::vector<int> vectors);

    signals:
        void started();
        void finished(testResults results);

        void progressValChanged(const int &val);
        void progressTextChanged(const QString &text);
        void progressMaxValChanged(const int &val);

    protected:
        void run() override;
    private:
        testResults startTest();
        testResult testAlg(const QVector<double> &keys, const testAlgorithm &alg, int &progress);

        int m_delay = 0;
        int m_itCount = 0;
        std::vector<int> m_vectors;
        

        const MainWindow *m_window;
};
