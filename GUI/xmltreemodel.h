#ifndef XMLTREEMODEL_H
#define XMLTREEMODEL_H

#include <QAbstractItemModel>
#include <QDebug>

#include <memory>

#include "DOM.hpp"

namespace XML {

class TreeModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    explicit TreeModel(DOM::Document &data, QObject *parent = nullptr);

    void setDocument(DOM::Document &data);

    DOM::Node* getItem(const QModelIndex &index) const;

    // Header:
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    bool setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role = Qt::EditRole) override;

    // Basic functionality:
    QModelIndex index(int row, int column,
                      const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &index) const override;

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    // Editable:
    bool setData(const QModelIndex &index, const QVariant &value,
                 int role = Qt::EditRole) override;

    Qt::ItemFlags flags(const QModelIndex& index) const override;

    // Add data:
    bool appendChild(const QModelIndex &parent, DOM::Node *node);

    // Remove data:
    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;

    DOM::Document* getDocument();
signals:
    void errorOccurred(const QString &title, const QString &info);
private:

    std::unique_ptr<DOM::Document> document;
};

} // namespace XML

#endif // XMLTREEMODEL_H
