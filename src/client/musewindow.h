#ifndef MUSEWINDOW_H
#define MUSEWINDOW_H

#include <QMainWindow>
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QSortFilterProxyModel>
#include <QTableView>
#include <QTimer>
#include <QThread>
#include <QMenu>
#include <fmod.hpp>
#include <fmod_errors.h>
#include <fmod_common.h>
#include <algorithm>
#include <dirent.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "songmodel.h"
#include "artistmodel.h"
#include "albummodel.h"
#include "genremodel.h"
#include "serverdialog.h"
#include "addtoplaylistdialog.h"
#include "removefromplaylistdialog.h"
#include "testdata.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MuseWindow; }
QT_END_NAMESPACE

struct songinfo {
    QString info;
    int song_id;
};

enum PlayState {
    NOT_PLAYING,
    STARTED,
    RESUMED,
    PAUSED
};

enum HistoryState {
    NO_HISTORY,
    HAS_HISTORY
};

enum QueueState {
    NO_QUEUE,
    HAS_QUEUE,
    END_OF_QUEUE
};

enum RepeatState {
    NO_REPEAT,
    REPEAT,
    REPEAT_ONE
};

enum ShuffleState {
    NO_SHUFFLE,
    SHUFFLE
};

enum ConnectionState {
    NOT_CONNECTED,
    CONNECTED
};

class SongBurstThread;
class DownloadThread;
class AlbumBurstThread;
class ArtistBurstThread;

class MuseWindow : public QMainWindow
{
    Q_OBJECT


public:
    MuseWindow(QWidget *parent = nullptr);
    ~MuseWindow();

    //This also has to be public in order to call it from the DownloadWorker
    int downloadSong(char* song_path, int song_id);

    ArtistModel* artist_model;
    AlbumModel* album_model;
    GenreModel* genre_model;
    SongModel* song_model;

private slots:
    void on_tabWidget_tabBarClicked(int index);

    void on_songView_doubleClicked(const QModelIndex &index);
    void on_artistView_doubleClicked(const QModelIndex &index);
    void on_albumView_doubleClicked(const QModelIndex &index);
    void on_genreView_doubleClicked(const QModelIndex &index);
    void on_playlistView_doubleClicked(const QModelIndex &index);

    void on_playButton_clicked();
    void on_rewindButton_clicked();
    void on_skipButton_clicked();
    void on_repeatButton_clicked();
    void on_shuffleButton_clicked();
    void on_connectButton_clicked();

    void on_timeout();

    void on_songView_customContextMenuRequested(const QPoint &pos);
    void on_playlistView_customContextMenuRequested(const QPoint &pos);

    void on_songView_addSongToPlaylist();
    void on_playlistView_removeSongsFromPlaylist();
    void on_playlistView_deletePlaylist();

private:
    Ui::MuseWindow *ui;
    PlayState play_state = NOT_PLAYING;
    HistoryState history_state = NO_HISTORY;
    QueueState queue_state = NO_QUEUE;
    ConnectionState connection_state = NOT_CONNECTED;
    RepeatState repeat_state = NO_REPEAT;
    ShuffleState shuffle_state = NO_SHUFFLE;

    DownloadThread* dlthread = NULL;
    SongBurstThread* sbthread = NULL;
    AlbumBurstThread* albthread = NULL;
    ArtistBurstThread* arbthread = NULL;

    QString ip_address;
    QString port;

    std::vector<struct songinfo> queue;
    std::vector<struct songinfo> history;
    std::vector<int> downloaded;

    char* songProgressText;
    char* songLengthText;
    char* connectionText;

    pid_t download_pid = 0;

    QTimer *timer;

    //If the download is finished, it's set.
    int download_fin = 1;

    struct playlist* playlists = NULL;

    FMOD_RESULT result;
    FMOD::System *system = NULL;
    FMOD::Sound* song_to_play = NULL;
    FMOD::Channel* song_channel = NULL;

    PlaylistModel* playlist_model;

    void configureTableView(QTableView* view);
    void initializeFMOD();
    void clearModels();
    void stopAndReadyUpFMOD();
    void clearSongs();
    void deleteSong(int song_id);

    void changePlayState(PlayState state);
    void changeHistoryState(HistoryState state);
    void changeQueueState(QueueState state, const QModelIndex* index = NULL, bool shuffle_all = true);
    void changeConnectionState(ConnectionState state);
    void changeRepeatState(RepeatState state);
    void changeShuffleState(ShuffleState state);
};

class SongBurstThread: public QThread{
    Q_OBJECT
    void run();

    int start_id = 0;
    int end_id = 25;
    int iter = 25;

    MuseWindow* window;

public:
    SongBurstThread(MuseWindow* window, int iter);
    void reset();
};

class AlbumBurstThread: public QThread{
    Q_OBJECT
    void run();

    int start_id = 0;
    int end_id = 25;
    int iter = 25;

    MuseWindow* window;

public:
    AlbumBurstThread(MuseWindow* window, int iter);
    void reset();
};

class ArtistBurstThread: public QThread{
    Q_OBJECT
    void run();

    int start_id = 0;
    int end_id = 25;
    int iter = 25;

    MuseWindow* window;

public:
    ArtistBurstThread(MuseWindow* window, int iter);
    void reset();
};

class DownloadThread: public QThread{
    Q_OBJECT
    void run();

    int song_id = 0;
    MuseWindow* window = NULL;

public:
    DownloadThread(MuseWindow* window);
    void setSongID(int song_id);
};

#endif // MUSEWINDOW_H
