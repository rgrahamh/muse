#include "artistmodel.h"

ArtistModel::ArtistModel(QObject *parent) : QAbstractTableModel(parent)
{
}

// Create a method to populate the model with data:
void ArtistModel::populateData(struct artistinfolst* artists)
{
    // clear previous data
    this->ids.clear();
    this->names.clear();

    // populate new data
    struct artistinfolst* cursor = artists;
    while(cursor != NULL) {
        this->ids.append(cursor->id);
        this->names.append(cursor->name);

        cursor = cursor->next;
    }

    free_artistinfolst(artists);

    return;
}

int ArtistModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return ids.length();
}

int ArtistModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 1;
}

QVariant ArtistModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || role != Qt::DisplayRole) {
        return QVariant();
    }
    if (index.column() == 0) {
        return names[index.row()];
    }
    return QVariant();
}

QVariant ArtistModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole && orientation == Qt::Horizontal) {
        if (section == 0) {
            return QString("Artist");
        }
    }
    return QVariant();
}
