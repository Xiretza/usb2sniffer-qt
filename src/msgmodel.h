#ifndef MSGMODEL_H
#define MSGMODEL_H

#include <QAbstractItemModel>

#include "msgitem.h"

class MSGModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    explicit MSGModel(QObject *parent = nullptr);
    ~MSGModel();

    QVariant data(const QModelIndex &index, int role) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const override;
    QModelIndex index(int row, int column,
                      const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &index) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    /* custom API */
    int addMessage(uint64_t ts, uint8_t type, uint8_t val);
    int addMessageVector(std::vector<std::tuple<uint64_t, uint8_t, uint8_t>>& vec);

private:
    MSGItem *m_rootItem;
};

#endif // MSGMODEL_H
