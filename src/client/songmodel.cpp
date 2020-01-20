#include "songmodel.h"

/**
 * @brief SongModel::SongModel Constructor for the SongModel class
 * @param parent The object responsible for memory management on this object
 */
SongModel::SongModel(QObject *parent) : QAbstractTableModel(parent)
{
}

/**
 * @brief SongModel::populateData Loads information from an outside source into the model
 * @param songs An songinfolst linked-list from "muse_client.h"
 */
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

/**
 * @brief SongModel::rowCount Returns the number of rows in the model
 * @param parent Overridden from parent class
 * @return int The number of rows in the model
 */
int SongModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return ids.length();
}

/**
 * @brief SongModel::columnCount Returns the number of columns in the model
 * @param parent Overridden from parent class
 * @return int The number of columns in the model
 */
int SongModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 6;
}

/**
 * @brief SongModel::data Retrieves a single table cell from the model
 * @param index Overridden from parent class
 * @param role Overridden from parent class
 * @return QVariant The value of the selected cell
 */
QVariant SongModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || (role != Qt::DisplayRole && role != Qt::UserRole)) {
        return QVariant();
    }
    else if(role == Qt::UserRole) {
        return ids[index.row()];
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

/**
 * @brief SongModel::headerData Retrieves the header information for the table
 * @param section Overridden from parent class
 * @param orientation Overridden from parent class
 * @param role Overridden from parent class
 * @return QVariant The header label value
 */
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
