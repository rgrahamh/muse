#include "artistmodel.h"

/**
 * @brief AristModel::ArtistModel Constructor for the ArtistModel class
 * @param parent The object responsible for memory management on this object
 */
ArtistModel::ArtistModel(QObject *parent) : QAbstractTableModel(parent)
{
}

/**
 * @brief ArtistModel::populateData Loads information from an outside source into the model
 * @param artists An artistinfolst linked-list from "muse_client.h"
 */
void ArtistModel::populateData(struct artistinfolst* artists)
{
	beginResetModel();

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

	endResetModel();

	return;
}

/**
 * @brief ArtistModel::addData Adds data to the model
 * @param artists An artistinfolst linked-list from "muse_client.h"
 */
void ArtistModel::addData(struct artistinfolst* artists)
{
	struct artistinfolst* cursor = artists;
	int iter = 0;
	while(cursor != NULL) {
		iter++;
		cursor = cursor->next;
	}
    beginInsertRows(QModelIndex(), rowCount(), rowCount() + iter - 1);

	// populate new data
	cursor = artists;
	while(cursor != NULL) {
		this->ids.append(cursor->id);
		this->names.append(cursor->name);

		cursor = cursor->next;
	}

	endInsertRows();

	return;
}

/**
 * @brief ArtistModel::rowCount Returns the number of rows in the model
 * @param parent Overridden from parent class
 * @return int The number of rows in the model
 */
int ArtistModel::rowCount(const QModelIndex &parent) const
{
	Q_UNUSED(parent);
	return ids.length();
}

/**
 * @brief ArtistModel::columnCount Returns the number of columns in the model
 * @param parent Overridden from parent class
 * @return int The number of columns in the model
 */
int ArtistModel::columnCount(const QModelIndex &parent) const
{
	Q_UNUSED(parent);
	return 1;
}

/**
 * @brief ArtistModel::data Retrieves a single table cell from the model
 * @param index Overridden from parent class
 * @param role Overridden from parent class
 * @return QVariant The value of the selected cell
 */
QVariant ArtistModel::data(const QModelIndex &index, int role) const
{
	if (!index.isValid() || (role != Qt::DisplayRole && role != Qt::UserRole)) {
		return QVariant();
	}
	else if(role == Qt::UserRole) {
		return ids[index.row()];
	}
	if (index.column() == 0) {
		return names[index.row()];
	}
	return QVariant();
}

/**
 * @brief ArtistModel::headerData Retrieves the header information for the table
 * @param section Overridden from parent class
 * @param orientation Overridden from parent class
 * @param role Overridden from parent class
 * @return QVariant The header label value
 */
QVariant ArtistModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (role == Qt::DisplayRole && orientation == Qt::Horizontal) {
		if (section == 0) {
			return QString("Artist");
		}
	}
	return QVariant();
}

void ArtistModel::clearModel() {
	beginResetModel();

	// clear previous data
	this->ids.clear();
	this->names.clear();

	endResetModel();
}
