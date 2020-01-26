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
    configureTableView(ui->playlistView);

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
    artist_model = new ArtistModel(this);
    album_model = new AlbumModel(this);
    genre_model = new GenreModel(this);
    song_model = new SongModel(this);
    playlist_model = new PlaylistModel(this);

    ui->artistView->setModel(artist_model);
    ui->albumView->setModel(album_model);
    ui->genreView->setModel(genre_model);
    ui->songView->setModel(song_model);
    ui->playlistView->setModel(playlist_model);

    // request context menu for right-clicks
    ui->songView->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->songView, SIGNAL(customContextMenuRequested(QPoint)), SLOT(customMenuRequested(QPoint)));

    // last-minute setup
    changeConnectionState(NOT_CONNECTED);
    clearSongs();
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
    free_playlist(playlists);

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

void MuseWindow::on_tabWidget_tabBarClicked(int index)
{
    /* The tab has changed, so we need to update the view with new data */
    if( connection_state )  {
        switch(index) {
            case 0: /* Artist */ {
                struct artistinfolst* artists;
                if( queryArtists(&artists) ) {
                    qDebug() << "Error fetching artists!" << endl;
                } else {
                    artist_model->populateData(artists);
                }
                break;
            }
            case 1: /* Album */ {
                struct albuminfolst* albums;
                if( queryAlbums(&albums) ) {
                    qDebug() << "Error fetching albums!" << endl;
                } else {
                    album_model->populateData(albums);
                }
                break;
            }
            case 2: /* Genre */ {
                struct genreinfolst* genres;
                if( queryGenres(&genres) ) {
                    qDebug() << "Error fetching genres!" << endl;
                } else {
                    genre_model->populateData(genres);
                }
                break;
            }
            case 3: /* Song */ {
                struct songinfolst* songs;
                if( querySongs(&songs) ) {
                    qDebug() << "Error fetching songs!" << endl;
                } else {
                    song_model->populateData(songs);
                }
                break;
            }
            case 4: /* Playlist */  {
                free_playlist(playlists);

                playlists = NULL;

                scanPlaylists(&playlists);
                if( playlists == NULL ) {
                    qDebug() << "Error scanning for playlists!" << endl;
                } else {
                    playlist_model->populateData(playlists);
                }
                break;
            }
        }
    }
}

/**QString
 * @brief MuseWindow::on_songView_doubleClicked Slot for when a row in the SongTable is double-clicked
 * @param index The index of the cell double clicked
 */
void MuseWindow::on_songView_doubleClicked(const QModelIndex &index)
{
    int song_id = index.data(Qt::UserRole).value<int>();

    qDebug() << "Song id selected is: " << song_id << endl;

    if( connection_state == CONNECTED ) {
        changePlayState(NOT_PLAYING);
        if( queue_state == HAS_QUEUE ) {  // prevent duplicates of songs double-clicked from entering history
            changeHistoryState(HAS_HISTORY);
        }
        changeQueueState(NO_QUEUE); // clear the current queue
        changeQueueState(HAS_QUEUE, &index, false); // re-initialize the queue
        changePlayState(STARTED);
    }
}

/**
 * @brief MuseWindow::on_artistView_doubleClicked Slot for when a row in the ArtistTable is double-clicked
 * @param index The index of the cell double clicked
 */
void MuseWindow::on_artistView_doubleClicked(const QModelIndex &index)
{
    int artist_id = index.data(Qt::UserRole).value<int>();

    qDebug() << "Artist id selected is: " << artist_id << endl;

    struct albuminfolst* albums;
    if( queryArtistAlbums(artist_id, &albums) ) {
        qDebug() << "Error retrieving artist albums!" << endl;
        return;
    }

    album_model->populateData(albums);
    ui->tabWidget->setCurrentIndex(1);
}

/**
 * @brief MuseWindow::on_albumView_doubleClicked Slot for when a row in the AlbumTable is double-clicked
 * @param index The index of the cell double clicked
 */
void MuseWindow::on_albumView_doubleClicked(const QModelIndex &index)
{
    int album_id = index.data(Qt::UserRole).value<int>();

    qDebug() << "Album id selected is: " << album_id << endl;

    struct songinfolst* songs;
    if( queryAlbumSongs(album_id, &songs) ) {
        qDebug() << "Error retrieving album songs!" << endl;
        return;
    }

    song_model->populateData(songs);
    ui->tabWidget->setCurrentIndex(3);
}

/**
 * @brief MuseWindow::on_genreView_doubleClicked Slot for when a row in the GenreTable is double-clicked
 * @param index The index of the cell double clicked
 */
void MuseWindow::on_genreView_doubleClicked(const QModelIndex &index)
{
    const char* genre = index.data(Qt::DisplayRole).value<QString>().toStdString().c_str();

    qDebug() << "Genre selected is: " << genre << endl;

    struct songinfolst* songs;
    if( queryGenreSongs(genre, &songs) ) {
        qDebug() << "Error retrieving genre songs!" << endl;
        return;
    }

    song_model->populateData(songs);
    ui->tabWidget->setCurrentIndex(3);
}

void MuseWindow::on_playlistView_doubleClicked(const QModelIndex &index)
{
    const char* playlist_name = index.data(Qt::DisplayRole).value<QString>().toStdString().c_str();

    qDebug() << "Playlist selected is: " << playlist_name << endl;

    // find the playlist
    struct playlist* cursor_pl = playlists;
    struct playlist* playlist = NULL;
    while( cursor_pl != NULL ) {
        if( strcmp(playlist_name, cursor_pl->name) == 0 ) {
            playlist = cursor_pl;
            break;
        }

        cursor_pl = cursor_pl->prev;
    }

    // get songinfo on all the songs
    struct songinfolst* songs = NULL;

    struct songlst* cursor_sng = playlist->first_song;
    while( cursor_sng != NULL ) {
        querySongInfo(&songs, cursor_sng->id);

        cursor_sng = cursor_sng->next;
    }

    song_model->populateData(songs);
    ui->tabWidget->setCurrentIndex(3);
}

/**
 * @brief MuseWindow::on_playButton_clicked Slot for when the play button is pressed
 */
void MuseWindow::on_playButton_clicked()
{
    switch( play_state ) {
        case NOT_PLAYING: {
            if( connection_state == CONNECTED && (queue_state == HAS_QUEUE || queue_state == END_OF_QUEUE) ) {
                changePlayState(STARTED);
            } // if we're not connected or don't have a queue, we can't play songs
            break;
        }
        case STARTED: {
            changePlayState(PAUSED);
            break;
        }
        case RESUMED: {
            changePlayState(PAUSED);
            break;
        }
        case PAUSED: {
            changePlayState(RESUMED);
            break;
        }
    }
}

/**
 * @brief MuseWindow::on_rewindButton_clicked Slot for when the rewind button is pressed
 */
void MuseWindow::on_rewindButton_clicked()
{
    switch( history_state ) {
        case NO_HISTORY: {
            // just rewind the song
            if( play_state != NOT_PLAYING ) {
                song_channel->setPosition(0, FMOD_TIMEUNIT_MS);
                ui->songProgressLabel->setText("0:00");
                ui->songProgressSlider->setValue(0);

                changePlayState(RESUMED);
            }
            break;
        }
        case HAS_HISTORY: {
            if( play_state != NOT_PLAYING ) {
                unsigned int position;
                song_channel->getPosition(&position, FMOD_TIMEUNIT_MS);
                if( position < 1500 ) {
                    changePlayState(NOT_PLAYING);

                    queue.insert(queue.begin(), history.front());
                    history.erase(history.begin());

                    changeQueueState(HAS_QUEUE);

                    if( history.empty() ) {
                        changeHistoryState(NO_HISTORY);
                    }

                    changePlayState(STARTED);
                } else {
                    song_channel->setPosition(0, FMOD_TIMEUNIT_MS);
                    ui->songProgressLabel->setText("0:00");
                    ui->songProgressSlider->setValue(0);

                    changePlayState(RESUMED);
                }
            } else {
                changePlayState(NOT_PLAYING);

                queue.insert(queue.begin(), history.front());
                history.erase(history.begin());

                changeQueueState(HAS_QUEUE);

                if( history.empty() ) {
                    changeHistoryState(NO_HISTORY);
                }

                changePlayState(STARTED);
            }
            break;
        }
    }
}

void MuseWindow::on_skipButton_clicked()
{
    // get the easiest case out of the way first
    if( repeat_state == REPEAT_ONE && play_state != NOT_PLAYING ) {
        song_channel->setPosition(0, FMOD_TIMEUNIT_MS);
        ui->songProgressLabel->setText("0:00");
        ui->songProgressSlider->setValue(0);

        changePlayState(RESUMED);
        return;
    }

    switch( queue_state ) {
        case NO_QUEUE: {
            break;
        }
        case HAS_QUEUE: {
            changePlayState(NOT_PLAYING);
            changeHistoryState(HAS_HISTORY);

            queue.erase(queue.begin());

            if( queue.size() == 1 ) {
                changeQueueState(END_OF_QUEUE);
            }

            changePlayState(STARTED);
            break;
        }
        case END_OF_QUEUE: {
            switch( repeat_state ) {
                case NO_REPEAT: {
                    changeHistoryState(HAS_HISTORY);
                    changeQueueState(NO_QUEUE);
                    changePlayState(NOT_PLAYING);
                    break;
                }
                case REPEAT: {
                    changePlayState(NOT_PLAYING);

                    changeHistoryState(HAS_HISTORY);
                    changeQueueState(NO_QUEUE);
                    QModelIndex index = song_model->index(0, 0);
                    changeQueueState(HAS_QUEUE, &index);

                    changePlayState(STARTED);
                    break;
                }
                case REPEAT_ONE: {
                    // covered above
                    break;
                }
            }

            break;
        }
    }
}

void MuseWindow::on_repeatButton_clicked()
{
    switch( repeat_state ) {
        case NO_REPEAT: {
            changeRepeatState(REPEAT);
            break;
        }
        case REPEAT: {
            changeRepeatState(REPEAT_ONE);
            break;
        }
        case REPEAT_ONE: {
            changeRepeatState(NO_REPEAT);
            break;
        }
    }
}

void MuseWindow::on_shuffleButton_clicked()
{
    switch( shuffle_state ) {
        case NO_SHUFFLE: {
            changeShuffleState(SHUFFLE);
            break;
        }
        case SHUFFLE: {
            changeShuffleState(NO_SHUFFLE);
            break;
        }
    }
}

/**
 * @brief MuseWindow::on_connectButton_clicked Slot for when the connection button is pressed
 */
void MuseWindow::on_connectButton_clicked()
{
    switch( connection_state ) {
        case NOT_CONNECTED: {
            ServerDialog* serverDialog = new ServerDialog(this);
            if( serverDialog->exec() == QDialog::Accepted ) {
                ip_address = serverDialog->getServerIP();
                port = serverDialog->getServerPort();

                if( connectToServ(port.toStdString().c_str(), ip_address.toStdString().c_str()) ) {
                    qDebug() << "Error connecting to server!" << endl;
                } else {
                    // handle state
                    changePlayState(NOT_PLAYING);
                    changeHistoryState(NO_HISTORY);
                    changeQueueState(NO_QUEUE);
                    changeConnectionState(CONNECTED);
                    changeRepeatState(NO_REPEAT);
                    changeShuffleState(NO_SHUFFLE);
                }
            }
            break;
        }
        case CONNECTED: {
            changePlayState(NOT_PLAYING);
            changeHistoryState(NO_HISTORY);
            changeQueueState(NO_QUEUE);
            changeConnectionState(NOT_CONNECTED);
            changeRepeatState(NO_REPEAT);
            changeShuffleState(NO_SHUFFLE);
            break;
        }
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
        // get the easy case out of the way
        if( repeat_state == REPEAT_ONE ) {
            changePlayState(STARTED);
            return;
        }
        switch( queue_state ) {
            case NO_QUEUE: {
                break;
            }
            case HAS_QUEUE: {
                changePlayState(NOT_PLAYING);
                changeHistoryState(HAS_HISTORY);

                queue.erase(queue.begin());

                if( queue.size() == 1 ) {
                    changeQueueState(END_OF_QUEUE);
                }

                changePlayState(STARTED);
                break;
            }
            case END_OF_QUEUE: {
                switch( repeat_state ) {
                    case NO_REPEAT: {
                        changeQueueState(NO_QUEUE);
                        changePlayState(NOT_PLAYING);
                        break;
                    }
                    case REPEAT: {
                        changePlayState(NOT_PLAYING);

                        changeQueueState(NO_QUEUE);
                        QModelIndex index = song_model->index(0, 0);
                        changeQueueState(HAS_QUEUE, &index);

                        changePlayState(STARTED);
                        break;
                    }
                    case REPEAT_ONE: {
                        // covered above
                        break;
                    }
                }
                break;
            }
        }
    }
}

void MuseWindow::clearModels() {
    song_model->clearModel();
    artist_model->clearModel();
    album_model->clearModel();
    genre_model->clearModel();
    playlist_model->clearModel();
}

void MuseWindow::stopAndReadyUpFMOD() {
    ui->playButton->setText("Play");
    ui->songProgressLabel->setText("0:00");
    ui->songProgressSlider->setValue(0);
    ui->songInfoLabel->setText("");

    song_channel->stop();
    song_to_play->release();

    song_channel = NULL;
    song_to_play = NULL;
}

void MuseWindow::clearSongs() {
    DIR* dir;
    struct dirent* file_info;
    struct stat stat_info;

    char *mp3_filepath;
    char *mp3_filename = (char*) malloc(150);
    strcpy(mp3_filename, "/Documents/MUSE");

    mp3_filepath = (char*) malloc(strlen(getenv("HOME")) + strlen(mp3_filename) + 200);
    strcpy(mp3_filepath, getenv("HOME"));
    strcat(mp3_filepath, mp3_filename);

    if((dir = opendir(mp3_filepath)) != NULL){
        while((file_info = readdir(dir)) != NULL){
            lstat(file_info->d_name, &stat_info);
            if(strcmp((file_info->d_name + (strlen(file_info->d_name) - 4)), ".mp3") == 0) {
                char* rm_file = (char*) malloc(strlen(getenv("HOME")) + strlen(mp3_filename) + 200);
                strcpy(rm_file, mp3_filepath);
                strcat(rm_file, "/");
                strcat(rm_file, file_info->d_name);

                remove(rm_file);

                free(rm_file);
            }
        }
    }
    closedir(dir);

    free(mp3_filename);
    free(mp3_filepath);
}

int MuseWindow::downloadSong(char* song_path, int song_id) {

    int download = song_id;

    // see if song is already downloaded
    int found_dupl = -1;
    for( unsigned int i = 0; i < downloaded.size(); i++ ) {
        if( download == downloaded.at(i) ) {
            found_dupl = i;
            break;
        }
    }

    // song is not on disk
    if( found_dupl == -1 ) {
        downloaded.insert(downloaded.begin(), download);
        // download the song
        if( getSong(download, song_path) ) {
            qDebug() << "Error downloading file!" << endl;
            return 1;
        }

        // is downloaded too big?
        if( downloaded.size() > 5 ) {
            int delete_song = downloaded.back();
            downloaded.pop_back();

            char *del_song_path;
            char *del_song_name = (char*) malloc(100);
            sprintf(del_song_name, "/Documents/MUSE/muse_download_%d.mp3", delete_song);

            del_song_path = (char*) malloc(strlen(getenv("HOME")) + strlen(del_song_name) + 1); // to account for NULL terminator
            strcpy(del_song_path, getenv("HOME"));
            strcat(del_song_path, del_song_name);

            if( remove(del_song_path) ) {
                qDebug() << "Error deleting old file!" << endl;
            } else {
                qDebug() << "Song with id: " << delete_song << " deleted." << endl;
            }

            free(del_song_name);
            free(del_song_path);
        }


    } else { // song found, move to front of queue
        downloaded.erase(downloaded.begin() + found_dupl);
        downloaded.insert(downloaded.begin(), download);
    }

    return 0;
}

void MuseWindow::changePlayState(PlayState state) {
    play_state = state;

    if( state == NOT_PLAYING ) {
        stopAndReadyUpFMOD();
        timer->stop();

        ui->songInfoLabel->setText("");
        ui->songProgressLabel->setText("-:--");
        ui->songLengthLabel->setText("-:--");

        ui->songProgressSlider->setValue(0);
        ui->playButton->setText("Play");
    } else if( state == STARTED ) {
            char *new_song_path;
            char *new_song_name = (char*) malloc(100);
            sprintf(new_song_name, "/Documents/MUSE/muse_download_%d.mp3", queue.front().song_id);

            new_song_path = (char*) malloc(strlen(getenv("HOME")) + strlen(new_song_name) + 1); // to account for NULL terminator
            strcpy(new_song_path, getenv("HOME"));
            strcat(new_song_path, new_song_name);

        if( downloadSong(new_song_path, queue.front().song_id) ) { // download the next song
            qDebug() << "Unable to retrieve song" << endl;
            changePlayState(NOT_PLAYING);
        } else {
            // try playing a song
            FMOD_RESULT result = system->createStream(new_song_path, FMOD_CREATESTREAM, NULL, &song_to_play);
            if( result == FMOD_RESULT::FMOD_OK ) {
                unsigned int song_length;
                song_to_play->getLength(&song_length, FMOD_TIMEUNIT_MS);

                unsigned int song_length_s = song_length / 1000;

                memset(songLengthText, 0, 10);
                sprintf(songLengthText, "%d:%02d", song_length_s / 60, song_length_s % 60);
                ui->songLengthLabel->setText(songLengthText);
                ui->songProgressLabel->setText("0:00");

                ui->songProgressSlider->setMinimum(0);
                ui->songProgressSlider->setMaximum(song_length);
                ui->songProgressSlider->setValue(0);

                system->playSound(song_to_play, NULL, false, &song_channel);

                // set information up
                ui->songInfoLabel->setText(queue.front().info);
                ui->playButton->setText("Pause");

                // start the update timer
                timer->start(75);
            } else {
                changePlayState(NOT_PLAYING);
            }
        }

        free(new_song_name);
        free(new_song_path);

    } else if( state == RESUMED ) {
        song_channel->setPaused(false);
        ui->playButton->setText("Pause");
        timer->start(75);
    } else if( state == PAUSED ) {
        song_channel->setPaused(true);
        ui->playButton->setText("Play");
        timer->stop();
    }
}

void MuseWindow::changeHistoryState(HistoryState state) {
    history_state = state;

    if( state == NO_HISTORY ) {
        history.clear();
    } else if( state == HAS_HISTORY ) {
        if( !history.empty() && history.front().song_id != queue.front().song_id ) {
            history.insert(history.begin(), queue.front());
        } else if( history.empty() ) {
            history.insert(history.begin(), queue.front());
        }
    }
}

void MuseWindow::changeQueueState(QueueState state, const QModelIndex* index, bool shuffle_all) {
    queue_state = state;

    if( state == NO_QUEUE ) {
        queue.clear();
    } else if( state == HAS_QUEUE ) {
        if( index != NULL ) {
            // insert the songs in order
            for( int i = index->row(); i < song_model->rowCount(); i++ ) {
                struct songinfo info;
                info.song_id = song_model->data(index->siblingAtRow(i), Qt::UserRole).value<int>();
                info.info = song_model->data(index->sibling(i, 1)).value<QString>() + " - " + song_model->data(index->sibling(i, 2)).value<QString>();
                queue.push_back(info);
            }

            // shuffle the queue
            if( shuffle_state == SHUFFLE && shuffle_all ) {
                std::random_shuffle( queue.begin(), queue.end() );
            } else if( shuffle_state == SHUFFLE ) {
                std::random_shuffle( queue.begin()+1, queue.end() );
            }

            if( queue.size() == 1 ) {
                changeQueueState(END_OF_QUEUE);
            }
        }
    } else if( state == END_OF_QUEUE ) {

    }
}

void MuseWindow::changeConnectionState(ConnectionState state) {
    connection_state = state;

    if( state == CONNECTED ) {
        memset(connectionText, 0, 100);
        sprintf(connectionText, "Connected to: %s:%s", ip_address.toStdString().c_str(), port.toStdString().c_str());
        ui->serverInfoLabel->setText(connectionText);
        ui->connectButton->setText("Disconnect");

        on_tabWidget_tabBarClicked(ui->tabWidget->currentIndex());
    } else if( state == NOT_CONNECTED ) {
        disconnect();
        clearModels();
        clearSongs();
        free_playlist(playlists);
        ui->serverInfoLabel->setText("Not connected to server.");
        ui->connectButton->setText("Connect to...");
    }
}

void MuseWindow::changeRepeatState(RepeatState state) {
    repeat_state = state;

    if( state == NO_REPEAT ) {
        ui->repeatButton->setText("No Repeat");
        ui->repeatButton->setChecked(false);
    } else if( state == REPEAT ) {
        ui->repeatButton->setText("Repeat");
        ui->repeatButton->setChecked(true);
    } else if( state == REPEAT_ONE ) {
        ui->repeatButton->setText("Repeat One");
        ui->repeatButton->setChecked(true);
    }
}

void MuseWindow::changeShuffleState(ShuffleState state) {
    shuffle_state = state;

    if( state == NO_SHUFFLE ) {
        ui->shuffleButton->setText("No Shuffle");
        ui->shuffleButton->setChecked(false);

        if( queue_state == HAS_QUEUE ) {
            // find the song that is currently playing
            QModelIndex index = song_model->index(0, 0);
            QModelIndex current;
            for( int i = 0; i < song_model->rowCount(); i++ ) {
                if( index.siblingAtRow(i).data(Qt::UserRole).value<int>() == queue.front().song_id ) {
                    current = index.siblingAtRow(i);
                    break;
                }
            }

            changeQueueState(NO_QUEUE);
            changeQueueState(HAS_QUEUE, &current);
        }

    } else if( state == SHUFFLE ) {
        ui->shuffleButton->setText("Shuffle");
        ui->shuffleButton->setChecked(true);

        if( queue_state == HAS_QUEUE || queue_state == END_OF_QUEUE ) {
            // find the song that is currently playing
            QModelIndex index = song_model->index(0, 0);

            // we need to keep the current song at the front of the queue, but shuffle the rest behind it
            songinfo temp = queue.front();
            changeQueueState(NO_QUEUE);
            queue.push_back(temp);
            changeQueueState(HAS_QUEUE, &index, false);
        }
    }
}

void MuseWindow::on_songView_customContextMenuRequested(const QPoint &pos)
{
    QModelIndex index = ui->songView->indexAt(pos);
    int song_id = index.data(Qt::UserRole).value<int>();

    ui->songView->setCurrentIndex(index);

    if( song_id <= 0 ) { // not an actual song selected
        return;
    }

    QAction* addToPlaylistAction = new QAction("Add song to playlist...", this);
    addToPlaylistAction->setData(song_id);
    connect(addToPlaylistAction, &QAction::triggered, this, &MuseWindow::on_songView_addSongToPlaylist);

    QMenu* menu = new QMenu(this);
    menu->addAction(addToPlaylistAction);
    menu->popup(ui->songView->viewport()->mapToGlobal(pos));
}

void MuseWindow::on_songView_addSongToPlaylist() {
    QAction *act = qobject_cast<QAction *>(sender());
    QVariant v = act->data();
    int song_id = v.value<int>();

    PlaylistDialog* playlistDialog = new PlaylistDialog(this);
    if( playlistDialog->exec() == QDialog::Accepted ) {
        qDebug() << "Accepted!" << endl;
        struct playlist* selected = playlistDialog->getSelected();
        if( selected != NULL ) {
            addSongToPlaylist(song_id, selected);

            char *new_playlist_path;
            char *new_playlist_name = (char*) malloc(100);
            sprintf(new_playlist_name, "/Documents/MUSE/%s.pl", selected->name);

            new_playlist_path = (char*) malloc(strlen(getenv("HOME")) + strlen(new_playlist_name) + 1); // to account for NULL terminator
            strcpy(new_playlist_path, getenv("HOME"));
            strcat(new_playlist_path, new_playlist_name);
            savePlaylist(selected, new_playlist_path);

            free(new_playlist_name);
            free(new_playlist_path);

            free_playlist(playlistDialog->getPlaylists());
        }
    } else {
        qDebug() << "Declined." << endl;
    }
}
