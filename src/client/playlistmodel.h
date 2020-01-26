#ifndef PLAYLISTMODEL_H
#define PLAYLISTMODEL_H

#include <QAbstractItemModel>

#include "muse_client.h"

class PlaylistModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    PlaylistModel(QObject *parent = 0);

    void populateData(struct playlist* playlists);

    int rowCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
    int columnCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;

    void clearModel();

private:
    QList<char*> names;

};

#endif // PLAYLISTMODEL_H
