#include "musewindow.h"
#include "ui_musewindow.h"

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

    // initialize audio library
    initializeFMOD();

    // stop user from messing with progress bar
    ui->songProgressSlider->setAttribute(Qt::WA_TransparentForMouseEvents);

    // allocate space for strings
    songProgressText = (char*) calloc(10, sizeof(char));
    songLengthText = (char*) calloc(10, sizeof(char));
    connectionText = (char*) calloc(100, sizeof(char));

    // intialize update timer
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &MuseWindow::on_timeout);

    // create the models
    ArtistModel *artist_model = new ArtistModel(this);
    AlbumModel *album_model = new AlbumModel(this);
    GenreModel *genre_model = new GenreModel(this);
    SongModel *song_model = new SongModel(this);

    // populate the models
    // song_model->populateData(test_song1);

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
    // release audio library resources
    system->release();

    free(songProgressText);
    free(songLengthText);
    free(connectionText);

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

void MuseWindow::initializeFMOD() {
    result = FMOD::System_Create(&system);      // Create the main system object.
    if (result != FMOD_OK)
    {
        printf("FMOD error! (%d) %s\n", result, FMOD_ErrorString(result));
        exit(-1);
    }

    result = system->init(512, FMOD_INIT_NORMAL, 0);    // Initialize FMOD.
    if (result != FMOD_OK)
    {
        printf("FMOD error! (%d) %s\n", result, FMOD_ErrorString(result));
        exit(-1);
    }
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

        bool is_playing = false;
        song_channel->isPlaying(&is_playing);
        if( is_playing ) {
            song_channel->setPaused(false);
        } else {
            // try playing a song
            system->createStream("/home/dfletch/Documents/School/muse/assets/music/summer_is_gone.mp3", FMOD_DEFAULT, NULL, &song_to_play);

            unsigned int song_length;
            song_to_play->getLength(&song_length, FMOD_TIMEUNIT_MS);

            unsigned int song_length_s = song_length / 1000;

            memset(songLengthText, 0, 10);
            sprintf(songLengthText, "%d:%02d", song_length_s / 60, song_length_s % 60);
            ui->songLengthLabel->setText(songLengthText);

            ui->songProgressSlider->setMinimum(0);
            ui->songProgressSlider->setMaximum(song_length);
            ui->songProgressSlider->setValue(0);

            system->playSound(song_to_play, NULL, false, &song_channel);
            timer->start(75);
        }

        // song_to_play->release();

    } else {
        ui->playButton->setText("Play");

        bool is_playing = false;
        song_channel->isPlaying(&is_playing);
        if( is_playing ) {
            song_channel->setPaused(true);
        }
    }
}

void MuseWindow::on_songView_doubleClicked(const QModelIndex &index)
{
    qDebug() << "Song index selected is: " << index.row() << endl;
}

void MuseWindow::on_rewindButton_clicked()
{
    bool is_playing = false;
    song_channel->isPlaying(&is_playing);
    if( is_playing ) {
        song_channel->setPosition(0, FMOD_TIMEUNIT_MS);
        timer->stop();
    }
}

void MuseWindow::on_timeout() {
    bool is_playing = false;
    song_channel->isPlaying(&is_playing);
    if( is_playing ) {
        unsigned int position;
        song_channel->getPosition(&position, FMOD_TIMEUNIT_MS);

        unsigned int position_s = position / 1000;

        memset(songProgressText, 0, 10);
        sprintf(songProgressText, "%d:%02d", position_s / 60, position_s % 60);
        ui->songProgressLabel->setText(songProgressText);

        ui->songProgressSlider->setValue(position);
    } else {
        // song has finished playback
        ui->songProgressSlider->setValue(0);
        ui->songProgressLabel->setText("0:00");
        ui->songLengthLabel->setText("-:--");
        play_state = false;
        ui->playButton->setText("Play");
        timer->stop();
    }
}

void MuseWindow::on_connectButton_clicked()
{
    connection_state = !connection_state;
    if( connection_state ) {
        ServerDialog* serverDialog = new ServerDialog(this);
        if( serverDialog->exec() == QDialog::Accepted ) {
            ip_address = serverDialog->getServerIP();
            port = serverDialog->getServerPort();

            memset(connectionText, 0, 100);
            sprintf(connectionText, "Connected to: %s:%s", ip_address.toStdString().c_str(), port.toStdString().c_str());
            ui->serverInfoLabel->setText(connectionText);
            ui->connectButton->setText("Disconnect");
        }
    } else {
        ui->serverInfoLabel->setText("Not connected to server.");
        ui->connectButton->setText("Connect to...");
    }
}
