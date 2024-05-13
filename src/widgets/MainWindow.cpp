#include <QApplication>
#include <QValidator>
#include <QThread>
#include <QRandomGenerator>
#include <QCheckBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSpacerItem>
#include <QTableView>
#include <QHeaderView>
#include <QGridLayout>
#include <unistd.h>

#include "AbstractTestModel.h"
#include "AlgorithmHandler.h"
#include "Log.h"
#include "SourceModelHandler.h"
#include "AbstractSourceModel.h"
#include "ChangeLangDialog.h"
#include "GraphView.h"
#include "TableModel.h"
#include "TableView.h"

#include "StandardTestModel.h"
#include "sort.h"

#include "MainWindow.h"

const QVector<QColor> colors = {"black", "red", "blue", "#30f930", "purple", "orange", "brown", "gray", "green", "#7f1212"};

MainWindow::MainWindow()
    : QWidget()
    , m_main(new QHBoxLayout(this))
    , m_right(new QVBoxLayout())
    , m_left(new QVBoxLayout())
    , m_tabMain(new QTabWidget(this))
    , m_tabGraphs(new QTabWidget(this))
    , m_tabTables(new QTabWidget(this))
    , m_testButton(new QPushButton(tr("Test"), this))
    , m_progressBar(new QProgressBar(this))
    , m_iterationsCount(new QLineEdit(this))
    , m_settingsBtn(new QPushButton(tr("Change Language"), this))
    , m_dataSource(new QComboBox(this))
    , m_sourceContainer(new QVBoxLayout())
    , m_groupBoxLayout(new QGridLayout())
    , m_btnGroup(new QButtonGroup(this))
    , m_groupBox(new QGroupBox(this))
    , m_scroll(new QScrollArea(this))
    , m_test_model(new StandardTestModel(this))
{
    this->setWindowTitle(tr("Sorting Benchmark"));

    m_sourceContainer->setContentsMargins(0, 0, 0, 0);
    m_sourceContainer->setSpacing(0);

    m_progressBar->setTextVisible(true);
    m_progressBar->setFormat(tr("Press test"));
    m_progressBar->setAlignment(Qt::AlignCenter);
    m_progressBar->setMinimum(0);
    m_progressBar->setMaximum(1);
    m_progressBar->setValue(1);

    m_right->addWidget(new QLabel(tr("Data source"), this));
    m_right->addWidget(m_dataSource);
    m_right->addLayout(m_sourceContainer);
    m_right->addWidget(new QLabel(tr("Iteration count")));
    m_right->addWidget(m_iterationsCount);

    m_scroll->setWidgetResizable(true);

    m_btnGroup->setExclusive(false);

    m_groupBox->setLayout(m_groupBoxLayout);
    m_scroll->setWidget(m_groupBox);
    m_right->addWidget(m_scroll);

    m_left->addWidget(m_tabMain);

    m_right->addSpacerItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Expanding));
    m_right->addWidget(m_progressBar);
    m_right->addWidget(m_testButton, 0, Qt::AlignTop);
    m_right->addWidget(m_settingsBtn, 0, Qt::AlignTop);

    m_main->addLayout(m_left, 1);
    m_main->addLayout(m_right, 0);

    m_tabMain->addTab(m_tabGraphs, tr("Graph"));
    m_tabMain->addTab(m_tabTables, tr("Table"));

    m_progressBar->setMinimumWidth(200);

    connect(m_testButton, &QPushButton::clicked, this, &MainWindow::startTest);

    connect(m_settingsBtn, &QPushButton::clicked, [&]{
                ChangeLangDialog d(this);
                d.exec();
                if(d.result() == QDialog::Accepted) {
                    this->close();
                    qApp->exit(EXIT_CODE_REBOOT);
                }
            });

    connect(m_dataSource, &QComboBox::currentIndexChanged, this, &MainWindow::sourceChanged);

    connect(&algorithmHandler(), &AlgorithmHandler::onNewAlgorithmRegister, this, [&](const AlgorithmHandler::Algorithm &alg) {
                auto checkbox = new QCheckBox(alg.name, this);
                m_groupBoxLayout->addWidget(checkbox);
                m_btnGroup->addButton(checkbox);
            });

    connect(&algorithmHandler(), &AlgorithmHandler::onNewAlgorithmUnregister, this, [&](const AlgorithmHandler::Algorithm &alg) {
                auto btn = m_groupBox->findChild<QCheckBox*>(alg.name);
                if(btn) {
                    btn->deleteLater();

                }
            });
    connect(m_btnGroup, &QButtonGroup::buttonToggled, this, &MainWindow::checkboxChecked);
    connect(m_test_model, &StandardTestModel::progressValueUpdate, m_progressBar, &QProgressBar::setValue);
    connect(m_test_model, &StandardTestModel::progressMaxValueUpdate, m_progressBar, &QProgressBar::setMaximum);
    connect(m_test_model, &StandardTestModel::progressTextUpdate, m_progressBar, &QProgressBar::setFormat);
    connect(m_test_model, &StandardTestModel::onTestFinished, this, &MainWindow::testFinished);

    loadSources();
    createCheckBoxes();
}

MainWindow::~MainWindow()
{
}

void MainWindow::sourceChanged(const int &source)
{
    while (QLayoutItem* item = m_sourceContainer->takeAt(0)) {
        if (QWidget* widget = item->widget()) {
            delete widget;
        }
        delete item;
    }
    if(source >= sourceModelHandler().count()) {
        Log_Warning("No source index: " + QString::number(source));
        return;
    }

    m_sourceContainer->addWidget(sourceModelHandler().model(source)->widget());
}

void MainWindow::loadSources()
{
    const auto count = sourceModelHandler().count();
    QStringList sources;
    for(size_t i = 0; i < count; ++i) {
         sources.push_back(sourceModelHandler().model(i)->name());
    }
    if(count > 0) {
        m_dataSource->addItems(sources);
        m_dataSource->setCurrentIndex(0);
        sourceChanged(0);
    }
}

void MainWindow::checkboxChecked(QAbstractButton *button, bool checked)
{
    algorithmHandler().setEnabled(button->text().replace("&", ""), checked);
}

void MainWindow::createCheckBoxes()
{
    while (QLayoutItem* item = m_groupBoxLayout->takeAt(0)) {
        if (QWidget* widget = item->widget()) {
            delete widget;
        }
        delete item;
    }
    const auto count = algorithmHandler().algorithmsCount();
    for(size_t i = 0; i < count; ++i) {
        auto checkbox = new QCheckBox(algorithmHandler().algorithm(i).name, this);
        m_groupBoxLayout->addWidget(checkbox);
        m_btnGroup->addButton(checkbox);
    }
}

void MainWindow::testFinished(const std::vector<AbstractTestModel::_result> &results)
{
    Log_Debug("Test finished");
    if(m_SwapsPlot != nullptr) {
        m_SwapsPlot->deleteLater();
    }
    if(m_TimesPlot != nullptr) {
        m_TimesPlot->deleteLater();
    }
    auto tables = m_tabTables->findChildren<TableView*>();
    for(auto table : tables) {
        table->deleteLater();
    }

    m_SwapsPlot = new GraphView(this);
    m_TimesPlot = new GraphView(this);

    m_TimesPlot->yAxis->setLabel(tr("Time (nanoseconds)"));
    m_TimesPlot->xAxis->setLabel(tr("Numbers in array"));
    m_SwapsPlot->yAxis->setLabel(tr("Swaps count"));
    m_SwapsPlot->xAxis->setLabel(tr("Numbers in array"));

    m_SwapsPlot->setLabelText(tr("Swaps count"));
    m_TimesPlot->setLabelText(tr("Wasted time"));

    const auto keys = sourceModelHandler().model(m_dataSource->currentIndex())->keys();
    int colorIt = 0;
    for(const auto &r : results) {
        m_SwapsPlot->addData(keys, r.swaps, r.algorithm.name, colors[colorIt]);
        m_TimesPlot->addData(keys, r.times, r.algorithm.name, colors[colorIt]);
        ++colorIt;
    }
    m_SwapsPlot->applyLegend();
    m_TimesPlot->applyLegend();

    m_tabGraphs->addTab(m_SwapsPlot, tr("Swaps"));
    m_tabGraphs->addTab(m_TimesPlot, tr("Time"));

    const size_t rows = results.size();
    for(const auto &r : results) {
        auto table = new TableView(this);
        auto model = new TableModel(table);

        model->setData({.keys = keys, .swaps = r.swaps, .time = r.times});
        table->setModel(model);

        m_tables.push_back(table);
        m_tabTables->addTab(table, r.algorithm.name);
    }
    setControlsEnabled(true);
}

void MainWindow::startTest()
{
    if(algorithmHandler().enabledCount() == 0) return;

    auto source_model = sourceModelHandler().model(m_dataSource->currentIndex());
    if(source_model == nullptr) return;

    bool ok;
    const auto it_count = m_iterationsCount->text().toInt(&ok);
    if(!ok) {
        Log_Warning("Failed to parse iteration count");
        return;
    }

    m_test_model->setSourceModel(source_model);
    m_test_model->setIterationCount(it_count);

    if(!source_model->commit()) return;
    m_test_model->startTest();
    setControlsEnabled(false);
}

void MainWindow::setControlsEnabled(const bool &val)
{
    m_testButton->setEnabled(val);
    m_iterationsCount->setEnabled(val);
    m_dataSource->setEnabled(val);
    sourceModelHandler().model(m_dataSource->currentIndex())->widget()->setEnabled(val);
}

