#include "musewindow.h"
#include "ui_musewindow.h"
#include "songmodel.h"
#include "artistmodel.h"
#include "albummodel.h"
#include "genremodel.h"

MuseWindow::MuseWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MuseWindow)
{
    ui->setupUi(this);
    ui->tabWidget->setCurrentIndex(0);

    // configure tables
    configureTableView(ui->artistView);
    configureTableView(ui->albumView);
    configureTableView(ui->genreView);
    configureTableView(ui->songView);

    // configure song scrubber
    ui->songProgressSlider->setMinimum(0);
    ui->songProgressSlider->setMaximum(100);
    ui->songProgressSlider->setTickInterval(1);

    // create some test data
    QList<QString> song_titles;
    QList<QString> artist_names;
    QList<QString> album_names;
    QList<QString> genres;

    song_titles.append("Anchor");
    artist_names.append("Skillet");
    album_names.append("Save Me");
    genres.append("Rock");

    song_titles.append("Plain White Rapper");
    artist_names.append("KJ-52");
    album_names.append("Behind the Musik");
    genres.append("Rap");

    // create the models
    ArtistModel *artist_model = new ArtistModel(this);
    AlbumModel *album_model = new AlbumModel(this);
    GenreModel *genre_model = new GenreModel(this);
    SongModel *song_model = new SongModel(this);

    // populate the models
    song_model->populateData(song_titles, artist_names, album_names, genres);

    // allow filtering by use of proxy models
    QSortFilterProxyModel *artist_proxy = new QSortFilterProxyModel();
    artist_proxy->setSourceModel(artist_model);
    ui->artistView->setModel(artist_proxy);

    QSortFilterProxyModel *album_proxy = new QSortFilterProxyModel();
    album_proxy->setSourceModel(album_model);
    ui->albumView->setModel(album_proxy);

    QSortFilterProxyModel *genre_proxy = new QSortFilterProxyModel();
    genre_proxy->setSourceModel(genre_model);
    ui->genreView->setModel(genre_proxy);

    QSortFilterProxyModel *song_proxy = new QSortFilterProxyModel();
    song_proxy->setSourceModel(song_model);
    ui->songView->setModel(song_proxy);
}

MuseWindow::~MuseWindow()
{
    delete ui;
}

void MuseWindow::configureTableView(QTableView* view) {
    // set selection behavior and mode for proper access
    view->setSelectionBehavior(QAbstractItemView::SelectRows);
    view->setSelectionMode(QAbstractItemView::SingleSelection);
    view->setEditTriggers(QAbstractItemView::NoEditTriggers);
    view->horizontalHeader()->setHighlightSections(false);
    view->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    view->setSortingEnabled(true);

    // display
    view->horizontalHeader()->setVisible(true);
    view->setVisible(true);
}

void MuseWindow::on_tabWidget_currentChanged(int index)
{
    /* The tab has changed, so we need to update the view with new data */
    switch(index) {
//        case 0: /* Artist */
//            ui->artistView->addItem("ARTIST");
//            break;
//        case 1: /* Album */
//            ui->albumView->addItem("ALBUM");
//            break;
//        case 2: /* Genre */
//            ui->genreView->addItem("GENRE");
//            break;
//        case 3: /* Song */
//            break;
    }
}

void MuseWindow::on_playButton_clicked()
{
    play_state = !play_state;
    if( play_state ) {
        ui->playButton->setText("Pause");
    } else {
        ui->playButton->setText("Play");
    }
}

void MuseWindow::on_songView_doubleClicked(const QModelIndex &index)
{
    qDebug() << "Song index selected is: " << index.row() << endl;
}
