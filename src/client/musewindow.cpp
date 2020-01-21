#include "musewindow.h"
#include "ui_musewindow.h"

/**
 * @brief MuseWindow::MuseWindow Constructor for the main window of the client UI
 * @param parent The parent to attach to
 */
MuseWindow::MuseWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MuseWindow)
{
    // ui initialization
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
    timer->start(75);

    // create the models
    artist_model = new ArtistModel(this);
    album_model = new AlbumModel(this);
    genre_model = new GenreModel(this);
    song_model = new SongModel(this);

//    // populate the models
//    song_model->populateData(getTestSongs());
//    artist_model->populateData(getTestArtists());
//    album_model->populateData(getTestAlbums());
//    genre_model->populateData(getTestGenres());

//    // allow filtering by use of proxy models
//    QSortFilterProxyModel *artist_proxy = new QSortFilterProxyModel();
//    artist_proxy->setSourceModel(artist_model);
//    ui->artistView->setModel(artist_proxy);

//    QSortFilterProxyModel *album_proxy = new QSortFilterProxyModel();
//    album_proxy->setSourceModel(album_model);
//    ui->albumView->setModel(album_proxy);

//    QSortFilterProxyModel *genre_proxy = new QSortFilterProxyModel();
//    genre_proxy->setSourceModel(genre_model);
//    ui->genreView->setModel(genre_proxy);

//    QSortFilterProxyModel *song_proxy = new QSortFilterProxyModel();
//    song_proxy->setSourceModel(song_model);
//    ui->songView->setModel(song_proxy);

    ui->artistView->setModel(artist_model);
    ui->albumView->setModel(album_model);
    ui->genreView->setModel(genre_model);
    ui->songView->setModel(song_model);

}

/**
 * @brief MuseWindow::~MuseWindow Deconstructor
 */
MuseWindow::~MuseWindow()
{
    // release audio library resources
    system->release();

    free(songProgressText);
    free(songLengthText);
    free(connectionText);

    delete ui;
}

/**
 * @brief MuseWindow::configureTableView Sets project defaults for each table view
 * @param view The QTableView being processed
 */
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
    view->verticalHeader()->setVisible(false);
    view->setVisible(true);
}

/**
 * @brief MuseWindow::initializeFMOD Readies audio-library for playback
 */
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

/**
 * @brief MuseWindow::on_tabWidget_currentChanged Slot for when a tab changes
 * @param index The index of the tab that was switched to
 */
void MuseWindow::on_tabWidget_currentChanged(int index)
{
    /* The tab has changed, so we need to update the view with new data */
    if( connection_state )  {
        switch(index) {
            case 0: /* Artist */
                struct artistinfolst* artists;
                if( int err = queryArtists(&artists) ) {
                    qDebug() << "Error fetching artists!" << endl;
                } else {
                    artist_model->populateData(artists);
                }
                break;
            case 1: /* Album */
                struct albuminfolst* albums;
                if( int err = queryAlbums(&albums) ) {
                    qDebug() << "Error fetching albums!" << endl;
                } else {
                    album_model->populateData(albums);
                }
                break;
            case 2: /* Genre */
                struct genreinfolst* genres;
                if( int err = queryGenres(&genres) ) {
                    qDebug() << "Error fetching genres!" << endl;
                } else {
                    genre_model->populateData(genres);
                }
                break;
            case 3: /* Song */
                struct songinfolst* songs;
                if( int err = querySongs(&songs) ) {
                    qDebug() << "Error fetching songs!" << endl;
                } else {
                    song_model->populateData(songs);
                }
                break;
        }
    }
}

/**QString
 * @brief MuseWindow::on_songView_doubleClicked Slot for when a row in the SongTable is double-clicked
 * @param index The index of the cell double clicked
 */
void MuseWindow::on_songView_doubleClicked(const QModelIndex &index)
{
    qDebug() << "Song id selected is: " << index.data(Qt::UserRole).value<int>() << endl;
}

/**
 * @brief MuseWindow::on_artistView_doubleClicked Slot for when a row in the ArtistTable is double-clicked
 * @param index The index of the cell double clicked
 */
void MuseWindow::on_artistView_doubleClicked(const QModelIndex &index)
{
    qDebug() << "Artist id selected is: " << index.data(Qt::UserRole).value<int>() << endl;
}

/**
 * @brief MuseWindow::on_albumView_doubleClicked Slot for when a row in the AlbumTable is double-clicked
 * @param index The index of the cell double clicked
 */
void MuseWindow::on_albumView_doubleClicked(const QModelIndex &index)
{
    qDebug() << "Album id selected is: " << index.data(Qt::UserRole).value<int>() << endl;
}

/**
 * @brief MuseWindow::on_genreView_doubleClicked Slot for when a row in the GenreTable is double-clicked
 * @param index The index of the cell double clicked
 */
void MuseWindow::on_genreView_doubleClicked(const QModelIndex &index)
{
    qDebug() << "Genre selected is: " << index.data().value<QString>() << endl;
}

/**
 * @brief MuseWindow::on_playButton_clicked Slot for when the play button is pressed
 */
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

/**
 * @brief MuseWindow::on_rewindButton_clicked Slot for when the rewind button is pressed
 */
void MuseWindow::on_rewindButton_clicked()
{
    bool is_playing = false;
    song_channel->isPlaying(&is_playing);
    if( is_playing ) {
        song_channel->setPosition(0, FMOD_TIMEUNIT_MS);
        ui->songProgressLabel->setText("0:00");
        ui->songProgressSlider->setValue(0);
    }
}

/**
 * @brief MuseWindow::on_connectButton_clicked Slot for when the connection button is pressed
 */
void MuseWindow::on_connectButton_clicked()
{
    if( !connection_state ) {
        ServerDialog* serverDialog = new ServerDialog(this);
        if( serverDialog->exec() == QDialog::Accepted ) {
            ip_address = serverDialog->getServerIP();
            port = serverDialog->getServerPort();

            if( int err = connectToServ(port.toStdString().c_str(), ip_address.toStdString().c_str()) ) {
                qDebug() << "Error connecting to server!" << endl;
            } else {
                memset(connectionText, 0, 100);
                sprintf(connectionText, "Connected to: %s:%s", ip_address.toStdString().c_str(), port.toStdString().c_str());
                ui->serverInfoLabel->setText(connectionText);
                ui->connectButton->setText("Disconnect");
                connection_state = true;

                ui->tabWidget->setCurrentIndex(0);
            }
        }
    } else {

        connection_state = false;
        ui->serverInfoLabel->setText("Not connected to server.");
        ui->connectButton->setText("Connect to...");
    }
}

/**
 * @brief MuseWindow::on_timeout Slot for when the update poll timer times out
 */
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
    }
}
