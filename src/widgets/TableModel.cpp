#include "TableModel.h"

TableModel::TableModel(QObject *parent)
    : QAbstractTableModel(parent)
{

}

bool TableModel::removeRows(int position, int rows, const QModelIndex &index)
{
    if(position < 0 || position > m_data.keys.size() || position+rows > m_data.keys.size()) return false;

    beginRemoveRows(index, position, position+rows);
    m_data.keys.erase(m_data.keys.begin()+position, m_data.keys.begin()+position+rows);
    endRemoveRows();

    return true;
}

int TableModel::rowCount(const QModelIndex &parent) const
{
    return parent.isValid() ? 0 : m_data.keys.size();
}

int TableModel::columnCount(const QModelIndex &parent) const
{
    return parent.isValid() ? 0 : 3;
}

QVariant TableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(orientation != Qt::Horizontal) return QVariant();
    if(role == Qt::DisplayRole) {
        if(section == 0) return tr("Numbers count");
        else if(section == 1) return tr("Swaps");
        else if(section == 2) return tr("Time");
    }
    return QVariant();
}

Qt::ItemFlags TableModel::flags(const QModelIndex &index) const
{
    if (!index.isValid()) {
        return Qt::ItemIsEnabled | Qt::ItemIsEditable;
    }

    return QAbstractTableModel::flags(index);
}

QVariant TableModel::data(const QModelIndex &index, int role) const
{
    if(role == Qt::DisplayRole) {
        const auto column = index.column();
        const auto row = index.row();
        switch (column) {
            case 0:
                return m_data.keys[row];
            case 1:
                return m_data.swaps[row];
            case 2:
                return m_data.time[row];
            default:
                return QVariant();
        }
    }
    return QVariant();
}

void TableModel::setData(const modelData data)
{
    beginResetModel();
    m_data = data;
    endResetModel();
}
