#ifndef GENREMODEL_H
#define GENREMODEL_H

#include <QAbstractTableModel>
#include <QDebug>

#include "muse_client.h"

class GenreModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    GenreModel(QObject *parent = 0);

    void populateData(struct genreinfolst* genres);

    int rowCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
    int columnCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;

    void clearModel();

private:
    QList<QString> genres;

};

#endif // GENREMODEL_H
