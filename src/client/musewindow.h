#ifndef MUSEWINDOW_H
#define MUSEWINDOW_H

#include <QMainWindow>
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QSortFilterProxyModel>
#include <QTableView>
#include <QTimer>
#include <fmod.hpp>
#include <fmod_errors.h>
#include <fmod_common.h>


QT_BEGIN_NAMESPACE
namespace Ui { class MuseWindow; }
QT_END_NAMESPACE


class MuseWindow : public QMainWindow
{
    Q_OBJECT

public:
    MuseWindow(QWidget *parent = nullptr);
    ~MuseWindow();

private slots:
    void on_tabWidget_currentChanged(int index);
    void on_playButton_clicked();
    void on_songView_doubleClicked(const QModelIndex &index);
    void on_rewindButton_clicked();

    void on_timeout();

private:
    Ui::MuseWindow *ui;
    bool play_state = false;

    QTimer *timer;

    FMOD_RESULT result;
    FMOD::System *system = NULL;
    FMOD::Sound* song_to_play;
    FMOD::Channel* song_channel;

    void configureTableView(QTableView* view);
    void initializeFMOD();
};
#endif // MUSEWINDOW_H
