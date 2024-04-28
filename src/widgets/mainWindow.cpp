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
#include <QHeaderView>

#include <climits>
#include <random>
#include <ctime>

#include "Log.h"
#include "qcustomplot.h"
#include "model.h"
#include "sort.h"
#include "testThread.h"
#include "changeLangDialog.h"

#include "mainWindow.h"

const QVector<QColor> colors = {"black", "red", "blue", "#30f930", "purple", "orange", "brown", "gray", "green", "#7f1212"};

enum SOURCE {
    RANDOM_GENERATED,
    FROM_TEXT_FILE
};

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
    , m_startIt(std::make_unique<InputWitchLabel>(tr("Numbers on first array"), this))
    , m_endIt(std::make_unique<InputWitchLabel>(tr("Numbers on last array"), this))
    , m_min(std::make_unique<InputWitchLabel>(tr("Min number in array"), this))
    , m_max(std::make_unique<InputWitchLabel>(tr("Max number in array"), this))
    , m_stepIt(std::make_unique<InputWitchLabel>(tr("Step"), this))
    , m_iterationsCount(std::make_unique<InputWitchLabel>(tr("Iteration Count"), this))
    , m_progressBar(new QProgressBar(this))
    , m_testThread(new TestThread(this))
    , m_settingsBtn(new QPushButton(tr("Change Language"), this))
    , m_dataSource(new QComboBox(this))
{
    this->setWindowTitle(tr("Sorting Benchmark"));

    auto *validator = new QIntValidator(0, INT_MAX, this);

    m_iterationsCount->setValidator(validator);
    m_startIt->setValidator(validator);
    m_endIt->setValidator(validator);
    m_stepIt->setValidator(validator);
    m_min->setValidator(validator);
    m_max->setValidator(validator);

    m_dataSource->addItems({"Random generate", "Text file"});

    m_progressBar->setTextVisible(true);
    m_progressBar->setFormat(tr("Press test"));
    m_progressBar->setAlignment(Qt::AlignCenter);
    m_progressBar->setMinimum(0);
    m_progressBar->setMaximum(1);
    m_progressBar->setValue(1);

    m_right->addWidget(new QLabel(tr("Data source"), this));
    m_right->addWidget(m_dataSource);
    m_startIt->addToLayout(m_right);
    m_endIt->addToLayout(m_right);
    m_min->addToLayout(m_right);
    m_max->addToLayout(m_right);
    m_stepIt->addToLayout(m_right);

    m_iterationsCount->addToLayout(m_right);

    QVBoxLayout *checkbox_l = new QVBoxLayout();
    QVBoxLayout *checkbox_r = new QVBoxLayout();
    QHBoxLayout *checkbox = new QHBoxLayout();
    checkbox->addLayout(checkbox_l);
    checkbox->addLayout(checkbox_r);
    m_right->addLayout(checkbox);

    for(int i = 1; i < ALGORITHMS_COUNT; i++) {
        const QString name = TestThread::algorithmName(static_cast<ALGORITHMS>(i));
        auto w = new QCheckBox(name, this);
        w->setProperty("index", i);
        connect(w, &QCheckBox::stateChanged, this, &MainWindow::onAlgorithmEnable);
        m_checkboxes[name] = w;

        if(i < ALGORITHMS_COUNT/2) {
            checkbox_l->addWidget(w);
        } else {
            checkbox_r->addWidget(w);
        }
    }

    m_left->addWidget(m_tabMain);

    m_right->addSpacerItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Expanding));
    m_right->addWidget(m_progressBar);
    m_right->addWidget(m_testButton, 0, Qt::AlignTop);
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

    connect(m_min->lineEdit, &QLineEdit::textChanged, this, &MainWindow::validateInt);
    connect(m_max->lineEdit, &QLineEdit::textChanged, this, &MainWindow::validateInt);

    connect(m_settingsBtn, &QPushButton::clicked, [&]{
                ChangeLangDialog d(this);
                d.exec();
                if(d.result() == QDialog::Accepted) {
                    this->close();
                    qApp->exit(EXIT_CODE_REBOOT);
                }
            });
    connect(m_dataSource, &QComboBox::currentIndexChanged, this, &MainWindow::onSourceChanged);
}

MainWindow::~MainWindow()
{
    m_testThread->terminate();
    delete m_testThread;
}

void MainWindow::onSourceChanged(const int &source)
{
    if(source == RANDOM_GENERATED) {
        setTextFileSourceControlsVisible(false);
        setRandomSourceControlsVisible(true);

    } else if (source == FROM_TEXT_FILE) {
        setRandomSourceControlsVisible(false);
        setTextFileSourceControlsVisible(true);
    }
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
    const int sourceIndex = m_dataSource->currentIndex();
    if(sourceIndex == RANDOM_GENERATED) {
        if(m_iterationsCount->lineEdit->text().isEmpty()
                || m_startIt->lineEdit->text().isEmpty()
                || m_endIt->lineEdit->text().isEmpty()
                || m_stepIt->lineEdit->text().isEmpty()
                || m_min->lineEdit->text().isEmpty()
                || m_max->lineEdit->text().isEmpty()
          ) return;

        const int itCount = m_iterationsCount->lineEdit->text().toInt();
        const int start = m_startIt->lineEdit->text().toInt();
        const int end = m_endIt->lineEdit->text().toInt();
        const int step = m_stepIt->lineEdit->text().toInt();

        const int numVectors = ((end - start) / step) + 1;
        std::default_random_engine generator(time(0));

        std::vector<std::vector<int>> vec(numVectors);

        for (int i = 0; i < numVectors; ++i) {
            int currentSize = start + i * step;
            vec[i].resize(currentSize);
            std::uniform_int_distribution<int> distribution(0, currentSize - 1);
            for (int j = 0; j < currentSize; ++j) {
                vec[i][j] = distribution(generator);
            }
        }

        m_testThread->setData(vec);
        m_testThread->setIterationsCount(itCount);
        m_testThread->start(QThread::HighestPriority);
    } else if (sourceIndex == FROM_TEXT_FILE) {
        m_testThread->start(QThread::HighestPriority);
    }
}


void MainWindow::testEnd(std::vector<testResult> results)
{
    Log_Debug("Test Finished");
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

    QVector<double> keys;
    for(size_t i = 0; i < results[results.size()-1].swaps.size(); ++i){
        keys.push_back(i);
    }

    int colorIt = 0;
    for(const auto &r : results) {
        m_SwapsPlot->addGraph();
        auto i = m_SwapsPlot->graphCount()-1;
        m_SwapsPlot->graph(i)->setPen(QPen(colors[colorIt], 2));
        m_SwapsPlot->graph(i)->setData(keys, r.swaps);
        m_SwapsPlot->graph(i)->setName(TestThread::algorithmName(r.algorithm));

        m_TimesPlot->addGraph();
        i = m_TimesPlot->graphCount()-1;
        m_TimesPlot->graph(i)->setPen(QPen(colors[colorIt], 2));
        m_TimesPlot->graph(i)->setData(keys, r.times);
        m_TimesPlot->graph(i)->setName(TestThread::algorithmName(r.algorithm));

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

    connect(m_SwapsPlot, &QCustomPlot::legendClick, this, &MainWindow::onGraphLegendClicked);
    connect(m_TimesPlot, &QCustomPlot::legendClick, this, &MainWindow::onGraphLegendClicked);

    const size_t rows = results.size();
    for(const auto r : results) {
        auto table = new QTableView(this);
        auto model = new Model(table);

        model->setData({.keys = keys, .swaps = r.swaps, .time = r.times});
        table->setModel(model);
        table->setItemDelegateForColumn(model->columnCount()-1, m_timeColDelegate);
        table->verticalHeader()->setVisible(false);
        table->horizontalHeader()->setVisible(true);
        table->setSortingEnabled(true);
        table->horizontalHeader()->setStretchLastSection(true);
        table->resizeColumnsToContents();

        m_tables.push_back(table);
        m_tabTables->addTab(table, TestThread::algorithmName(r.algorithm));
    }

    m_tabGraphs->addTab(m_SwapsPlot, tr("Swaps"));
    m_tabGraphs->addTab(m_TimesPlot, tr("Time"));

    setControlsEnabled(true);
}

const QColor enabled(QStringLiteral("#000000"));
const QColor disabled(QStringLiteral("#707070"));
void MainWindow::onGraphLegendClicked(QCPLegend *legend, QCPAbstractLegendItem *legendItem, QMouseEvent *event)
{
    if (auto plItem = qobject_cast<QCPPlottableLegendItem*>(legendItem))
    {
        bool newState = !plItem->plottable()->visible();
        plItem->plottable()->setVisible(newState);
        legendItem->setTextColor(newState ? enabled : disabled);
        if(auto plot = qobject_cast<QCustomPlot*>(sender())) {
            plot->replot();
        }
    }
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
    m_dataSource->setEnabled(val);

    for(auto checkbox : m_checkboxes) {
        checkbox->setEnabled(val);
    }
}

void MainWindow::setRandomSourceControlsVisible(const bool &val)
{
    m_startIt->setVisible(val);
    m_endIt->setVisible(val);
    m_stepIt->setVisible(val);
    m_min->setVisible(val);
    m_max->setVisible(val);
}

void MainWindow::setTextFileSourceControlsVisible(const bool &val)
{

}

void MainWindow::onAlgorithmEnable(const int &state)
{
    const int index = sender()->property("index").toInt();
    m_testThread->enableAlgorithm(static_cast<ALGORITHMS>(index), state);
}
