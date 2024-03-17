#include <QApplication>
#include <QValidator>
#include <QtConcurrent/QtConcurrent>
#include <QThread>
#include <QRandomGenerator>

#include <climits>

#include "qcustomplot.h"
#include "sort.h"

#include "MainWindow.h"

using namespace std::chrono;

const QVector<QColor> colors = {"black", "red", "blue", "#30f930", "purple", "orange", "brown", "gray", "green", "#7f1212"};
const QVector<TestThread::algorithm> algorithms = {
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

TestThread::TestThread(const MainWindow *window, QObject *parent) : QThread(parent)
{
    m_window = window;
}

void TestThread::run()
{
    emit started();

    const int start = m_window->m_startIt->text().toInt();
    const int stop = m_window->m_endIt->text().toInt();
    const int step = m_window->m_stepIt->text().toInt();
    const int itCount = m_window->m_iterationsCount->text().toInt();

    testResults result;

    progressMaxValChanged(1);
    progressValChanged(1);
    progressTextChanged("Generating keys");

    for(int i = start; i <= stop; i += step) {
        result.keys.push_back(i);
    }
    if(result.keys.last() != stop) result.keys.push_back(stop);

    const int maxProgress = (result.keys.size() * itCount * algorithms.count()) + 1;
    progressMaxValChanged(maxProgress);
    progressValChanged(1);
    int currentProgress = 1;

    for(const auto algorithm : algorithms) {
        result.data.push_back(test(result.keys, algorithm, currentProgress));
    }

    progressValChanged(maxProgress);
    progressTextChanged("Testing finished!");

    emit finished(result);
}

TestThread::testResult TestThread::test(const QVector<double> &keys, const algorithm &alg, int &progress)
{
    const int itCount = m_window->m_iterationsCount->text().toInt();
    const int min = m_window->m_min->text().toInt();
    const int max = m_window->m_max->text().toInt();

    QVector<double> result;

    for(const auto &i : keys) {
        double average = 0;
        for(size_t j = 0; j < itCount; ++j) {
            progressTextChanged(QString("Testing %1. %2 of %3 (%4/%5)").arg(alg.name).arg(i).arg(keys.last()).arg(j+1).arg(itCount));
            progressValChanged(++progress);
            auto vec = generateVec(i, min, max);
            average += alg.func(vec);
        }
        result.push_back(average/itCount);
    }
    return {result, alg.name};
}

MainWindow::MainWindow()
    : QWidget()
    , m_hlayout(new QHBoxLayout(this))
    , m_vlayout(new QVBoxLayout())
    , m_inputsLayout(new QHBoxLayout())
    , m_leftLayout(new QVBoxLayout())
    , m_rightLayout(new QVBoxLayout())
    , m_testButton(new QPushButton("Test", this))
    , m_iterationsCount(new QLineEdit(this))
    , m_startIt(new QLineEdit(this))
    , m_endIt(new QLineEdit(this))
    , m_stepIt(new QLineEdit(this))
    , m_min(new QLineEdit(this))
    , m_max(new QLineEdit(this))
    , m_progressBar(new QProgressBar(this))
    , m_testThread(new TestThread(this, this))
{
    this->setWindowTitle("Sorting Benchmark");

    auto *validator = new QIntValidator(0, INT_MAX, this);

    m_iterationsCount->setValidator(validator);
    m_startIt->setValidator(validator);
    m_endIt->setValidator(validator);
    m_stepIt->setValidator(validator);
    m_min->setValidator(validator);
    m_max->setValidator(validator);

    m_progressBar->setTextVisible(true);
    m_progressBar->setFormat("Press test");
    m_progressBar->setAlignment(Qt::AlignCenter);
    m_progressBar->setMinimum(0);
    m_progressBar->setMaximum(1);
    m_progressBar->setValue(1);
    m_progressBar->setMinimumWidth(350);

    m_leftLayout->addWidget(new QLabel("Numbers on first array", this));
    m_leftLayout->addWidget(m_startIt);
    m_leftLayout->addWidget(new QLabel("Numbers on last array", this));
    m_leftLayout->addWidget(m_endIt);
    m_leftLayout->addWidget(new QLabel("Min number in array", this));
    m_leftLayout->addWidget(m_min);
    m_leftLayout->addSpacerItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Expanding));
    m_rightLayout->addWidget(new QLabel("Iteration Count", this));
    m_rightLayout->addWidget(m_iterationsCount);
    m_rightLayout->addWidget(new QLabel("Step", this));
    m_rightLayout->addWidget(m_stepIt);
    m_rightLayout->addWidget(new QLabel("Max number in array", this));
    m_rightLayout->addWidget(m_max);
    m_rightLayout->addSpacerItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Expanding));

    m_inputsLayout->addLayout(m_leftLayout);
    m_inputsLayout->addLayout(m_rightLayout);

    m_vlayout->addLayout(m_inputsLayout);
    m_vlayout->addWidget(m_progressBar);
    m_vlayout->addWidget(m_testButton, 0, Qt::AlignTop);

    m_hlayout->addLayout(m_vlayout);

    connect(m_testButton, &QPushButton::clicked, this, &MainWindow::test);
    connect(m_testThread, &TestThread::started, this, [&]{ setControlsEnabled(false); });
    connect(m_testThread, &TestThread::finished, this, &MainWindow::testEnd);
    connect(m_testThread, &TestThread::progressValChanged, m_progressBar, &QProgressBar::setValue);
    connect(m_testThread, &TestThread::progressTextChanged, m_progressBar, &QProgressBar::setFormat);
    connect(m_testThread, &TestThread::progressMaxValChanged, m_progressBar, &QProgressBar::setMaximum);

    connect(m_min, &QLineEdit::textChanged, this, &MainWindow::validateInt);
    connect(m_max, &QLineEdit::textChanged, this, &MainWindow::validateInt);
}

void MainWindow::validateInt(const QString &text)
{
    QLineEdit *lineEdit = qobject_cast<QLineEdit*>(sender());
    if(!lineEdit) return;
    bool ok;
    text.toInt(&ok);
    if(!ok) lineEdit->setText(QString::number(INT_MAX-1));
}

void MainWindow::test()
{
    if(m_iterationsCount->text().isEmpty()
            || m_startIt->text().isEmpty()
            || m_endIt->text().isEmpty()
            || m_stepIt->text().isEmpty()
            || m_min->text().isEmpty()
            || m_max->text().isEmpty()
            ) return;

    m_testThread->start(QThread::HighestPriority);
}


void MainWindow::testEnd(TestThread::testResults results)
{
    if(m_plot) {
        m_plot->deleteLater();
    }

    m_plot = new QCustomPlot(this);
    m_plot->setMinimumHeight(500);
    m_plot->setMinimumWidth(500);

    m_plot->yAxis->setLabel("Iteration count");
    m_plot->xAxis->setLabel("Digits in Vector");
    m_plot->legend->setFillOrder(QCPLegend::foColumnsFirst);

    int colorIt = 0;
    for(const auto &vec : results.data) {
        m_plot->addGraph();
        const auto i = m_plot->graphCount()-1;
        m_plot->graph(i)->setPen(QPen(colors[colorIt++], 2));
        m_plot->graph(i)->setData(results.keys, vec.data);
        m_plot->graph(i)->setName(vec.name);
    }

    QCPLayoutGrid *subLayout = new QCPLayoutGrid;
    m_plot->plotLayout()->addElement(1, 0, subLayout);
    subLayout->setMargins(QMargins(5, 0, 5, 5));

    for (size_t i = 0; i < m_plot->legend->itemCount(); i += 4) {
        for (size_t j = i; j < i + 4 && j < m_plot->legend->itemCount(); ++j) {
            subLayout->addElement(i / 4, j - i, m_plot->legend->item(j));
        }
    }

    m_plot->plotLayout()->setRowStretchFactor(1, 0.001);

    m_plot->rescaleAxes();

    m_hlayout->addWidget(m_plot);
    setControlsEnabled(true);
}

void MainWindow::setControlsEnabled(const bool &val)
{
    m_testButton->setEnabled(val);
    m_startIt->setEnabled(val);
    m_endIt->setEnabled(val);
    m_iterationsCount->setEnabled(val);
    m_stepIt->setEnabled(val);
    m_max->setEnabled(val);
    m_min->setEnabled(val);
}
