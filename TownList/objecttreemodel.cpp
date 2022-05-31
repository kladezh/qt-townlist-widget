#include "objecttreemodel.h"

ObjectTreeModel::ObjectTreeModel(QObject* parent) : QAbstractItemModel(parent)
{
    _root_item = new QObject(this);
}

QObject* ObjectTreeModel::internalObject(const QModelIndex& index) const {
    if (!index.isValid()) {
        return _root_item;
    }

    return static_cast<QObject*>(index.internalPointer());
}

void ObjectTreeModel::setColumns(const QStringList& columns) {
    _columns = columns;
}

QModelIndex ObjectTreeModel::index(const int row, const int column,
    const QModelIndex& parent) const {
    if (!hasIndex(row, column, parent)) {
        return QModelIndex();
    }

    return createIndex(row, column, internalObject(parent)->children().at(row));
}

QModelIndex ObjectTreeModel::parent(const QModelIndex& child) const {
    if (!child.isValid()) {
        return QModelIndex();
    }

    QObject* parent_obj = internalObject(child)->parent();

    if (parent_obj == _root_item) {
        return QModelIndex();
    }
    int row = parent_obj->parent()->children().indexOf(parent_obj);
    return createIndex(row, 0, parent_obj);
}

int ObjectTreeModel::rowCount(const QModelIndex& parent) const {
    return internalObject(parent)->children().count();
}

int ObjectTreeModel::columnCount(const QModelIndex& parent) const {
    Q_UNUSED(parent)
        return _columns.count();
}

QVariant ObjectTreeModel::data(const QModelIndex& index, const int role) const {
    if (!index.isValid() || (role != Qt::DisplayRole && role != Qt::EditRole)) {
        return QVariant();
    }

    return internalObject(index)->property(_columns.at(index.column()).toUtf8());
}

bool ObjectTreeModel::setData(const QModelIndex& index, const QVariant& value, const int role) {
    if (!index.isValid() || role != Qt::EditRole) {
        return false;
    }

    internalObject(index)->setProperty(_columns.at(index.column()).toUtf8(), value);

    emit dataChanged(index, index);
    return true;
}

Qt::ItemFlags ObjectTreeModel::flags(const QModelIndex& index) const {
    if (!index.isValid()) {
        return Qt::NoItemFlags;
    }

    return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
}

void ObjectTreeModel::add(QObject* item, const QModelIndex& parent) {
    beginInsertRows(parent, rowCount(parent), rowCount(parent));
    item->setParent(internalObject(parent));
    endInsertRows();
}

void ObjectTreeModel::remove(const QModelIndex& index) {
    beginRemoveRows(index.parent(), index.row(), index.row());
    internalObject(index)->deleteLater();
    endRemoveRows();
}
