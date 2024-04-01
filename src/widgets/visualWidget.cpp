#include <QPushButton>
#include <QIntValidator>
#include <QLabel>

#include "visual.h"
#include "sort.h"

#include "visualWidget.h"

VisualWidget::VisualWidget(QWidget *parent)
    : QWidget(parent)
    , m_visual(new Visual(this))
    , m_layout(new QVBoxLayout(this))
    , m_delay(new QLineEdit(this))
    , m_count(new QLineEdit(this))
    , m_start(new QPushButton("Start", this))
{
    m_delay->setValidator(new QIntValidator(0, INT_MAX, this));
    m_count->setValidator(new QIntValidator(0, 60, this));

    m_layout->addWidget(m_visual);
    m_layout->addWidget(new QLabel("Count:", this));
    m_layout->addWidget(m_count);
    m_layout->addWidget(new QLabel("Delay:", this));
    m_layout->addWidget(m_delay);
    m_layout->addWidget(m_start);

    m_visual->setMinimumSize(800, 400);
    m_visual->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    connect(m_start, &QPushButton::clicked, this, [&]{
                if(m_count->text().isEmpty() || m_delay->text().isEmpty()) return;
                m_visual->setVector(generateVec(m_count->text().toInt(), 0, 10000));
                m_visual->setDelay(m_delay->text().toInt());
                m_visual->start();
                m_start->setEnabled(false);
            });
    connect(m_visual, &Visual::onFinish, this, [&]{
                m_start->setEnabled(true);
            });
}
