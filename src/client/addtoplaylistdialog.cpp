#include "addtoplaylistdialog.h"
#include "ui_addtoplaylistdialog.h"

AddToPlaylistDialog::AddToPlaylistDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddToPlaylistDialog)
{
    ui->setupUi(this);

    scanPlaylists(&playlists);

    playlist_model = new PlaylistModel(this);
    playlist_model->populateData(playlists);

    // set selection behavior and mode for proper access
    ui->playlistView->setModel(playlist_model);
    ui->playlistView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->playlistView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->playlistView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->playlistView->horizontalHeader()->setHighlightSections(false);
    ui->playlistView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->playlistView->setSortingEnabled(true);

    // display
    ui->playlistView->horizontalHeader()->setVisible(true);
    ui->playlistView->verticalHeader()->setVisible(false);
    ui->playlistView->setVisible(true);
}

AddToPlaylistDialog::~AddToPlaylistDialog()
{
    delete ui;
}

void AddToPlaylistDialog::done(int status) {
    if( status == QDialog::Accepted ) {
        struct playlist* cursor = playlists;

        // check if playlist was selected
        QItemSelectionModel* item_selection = ui->playlistView->selectionModel();

        // nothing selected, don't close dialog
        if( !item_selection->hasSelection() ) {
            return;
        }

        int row = item_selection->selectedRows().front().row();
        const char* playlist_name = playlist_model->data(playlist_model->index(row, 0)).value<QString>().toStdString().c_str();

        while(cursor != NULL) {
            // try to find playlist
            if( strcmp(playlist_name, cursor->name) == 0 ) {
                selected = cursor;
            }

            cursor = cursor->prev;
        }
        QDialog::done(status);
        return;
    } else {
        QDialog::done(status);
        return;
    }


}

struct playlist* AddToPlaylistDialog::getPlaylists() {
    return playlists;
}

struct playlist* AddToPlaylistDialog::getSelected() {
    return selected;
}

void AddToPlaylistDialog::on_createNewButton_clicked()
{
    if( strcmp(ui->newPlaylistName->text().toStdString().c_str(), "") != 0 ) {
        char* name = (char*) calloc(ui->newPlaylistName->text().length(), sizeof(char));
        strcpy(name, ui->newPlaylistName->text().toStdString().c_str());
        addPlaylist(name, &playlists);
    }

    playlist_model->populateData(playlists);

    ui->newPlaylistName->setText("");
}

void AddToPlaylistDialog::on_playlistView_doubleClicked(const QModelIndex &index)
{
    ui->playlistView->setCurrentIndex(index);
    this->done(QDialog::Accepted);
}
