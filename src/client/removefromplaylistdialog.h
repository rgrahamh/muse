#ifndef REMOVEFROMPLAYLISTDIALOG_H
#define REMOVEFROMPLAYLISTDIALOG_H

#include <QDialog>

#include "songmodel.h"
#include "playlistmodel.h"
#include "muse_client.h"

namespace Ui {
class RemoveFromPlaylistDialog;
}

class RemoveFromPlaylistDialog : public QDialog
{
    Q_OBJECT

public:
    explicit RemoveFromPlaylistDialog(QWidget *parent = nullptr, struct playlist* playlist = NULL);
    void done(int status) override;

    int getSelected();

    ~RemoveFromPlaylistDialog();

private slots:
    void on_songView_doubleClicked(const QModelIndex &index);

private:
    Ui::RemoveFromPlaylistDialog *ui;

    SongModel* song_model;

    int selected;
};

#endif // REMOVEFROMPLAYLISTDIALOG_H
