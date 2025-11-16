#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "playlist.h"
#include <QMediaPlayer>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    QString username;
    QString password;
    QString serverUrl;
    void playNewPlaylist(const QList<Track> &tracks, int startIndex = 0);
    void playTrack(const Track &track);
    void onNextRequested();
    void onPreviousRequested();
    void onMediaStatusChanged(QMediaPlayer::MediaStatus status);

private slots:
    void changeRoot();
    void actionAbout();
    void actionExit();
    void playNextTrack();
    void playPrevTrack();
    void loopTracks();
    void remoteModeToggle();

private:
    Ui::MainWindow *ui;
    Playlist *currentPlaylist;
};
#endif // MAINWINDOW_H
