#include "removefromplaylistdialog.h"
#include "ui_removefromplaylistdialog.h"

RemoveFromPlaylistDialog::RemoveFromPlaylistDialog(QWidget *parent, struct playlist* playlist) :
    QDialog(parent),
    ui(new Ui::RemoveFromPlaylistDialog)
{
    ui->setupUi(this);

    song_model = new SongModel(this);

    // get songinfo on all the songs
    struct songinfolst* songs = NULL;

    struct songlst* cursor_sng = playlist->first_song;
    while( cursor_sng != NULL ) {
        querySongInfo(&songs, cursor_sng->id);

        cursor_sng = cursor_sng->next;
    }

    song_model->populateData(songs);

    // set selection behavior and mode for proper access
    ui->songView->setModel(song_model);
    ui->songView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->songView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->songView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->songView->horizontalHeader()->setHighlightSections(false);
    ui->songView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->songView->setSortingEnabled(true);

    // display
    ui->songView->horizontalHeader()->setVisible(true);
    ui->songView->verticalHeader()->setVisible(false);
    ui->songView->setVisible(true);
}

void RemoveFromPlaylistDialog::done(int status) {
    if( status == QDialog::Accepted ) {
        // remove the song from the playlist

        // check if a song was selected
        QItemSelectionModel* item_selection = ui->songView->selectionModel();

        // nothing selected, don't close dialog
        if( !item_selection->hasSelection() ) {
            return;
        }

        int row = item_selection->selectedRows().front().row();

        selected = row;

        QDialog::done(status);
        return;
    } else {
        QDialog::done(status);
        return;
    }
}

int RemoveFromPlaylistDialog::getSelected() {
    return selected;
}

RemoveFromPlaylistDialog::~RemoveFromPlaylistDialog()
{
    delete ui;
}

void RemoveFromPlaylistDialog::on_songView_doubleClicked(const QModelIndex &index)
{
    ui->songView->setCurrentIndex(index);
    this->done(QDialog::Accepted);
}
