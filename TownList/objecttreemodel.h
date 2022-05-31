#pragma once

#include <QObject>
#include <QAbstractItemModel>

class ObjectTreeModel : public QAbstractItemModel {
    Q_OBJECT

protected:
    QStringList _columns;
    QObject* _root_item;

public: // don't forget!
    QObject* internalObject(const QModelIndex& index) const;

public:
    ObjectTreeModel(QObject* parent = nullptr);

    void setColumns(const QStringList& columns);

    QModelIndex   index(const int row, const int column, const QModelIndex& parent) const override;
    QModelIndex   parent(const QModelIndex& child) const override;
    int           rowCount(const QModelIndex& parent) const override;
    int           columnCount(const QModelIndex& parent) const override;
    QVariant      data(const QModelIndex& index, const int role) const override;
    bool          setData(const QModelIndex& index, const QVariant& value, const int role) override;
    Qt::ItemFlags flags(const QModelIndex& index) const override;

public slots:
    void add(QObject* item, const QModelIndex& parent);
    void remove(const QModelIndex& index);
};

