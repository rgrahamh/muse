#include "songmodel.h"

SongModel::SongModel(QObject *parent) : QAbstractTableModel(parent)
{
}

// Create a method to populate the model with data:
void SongModel::populateData(struct songinfolst* songs)
{
    // clear previous data
    this->ids.clear();
    this->titles.clear();
    this->artists.clear();
    this->albums.clear();
    this->years.clear();
    this->track_nums.clear();
    this->genres.clear();

    // populate new data
    struct songinfolst* cursor = songs;
    while(cursor != NULL) {
        this->ids.append(cursor->id);
        this->titles.append(cursor->title);
        this->artists.append(cursor->artist);
        this->albums.append(cursor->album);
        this->years.append(QString::number(cursor->year));
        this->track_nums.append(QString::number(cursor->track_num));
        this->genres.append(cursor->genre);

        cursor = cursor->next;
    }

    free_songinfolst(songs);

    return;
}

int SongModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return ids.length();
}

int SongModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 6;
}

QVariant SongModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || role != Qt::DisplayRole) {
        return QVariant();
    }
    if (index.column() == 0) {
        return track_nums[index.row()];
    } else if (index.column() == 1) {
        return titles[index.row()];
    } else if (index.column() == 2) {
        return artists[index.row()];
    } else if (index.column() == 3) {
        return albums[index.row()];
    } else if (index.column() == 4) {
        return years[index.row()];
    } else if (index.column() == 5) {
        return genres[index.row()];
    }
    return QVariant();
}

QVariant SongModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole && orientation == Qt::Horizontal) {
        if (section == 0) {
            return QString("Track");
        } else if (section == 1) {
            return QString("Song");
        } else if (section == 2) {
            return QString("Artist");
        } else if (section == 3) {
            return QString("Album");
        } else if (section == 4) {
            return QString("Year");
        } else if (section == 5) {
            return QString("Genre");
        }
    }
    return QVariant();
}
