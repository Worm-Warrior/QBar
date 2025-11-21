#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMediaPlayer>
#include "playlist.h"

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

    // Server credentials
    QString username;
    QString password;
    QString serverUrl;

    // Public method for views to create and play a new playlist
    void playNewPlaylist(const QList<Track> &tracks, int startIndex = 0);

private slots:
    // Menu actions
    void changeRoot();
    void actionAbout();
    void actionExit();
    void remoteModeToggle();
    void ServerSettings();

    // Playlist navigation (called by PlayerControls)
    void onNextRequested();
    void onPreviousRequested();
    void onMediaStatusChanged(QMediaPlayer::MediaStatus status);

private:
    Ui::MainWindow *ui;
    Playlist *currentPlaylist;
    // Helper methods
    void playTrack(const Track &track);
};

#endif // MAINWINDOW_H
