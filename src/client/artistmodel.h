#ifndef ARTISTMODEL_H
#define ARTISTMODEL_H

#include <QAbstractTableModel>
#include <QDebug>

#include "muse_client.h"


class ArtistModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    ArtistModel(QObject *parent = 0);

    void populateData(struct artistinfolst* artists);

    int rowCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
    int columnCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;

private:
    QList<qint64> ids;
    QList<QString> names;

};

#endif // ARTISTMODEL_H
