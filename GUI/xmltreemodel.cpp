#include "xmltreemodel.h"

namespace XML {

TreeModel::TreeModel(DOM::Document *data, QObject *parent)
    : document(data), QAbstractItemModel(parent)
{
}

DOM::Node *TreeModel::getItem(const QModelIndex &index) const
{
    if (index.isValid()) {
        auto *item = static_cast<DOM::Node*>(index.internalPointer());
        if (item)
            return item;
    }

    return document.get();
}

QVariant TreeModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal and role == Qt::DisplayRole) {
        if (section == 0)
            return "Name";
        else if (section == 1)
            return "Type";
        else if (section == 2)
            return "Value";
    }

    return QVariant();
}

bool TreeModel::setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role)
{
    if (value != headerData(section, orientation, role)) {
        // FIXME: Implement me!
        emit headerDataChanged(orientation, section, section);
        return true;
    }
    return false;
}

QModelIndex TreeModel::index(int row, int column, const QModelIndex &parent) const
{
    if (parent.isValid() and parent.column() != 0)
            return QModelIndex();

    auto parentItem = getItem(parent);

}

QModelIndex TreeModel::parent(const QModelIndex &index) const
{
    // FIXME: Implement me!
}

int TreeModel::rowCount(const QModelIndex &parent) const
{
    auto *parentItem = getItem(parent);

    return parentItem->child_nodes.size();
}

int TreeModel::columnCount(const QModelIndex &parent) const
{
    return 3;
}

QVariant TreeModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    // FIXME: Implement me!
    return QVariant();
}

bool TreeModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (data(index, role) != value) {
        // FIXME: Implement me!
        emit dataChanged(index, index, QVector<int>() << role);
        return true;
    }
    return false;
}

Qt::ItemFlags TreeModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags;

    return Qt::ItemIsEditable | QAbstractItemModel::flags(index); // FIXME: Implement me!
}

bool TreeModel::insertRows(int row, int count, const QModelIndex &parent)
{
    beginInsertRows(parent, row, row + count - 1);
    // FIXME: Implement me!
    endInsertRows();
}

bool TreeModel::insertColumns(int column, int count, const QModelIndex &parent)
{
    beginInsertColumns(parent, column, column + count - 1);
    // FIXME: Implement me!
    endInsertColumns();
}

bool TreeModel::removeRows(int row, int count, const QModelIndex &parent)
{
    beginRemoveRows(parent, row, row + count - 1);
    // FIXME: Implement me!
    endRemoveRows();
}

bool TreeModel::removeColumns(int column, int count, const QModelIndex &parent)
{
    beginRemoveColumns(parent, column, column + count - 1);
    // FIXME: Implement me!
    endRemoveColumns();
}

} // namespace XML
