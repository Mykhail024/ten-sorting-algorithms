#include <QPainter>

#include "TableView.h"

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

TableView::TableView(QWidget *parent) : QTableView(parent)
{
    this->verticalHeader()->setVisible(false);
    this->horizontalHeader()->setVisible(true);
    this->horizontalHeader()->setStretchLastSection(true);
    this->setSortingEnabled(true);
}

void TableView::setModel(QAbstractItemModel *model)
{
    QTableView::setModel(model);
    this->setItemDelegateForColumn(model->columnCount()-1, new TimeColDelegate(model));
    this->resizeColumnsToContents();
}
