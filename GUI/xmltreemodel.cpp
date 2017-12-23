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
            return "Text Content";
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

    auto childItem = parentItem->child_at(row);

    if (childItem)
        return createIndex(row, column, childItem);

    return QModelIndex();
}

QModelIndex TreeModel::parent(const QModelIndex &index) const
{
    if (!index.isValid())
            return QModelIndex();

    auto childItem = getItem(index);
    auto parentItem = childItem->parent_node;

    if (parentItem == document.get())
        return QModelIndex();

    return createIndex(parentItem->child_num(), 0, parentItem);
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

    if (role != Qt::DisplayRole && role != Qt::EditRole)
        return QVariant();

    auto item = getItem(index);
    auto col = index.column();

    if (col == 0)
        return QVariant(QString::fromStdString(item->name));
    else if (col == 1)
        return QVariant(QString::fromStdString(item->type_name()));
    else if (col == 2) {
        return QVariant(QString::fromStdString(item->text_content()));
    }
    return QVariant();
}

bool TreeModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (data(index, role) != value) {
        if (index.column() < 0 or index.column() == 1 or index.column() > 2)
            return false;

        auto item = getItem(index);

        if (index.column() == 0)
            item->name = value.toString().toStdString();
        else if (index.column() == 2) {
            if (item->type == XML::DOM::Node::Type::ELEMENT_NODE) {
                beginRemoveRows(index, 0, item->child_nodes.size() - 1);
                item->child_nodes.clear();
                endRemoveRows();
                item->append_child(new XML::DOM::Text(value.toString().toStdString()));
            } else {
                item->value = value.toString().toStdString();
            }
        }

        emit dataChanged(index, index, QVector<int>() << role);
        return true;
    }
    return false;
}

Qt::ItemFlags TreeModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags;

    if (index.column() == 1)
        return QAbstractItemModel::flags(index);

    return Qt::ItemIsEditable | QAbstractItemModel::flags(index);
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

DOM::Document *TreeModel::getDocument()
{
    return document.get();
}

} // namespace XML
