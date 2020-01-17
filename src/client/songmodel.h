#ifndef SONGMODEL_H
#define SONGMODEL_H

#include <QAbstractTableModel>
#include <QDebug>


class SongModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    SongModel(QObject *parent = 0);

    // TODO: change prototype to accept a list of songinfo structs
    void populateData(const QList<QString> &titles, const QList<QString> &artists, const QList<QString> &albums, const QList<QString> &genres);

    int rowCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
    int columnCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;

private:
    QList<long> id;
    QList<QString> titles;
    QList<QString> artists;
    QList<QString> albums;
    QList<QString> years;
    QList<QString> track_nums;
    QList<QString> genres;

};

#endif // SONGMODEL_H
