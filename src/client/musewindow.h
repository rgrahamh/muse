#ifndef MUSEWINDOW_H
#define MUSEWINDOW_H

#include <QMainWindow>
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QSortFilterProxyModel>
#include <QTableView>


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

private:
    Ui::MuseWindow *ui;
    bool play_state = false;

    void configureTableView(QTableView* view);
};
#endif // MUSEWINDOW_H
