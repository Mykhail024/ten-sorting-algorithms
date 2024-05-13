#pragma once

#include "qcustomplot.h"

class QCPTextElement;

class GraphView : public QCustomPlot
{
    Q_OBJECT
    public:
        GraphView(QWidget *parent = nullptr);
        ~GraphView();
        QColor legendEnabledColor() const;
        QColor legendDisableColor() const;
        void setLegendEnabledColor(const QColor &color);
        void setLegendDisabledColor(const QColor &color);
        void setLabelText(const QString &text, const QFont font = QFont("sans", 12, QFont::Bold));
        void addData(const QVector<double> &keys, const QVector<double> &values, const QString &name, const QColor &color = QColor("#000000"));
        void applyLegend();

    public slots:
        void onGraphLegendClicked(QCPLegend *legend, QCPAbstractLegendItem *legendItem, QMouseEvent *event);

    private:
        QColor m_legend_enabled_color;
        QColor m_legend_disabled_color;
        
        QCPTextElement *m_label_text;
        QCPLayoutGrid *m_legendLayout;
};
