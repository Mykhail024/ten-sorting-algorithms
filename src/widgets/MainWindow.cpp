#include <QApplication>
#include <QValidator>
#include <QtConcurrent/QtConcurrent>
#include <QThread>
#include <QRandomGenerator>
#include <QCheckBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSpacerItem>
#include <QTableView>

#include <chrono>
#include <climits>
#include <qheaderview.h>

#include "Model.h"
#include "qcustomplot.h"
#include "sort.h"
#include "ChangeLangDialog.h"

#include "visualWidget.h"
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
    progressTextChanged(tr("Generating keys"));

    for(int i = start; i <= stop; i += step) {
        result.keys.push_back(i);
    }
    if(result.keys.last() != stop) result.keys.push_back(stop);

    QVector<TestThread::algorithm> enabled;
    for(const auto &algorithm : algorithms) {
        if(m_window->m_checkboxes[algorithm.name]->isChecked()) {
            enabled.push_back(algorithm);
        }
    }

    const int maxProgress = (result.keys.size() * enabled.size()) + 1;
    progressMaxValChanged(maxProgress);
    progressValChanged(1);
    int currentProgress = 1;

    for(const auto algorithm : enabled) {
        result.data.push_back(test(result.keys, algorithm, currentProgress));
    }
    
    progressValChanged(maxProgress);
    progressTextChanged(tr("Testing finished!"));

    emit finished(result);
}

TestThread::testResult TestThread::test(const QVector<double> &keys, const algorithm &alg, int &progress)
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
    return {swaps, times, alg.name};
}

TimeColDelegate::TimeColDelegate(QObject *parent) : QItemDelegate(parent)
{}

void TimeColDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    const auto time_ns = index.data(Qt::DisplayRole).toDouble();
    QString stime;

    if (time_ns >= 1000000000) {
        stime = QString(tr("%1 s")).arg(time_ns / 1000000000.0);
    } else if (time_ns >= 1000000) {
        stime = QString(tr("%1 ms")).arg(time_ns / 1000000.0);
    } else if (time_ns >= 1000) {
        stime = QString(tr("%1 us")).arg(time_ns / 1000.0);
    } else {
        stime = QString(tr("%1 ns")).arg(time_ns);
    }
    QRect adjustedRect = option.rect.adjusted(2, 2, -2, -2);
    painter->drawText(adjustedRect, option.displayAlignment, stime);
}

MainWindow::MainWindow()
    : QWidget()
    , m_main(new QHBoxLayout(this))
    , m_right(new QVBoxLayout())
    , m_left(new QVBoxLayout())
    , m_tabMain(new QTabWidget(this))
    , m_tabGraphs(new QTabWidget(this))
    , m_tabTables(new QTabWidget(this))
    , m_testButton(new QPushButton(tr("Test"), this))
    , m_startIt(new QLineEdit(this))
    , m_endIt(new QLineEdit(this))
    , m_min(new QLineEdit(this))
    , m_max(new QLineEdit(this))
    , m_stepIt(new QLineEdit(this))
    , m_iterationsCount(new QLineEdit(this))
    , m_progressBar(new QProgressBar(this))
    , m_testThread(new TestThread(this, this))
    , m_visBtn(new QPushButton(tr("Visual"), this))
    , m_vis(new VisualWidget())
    , m_settingsBtn(new QPushButton(tr("Change Language"), this))
{
    this->setWindowTitle(tr("Sorting Benchmark"));

    auto *validator = new QIntValidator(0, INT_MAX, this);

    m_iterationsCount->setValidator(validator);
    m_startIt->setValidator(validator);
    m_endIt->setValidator(validator);
    m_stepIt->setValidator(validator);
    m_min->setValidator(validator);
    m_max->setValidator(validator);

    m_progressBar->setTextVisible(true);
    m_progressBar->setFormat(tr("Press test"));
    m_progressBar->setAlignment(Qt::AlignCenter);
    m_progressBar->setMinimum(0);
    m_progressBar->setMaximum(1);
    m_progressBar->setValue(1);
    m_right->addWidget(new QLabel(tr("Numbers on first array"), this));
    m_right->addWidget(m_startIt);
    m_right->addWidget(new QLabel(tr("Numbers on last array"), this));
    m_right->addWidget(m_endIt);
    m_right->addWidget(new QLabel(tr("Min number in array"), this));
    m_right->addWidget(m_min);
    m_right->addWidget(new QLabel(tr("Max number in array"), this));
    m_right->addWidget(m_max);
    m_right->addWidget(new QLabel(tr("Step"), this));
    m_right->addWidget(m_stepIt);

    m_right->addWidget(new QLabel(tr("Iteration Count"), this));
    m_right->addWidget(m_iterationsCount);

    QVBoxLayout *checkbox_l = new QVBoxLayout();
    QVBoxLayout *checkbox_r = new QVBoxLayout();
    QHBoxLayout *checkbox = new QHBoxLayout();
    checkbox->addLayout(checkbox_l);
    checkbox->addLayout(checkbox_r);
    m_right->addLayout(checkbox);

    for(int i = 0; i < algorithms.size(); i++) {
        auto w = new QCheckBox(algorithms[i].name, this);
        m_checkboxes[algorithms[i].name] = w;

        if(i < algorithms.size()/2) {
            checkbox_l->addWidget(w);
        } else {
            checkbox_r->addWidget(w);
        }
    }

    m_left->addWidget(m_tabMain);

    m_right->addSpacerItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Expanding));
    m_right->addWidget(m_progressBar);
    m_right->addWidget(m_testButton, 0, Qt::AlignTop);
    m_right->addWidget(m_visBtn, 0, Qt::AlignTop);
    m_right->addWidget(m_settingsBtn, 0, Qt::AlignTop);

    m_main->addLayout(m_left, 1);
    m_main->addLayout(m_right, 0);

    m_tabMain->addTab(m_tabGraphs, tr("Graph"));
    m_tabMain->addTab(m_tabTables, tr("Table"));

    connect(m_testButton, &QPushButton::clicked, this, &MainWindow::test);
    connect(m_testThread, &TestThread::started, this, [&]{ setControlsEnabled(false); });
    connect(m_testThread, &TestThread::finished, this, &MainWindow::testEnd);
    connect(m_testThread, &TestThread::progressValChanged, m_progressBar, &QProgressBar::setValue);
    connect(m_testThread, &TestThread::progressTextChanged, m_progressBar, &QProgressBar::setFormat);
    connect(m_testThread, &TestThread::progressMaxValChanged, m_progressBar, &QProgressBar::setMaximum);

    connect(m_min, &QLineEdit::textChanged, this, &MainWindow::validateInt);
    connect(m_max, &QLineEdit::textChanged, this, &MainWindow::validateInt);

    connect(m_visBtn, &QPushButton::clicked, [&]{
                m_vis->show();
            });

    connect(m_settingsBtn, &QPushButton::clicked, [&]{
                ChangeLangDialog d(this);
                d.exec();
                if(d.result() == QDialog::Accepted) {
                    this->close();
                    qApp->exit(EXIT_CODE_REBOOT);
                }
            });
}

MainWindow::~MainWindow()
{
    delete m_vis;
    m_testThread->terminate();
    delete m_testThread;
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
    if(m_SwapsPlot) {
        m_SwapsPlot->deleteLater();
    }
    if(m_TimesPlot) {
        m_TimesPlot->deleteLater();
    }
    if(m_timeColDelegate == nullptr) {
        m_timeColDelegate = new TimeColDelegate(this);
    }

    for(auto table : m_tables) {
        delete table;
    }

    m_SwapsPlot = new QCustomPlot(this);
    m_SwapsPlot->yAxis->setLabel(tr("Swaps count"));
    m_SwapsPlot->xAxis->setLabel(tr("Digits in Vector"));
    m_SwapsPlot->legend->setFillOrder(QCPLegend::foColumnsFirst);
    m_SwapsPlot->plotLayout()->insertRow(0);
    m_SwapsPlot->plotLayout()->addElement(0, 0, new QCPTextElement(m_SwapsPlot, tr("Swaps count"), QFont("sans", 12, QFont::Bold)));
    m_SwapsPlot->setInteraction(QCP::iRangeZoom, true);
    m_SwapsPlot->setInteraction(QCP::iRangeDrag, true);

    m_TimesPlot = new QCustomPlot(this);
    m_TimesPlot->setMinimumHeight(400);
    m_TimesPlot->setMinimumWidth(400);
    m_TimesPlot->yAxis->setLabel(tr("Time (nanoseconds)"));
    m_TimesPlot->xAxis->setLabel(tr("Digits in Vector"));
    m_TimesPlot->legend->setFillOrder(QCPLegend::foColumnsFirst);
    m_TimesPlot->plotLayout()->insertRow(0);
    m_TimesPlot->plotLayout()->addElement(0, 0, new QCPTextElement(m_TimesPlot, tr("Wasted time"), QFont("sans", 12, QFont::Bold)));
    m_TimesPlot->setInteraction(QCP::iRangeZoom, true);
    m_TimesPlot->setInteraction(QCP::iRangeDrag, true);

    int colorIt = 0;
    for(const auto &vec : results.data) {
        m_SwapsPlot->addGraph();
        auto i = m_SwapsPlot->graphCount()-1;
        m_SwapsPlot->graph(i)->setPen(QPen(colors[colorIt], 2));
        m_SwapsPlot->graph(i)->setData(results.keys, vec.swaps, true);
        m_SwapsPlot->graph(i)->setName(vec.name);

        m_TimesPlot->addGraph();
        i = m_TimesPlot->graphCount()-1;
        m_TimesPlot->graph(i)->setPen(QPen(colors[colorIt], 2));
        m_TimesPlot->graph(i)->setData(results.keys, vec.time, true);
        m_TimesPlot->graph(i)->setName(vec.name);

        ++colorIt;
    }

    QCPLayoutGrid *subSwapsLayout = new QCPLayoutGrid;
    m_SwapsPlot->plotLayout()->insertRow(2);
    m_SwapsPlot->plotLayout()->addElement(2, 0, subSwapsLayout);
    subSwapsLayout->setMargins(QMargins(5, 0, 5, 5));

    QCPLayoutGrid *subTimesLayout = new QCPLayoutGrid;
    m_TimesPlot->plotLayout()->insertRow(2);
    m_TimesPlot->plotLayout()->addElement(2, 0, subTimesLayout);
    subTimesLayout->setMargins(QMargins(5, 0, 5, 5));

    for (size_t i = 0; i < m_SwapsPlot->legend->itemCount(); i += 4) {
        for (size_t j = i; j < i + 4 && j < m_SwapsPlot->legend->itemCount(); ++j) {
            subSwapsLayout->addElement(i / 4, j - i, m_SwapsPlot->legend->item(j));
            subTimesLayout->addElement(i / 4, j - i, m_TimesPlot->legend->item(j));
        }
    }

    m_SwapsPlot->plotLayout()->setRowStretchFactor(2, 0.001);
    m_SwapsPlot->rescaleAxes();

    m_TimesPlot->plotLayout()->setRowStretchFactor(2, 0.001);
    m_TimesPlot->rescaleAxes();

    const size_t rows = results.keys.size();
    for(const auto result : results.data) {
        auto table = new QTableView(this);
        auto model = new Model(table);

        model->setData({.keys = results.keys, .swaps = result.swaps, .time = result.time});
        table->setModel(model);
        table->setItemDelegateForColumn(model->columnCount()-1, m_timeColDelegate);
        table->verticalHeader()->setVisible(false);
        table->horizontalHeader()->setVisible(true);
        table->setSortingEnabled(true);
        table->resizeColumnsToContents();
        table->horizontalHeader()->setStretchLastSection(true);

        m_tables.push_back(table);
        m_tabTables->addTab(table, result.name);
    }

    m_tabGraphs->addTab(m_SwapsPlot, tr("Swaps"));
    m_tabGraphs->addTab(m_TimesPlot, tr("Time"));

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

    for(auto checkbox : m_checkboxes) {
        checkbox->setEnabled(val);
    }
}
