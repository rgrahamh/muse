#include "playlistmodel.h"

PlaylistModel::PlaylistModel(QObject *parent) : QAbstractTableModel(parent)
{
}

void PlaylistModel::populateData(struct playlist* playlists)
{
    beginResetModel();

    this->names.clear();

    struct playlist* cursor = playlists;
    while( cursor != NULL ) {


        this->names.append(cursor->name);

        cursor = cursor->prev;
    }

    endResetModel();
}

int PlaylistModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return names.length();
}

int PlaylistModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 1;
}

QVariant PlaylistModel::data(const QModelIndex &index, int role) const
{
    if(!index.isValid() || (role != Qt::DisplayRole && role != Qt::UserRole)) {
        return QVariant();
    } else if(role == Qt::UserRole) {
        // return helpful information here
    }
    if( index.column() == 0 ) {
        return names[index.row()];
    }
    return QVariant();
}

QVariant PlaylistModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if( role == Qt::DisplayRole && orientation == Qt::Horizontal ) {
        if( section == 0 ) {
            return QString("Playlist");
        }
    }

    return QVariant();
}

void PlaylistModel::clearModel() {
    beginResetModel();

    this->names.clear();

    endResetModel();
}
