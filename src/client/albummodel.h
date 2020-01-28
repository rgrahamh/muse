#ifndef ALBUMMODEL_H
#define ALBUMMODEL_H

#include <QAbstractTableModel>
#include <QDebug>

#include "muse_client.h"


class AlbumModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    AlbumModel(QObject *parent = 0);

    void populateData(struct albuminfolst* albums);
    void addData(struct albuminfolst* albums);

    int rowCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
    int columnCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;

    void clearModel();

private:
    QList<qint64> ids;
    QList<QString> titles;
    QList<QString> years;

};

#endif // ALBUMMODEL_H
