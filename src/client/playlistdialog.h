#ifndef PLAYLISTDIALOG_H
#define PLAYLISTDIALOG_H

#include <QDialog>

#include "playlistmodel.h"
#include "muse_client.h"

namespace Ui {
class PlaylistDialog;
}

class PlaylistDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PlaylistDialog(QWidget *parent = nullptr);
    void done(int status) override;

    struct playlist* getPlaylists();
    struct playlist* getSelected();

    ~PlaylistDialog();

private slots:
    void on_createNewButton_clicked();

    void on_playlistView_doubleClicked(const QModelIndex &index);

private:
    Ui::PlaylistDialog *ui;
    int playlist_id;

    struct playlist* playlists = NULL;
    struct playlist* selected = NULL;
    PlaylistModel* playlist_model;
};

#endif // PLAYLISTDIALOG_H
