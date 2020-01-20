#ifndef SONGMODEL_H
#define SONGMODEL_H

#include <QAbstractTableModel>
#include <QDebug>

#include "muse_client.h"


class SongModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    SongModel(QObject *parent = 0);

    void populateData(struct songinfolst* songs);

    int rowCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
    int columnCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;

private:
    QList<qint64> ids;
    QList<QString> titles;
    QList<QString> artists;
    QList<QString> albums;
    QList<QString> years;
    QList<QString> track_nums;
    QList<QString> genres;

};

#endif // SONGMODEL_H
