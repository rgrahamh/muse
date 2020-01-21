#include "albummodel.h"

/**
 * @brief AlbumModel::AlbumModel Constructor for the AlbumModel class
 * @param parent The object responsible for memory management on this object
 */
AlbumModel::AlbumModel(QObject *parent) : QAbstractTableModel(parent)
{
}

/**
 * @brief AlbumModel::populateData Loads information from an outside source into the model
 * @param albums An albuminfolst linked-list from "muse_client.h"
 */
void AlbumModel::populateData(struct albuminfolst* albums)
{
    beginResetModel();

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

    endResetModel();

    return;
}

/**
 * @brief AlbumModel::rowCount Returns the number of rows in the model
 * @param parent Overridden from parent class
 * @return int The number of rows in the model
 */
int AlbumModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return ids.length();
}

/**
 * @brief AlbumModel::columnCount Returns the number of columns in the model
 * @param parent Overridden from parent class
 * @return int The number of columns in the model
 */
int AlbumModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 2;
}

/**
 * @brief AlbumModel::data Retrieves a single table cell from the model
 * @param index Overridden from parent class
 * @param role Overridden from parent class
 * @return QVariant The value of the selected cell
 */
QVariant AlbumModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || (role != Qt::DisplayRole && role != Qt::UserRole)) {
        return QVariant();
    }
    else if(role == Qt::UserRole) {
        return ids[index.row()];
    }
    if (index.column() == 0) {
        return titles[index.row()];
    } else if (index.column() == 1) {
        return years[index.row()];
    }
    return QVariant();
}

/**
 * @brief AlbumModel::headerData Retrieves the header information for the table
 * @param section Overridden from parent class
 * @param orientation Overridden from parent class
 * @param role Overridden from parent class
 * @return QVariant The header label value
 */
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
