#include "genremodel.h"

/**
 * @brief GenreModel::GenreModel Constructor for the GenreModel class
 * @param parent The object responsible for memory management on this object
 */
GenreModel::GenreModel(QObject *parent) : QAbstractTableModel(parent)
{
}

/**
 * @brief GenreModel::populateData Loads information from an outside source into the model
 * @param genres An genreinfolst linked-list from "muse_client.h"
 */
void GenreModel::populateData(struct genreinfolst* genres)
{
    beginResetModel();

    // clear previous data
    this->genres.clear();

    // populate new data
    struct genreinfolst* cursor = genres;
    while(cursor != NULL) {
        this->genres.append(cursor->genre);

        cursor = cursor->next;
    }

    free_genreinfolst(genres);

    endResetModel();

    return;
}

/**
 * @brief GenreModel::rowCount Returns the number of rows in the model
 * @param parent Overridden from parent class
 * @return int The number of rows in the model
 */
int GenreModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return genres.length();
}

/**
 * @brief GenreModel::columnCount Returns the number of columns in the model
 * @param parent Overridden from parent class
 * @return int The number of columns in the model
 */
int GenreModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 1;
}

/**
 * @brief GenreModel::data Retrieves a single table cell from the model
 * @param index Overridden from parent class
 * @param role Overridden from parent class
 * @return QVariant The value of the selected cell
 */
QVariant GenreModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || role != Qt::DisplayRole) {
        return QVariant();
    }
    if (index.column() == 0) {
        return genres[index.row()];
    }
    return QVariant();
}

/**
 * @brief GenreModel::headerData Retrieves the header information for the table
 * @param section Overridden from parent class
 * @param orientation Overridden from parent class
 * @param role Overridden from parent class
 * @return QVariant The header label value
 */
QVariant GenreModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole && orientation == Qt::Horizontal) {
        if (section == 0) {
            return QString("Genre");
        }
    }
    return QVariant();
}
