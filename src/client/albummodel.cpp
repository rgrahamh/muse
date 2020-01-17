#include "albummodel.h"

AlbumModel::AlbumModel(QObject *parent) : QAbstractTableModel(parent)
{
}

// Create a method to populate the model with data:
void AlbumModel::populateData(struct albuminfolst* albums)
{
    // clear previous data
    this->ids.clear();
    this->titles.clear();
    this->years.clear();

    // populate new data
    struct albuminfolst* cursor = albums;
    while(cursor != NULL) {
        this->ids.append(cursor->id);
        this->titles.append(cursor->title);
        this->years.append(QString::number(cursor->year));

        cursor = cursor->next;
    }

    free_albuminfolst(albums);

    return;
}

int AlbumModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return ids.length();
}

int AlbumModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 2;
}

QVariant AlbumModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || role != Qt::DisplayRole) {
        return QVariant();
    }
    if (index.column() == 0) {
        return titles[index.row()];
    } else if (index.column() == 1) {
        return years[index.row()];
    }
    return QVariant();
}

QVariant AlbumModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole && orientation == Qt::Horizontal) {
        if (section == 0) {
            return QString("Album");
        } else if (section == 1) {
            return QString("Year");
        }
    }
    return QVariant();
}
