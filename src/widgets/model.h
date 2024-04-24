#pragma once

#include <QAbstractTableModel>

class Model : public QAbstractTableModel
{
    Q_OBJECT
    public:
        struct modelData {
            QVector<double> keys;
            QVector<double> swaps;
            QVector<double> time;
        };

        Model(QObject *parent = nullptr);

        bool removeRows(int position, int rows, const QModelIndex &index = QModelIndex()) override;

        int rowCount(const QModelIndex &parent = QModelIndex()) const override;
        int columnCount(const QModelIndex &parent = QModelIndex()) const override;

        QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

        void setData(const modelData data);

    protected:
        Qt::ItemFlags flags(const QModelIndex &index = QModelIndex()) const override;
        QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    private:
       modelData m_data;
};
