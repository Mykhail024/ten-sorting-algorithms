#include "qcustomplot.h"

#include "GraphView.h"

GraphView::GraphView(QWidget *parent)
    : QCustomPlot()
    , m_legend_enabled_color(QStringLiteral("#00000"))
    , m_legend_disabled_color(QStringLiteral("#707070"))
    , m_legendLayout(new QCPLayoutGrid)
    , m_label_text(new QCPTextElement(this, "GraphView", QFont("sans", 12, QFont::Bold)))
{
    this->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
    this->legend->setFillOrder(QCPLegend::foColumnsFirst);

    m_legendLayout->setMargins(QMargins(5, 0, 5, 5));

    this->plotLayout()->insertRow(0);
    this->plotLayout()->addElement(0, 0, m_label_text);
    this->plotLayout()->insertRow(2);
    this->plotLayout()->addElement(2, 0, m_legendLayout);
    this->plotLayout()->setRowStretchFactor(2, 0.001);

    connect(this, &QCustomPlot::legendClick, this, &GraphView::onGraphLegendClicked);
}

GraphView::~GraphView()
{
    delete m_legendLayout;
}

void GraphView::onGraphLegendClicked(QCPLegend *legend, QCPAbstractLegendItem *legendItem, QMouseEvent *event)
{
    if (auto plItem = qobject_cast<QCPPlottableLegendItem*>(legendItem))
    {
        bool newState = !plItem->plottable()->visible();
        plItem->plottable()->setVisible(newState);
        legendItem->setTextColor(newState ? m_legend_enabled_color : m_legend_disabled_color);
        if(auto plot = qobject_cast<QCustomPlot*>(sender())) {
            plot->replot();
        }
    }
}

QColor GraphView::legendEnabledColor() const
{
    return m_legend_enabled_color;
}

QColor GraphView::legendDisableColor() const
{
    return m_legend_disabled_color;
}

void GraphView::setLegendEnabledColor(const QColor &color)
{
    m_legend_enabled_color = color;
}

void GraphView::setLegendDisabledColor(const QColor &color)
{
    m_legend_disabled_color = color;
}

void GraphView::setLabelText(const QString &text, const QFont font)
{
    m_label_text->setText(text);
    m_label_text->setFont(font);
}

void GraphView::applyLegend()
{
    m_legendLayout->clear();
    for (size_t i = 0; i < this->legend->itemCount(); i += 4) {
        for (size_t j = i; j < i + 4 && j < this->legend->itemCount(); ++j) {
            m_legendLayout->addElement(i / 4, j - i, this->legend->item(j));
        }
    }
}

void GraphView::addData(const QVector<double> &keys, const QVector<double> &values, const QString &name, const QColor &color)
{
    auto graph = this->addGraph();
    graph->setPen(QPen(color, 2));
    graph->setData(keys, values);
    graph->setName(name);

    this->rescaleAxes();
}
