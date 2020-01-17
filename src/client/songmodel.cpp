#include "songmodel.h"

SongModel::SongModel(QObject *parent) : QAbstractTableModel(parent)
{
}

// Create a method to populate the model with data:
void SongModel::populateData(const QList<QString> &titles, const QList<QString> &artists, const QList<QString> &albums, const QList<QString> &genres)
{
    this->titles.clear();
    this->artists.clear();
    this->albums.clear();
    this->genres.clear();

    this->titles = titles;
    this->artists = artists;
    this->albums = albums;
    this->genres = genres;
    return;
}

int SongModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return titles.length();
}

int SongModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 4;
}

QVariant SongModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || role != Qt::DisplayRole) {
        return QVariant();
    }
    if (index.column() == 0) {
        return titles[index.row()];
    } else if (index.column() == 1) {
        return artists[index.row()];
    } else if (index.column() == 2) {
        return albums[index.row()];
    } else if (index.column() == 3) {
        return genres[index.row()];
    }
    return QVariant();
}

QVariant SongModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole && orientation == Qt::Horizontal) {
        if (section == 0) {
            return QString("Song");
        } else if (section == 1) {
            return QString("Artist");
        } else if (section == 2) {
            return QString("Album");
        } else if (section == 3) {
            return QString("Genre");
        }
    }
    return QVariant();
}
