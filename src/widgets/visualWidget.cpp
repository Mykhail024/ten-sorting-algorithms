#include <QPushButton>
#include <QIntValidator>
#include <QLabel>

#include "algorithmThreads/bubleAlgorithmThread.h"
#include "algorithmThreads/cycleAlgorithmThread.h"
#include "visual.h"
#include "sort.h"

#include "visualWidget.h"

VisualWidget::VisualWidget(QWidget *parent)
    : QWidget(parent)
    , m_visual(new Visual(this))
    , m_layout(new QVBoxLayout(this))
    , m_delay(new QLineEdit(this))
    , m_count(new QLineEdit(this))
    , m_start(new QPushButton(tr("Start"), this))
    , m_algorithm(new QComboBox(this))
{
    this->setWindowTitle(tr("Visual"));
    m_delay->setValidator(new QIntValidator(0, INT_MAX, this));
    m_count->setValidator(new QIntValidator(0, 60, this));

    m_algorithm->addItems({"Buble", "Cycle"});

    m_layout->addWidget(m_visual);
    m_layout->addWidget(new QLabel(tr("Algorithm:"), this));
    m_layout->addWidget(m_algorithm);
    m_layout->addWidget(new QLabel(tr("Count:"), this));
    m_layout->addWidget(m_count);
    m_layout->addWidget(new QLabel(tr("Delay:"), this));
    m_layout->addWidget(m_delay);
    m_layout->addWidget(m_start);

    m_visual->setMinimumSize(800, 400);
    m_visual->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    connect(m_start, &QPushButton::clicked, this, [&]{
                if(m_count->text().isEmpty() || m_delay->text().isEmpty()) return;
                    m_visual->setVector(generateVec(m_count->text().toInt(), 0, 10000));
                    m_visual->setDelay(m_delay->text().toInt());
                    m_visual->setThread(algorithmFromString(m_algorithm->currentText()));
                if(m_visual->start()) {
                    enableControls(false);
                }
            });
    connect(m_visual, &Visual::onFinish, this, [&]{
                enableControls(true);
            });
}

AlgorithmThread* VisualWidget::algorithmFromString(const QString &name)
{
    if(name == "Buble") {
        return new BubleAlgorithmThread(m_visual);
    } else if (name == "Cycle") {
        return new CycleAlgorithmThread(m_visual);
    } else {
        qDebug() << "No algorithm " + name;
        return nullptr;
    }
}

void VisualWidget::enableControls(const bool &enable)
{
    m_start->setEnabled(enable);
    m_algorithm->setEnabled(enable);
    m_delay->setEnabled(enable);
    m_count->setEnabled(enable);
}
