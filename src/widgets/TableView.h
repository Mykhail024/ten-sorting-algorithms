#pragma once

#include <QTableView>
#include <QItemDelegate>

class TimeColDelegate : public QItemDelegate 
{
    Q_OBJECT
    public:
        explicit TimeColDelegate(QObject *parent = nullptr);
        void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;

};

class TableView : public QTableView
{
    public:
        TableView(QWidget *parent = nullptr);
        void setModel(QAbstractItemModel *model);

    private:
        using QTableView::setModel;
};
