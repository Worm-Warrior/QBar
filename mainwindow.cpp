#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QtMultimedia/QMediaPlayer>
#include <QtMultimedia/QAudioOutput>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Hard code credentials for now
    username = "admin";
    password = "rat";
    serverUrl = "http://192.168.4.165:4533";

    // Setup player
    QMediaPlayer *player = new QMediaPlayer(this);
    QAudioOutput *audioOut = new QAudioOutput(this);
    player->setAudioOutput(audioOut);
    audioOut->setVolume(0.5);
    ui->PlayerControls->setPlayer(player);

    // Create global playlist
    currentPlaylist = new Playlist(this);
    currentPlaylist->setShuffle(false);

    // Setup stacks (start in local mode)
    ui->browserStack->setCurrentIndex(0);
    ui->viewStack->setCurrentIndex(0);

    // Give views access to MainWindow so they can call playNewPlaylist()
    ui->RemoteView->setMainWindow(this);
    ui->MainView->setMainWindow(this);

    // === MENU ACTIONS ===
    connect(ui->chooseRoot, &QAction::triggered,
            this, &MainWindow::changeRoot);
    connect(ui->actionAbout, &QAction::triggered,
            this, &MainWindow::actionAbout);
    connect(ui->actionExit, &QAction::triggered,
            this, &MainWindow::actionExit);
    connect(ui->actionRemoteModeSwitch, &QAction::triggered,
            this, &MainWindow::remoteModeToggle);

    // === LOCAL MODE ===
    connect(ui->FileBrowser, &FileBrowserWidget::folderSelected,
            ui->MainView, &MediaViewWidget::displayFolder);

    // === REMOTE MODE ===
    connect(ui->RemoteBrowser, &RemoteFileBrowser::albumSelected,
            ui->RemoteView, &RemoteMediaView::fetchAlbum);

    // === PLAYER CONTROLS (routed through MainWindow/Playlist) ===
    connect(ui->PlayerControls, &PlayerControlsWidget::nextClicked,
            this, &MainWindow::onNextRequested);
    connect(ui->PlayerControls, &PlayerControlsWidget::prevClicked,
            this, &MainWindow::onPreviousRequested);
    connect(ui->PlayerControls->player, &QMediaPlayer::mediaStatusChanged,
            this, &MainWindow::onMediaStatusChanged);

    // === UI UPDATES (direct to PlayerControls) ===
    connect(ui->PlayerControls->player, &QMediaPlayer::positionChanged,
            ui->PlayerControls, &PlayerControlsWidget::on_positionChanged);
    connect(ui->PlayerControls->player, &QMediaPlayer::durationChanged,
            ui->PlayerControls, &PlayerControlsWidget::on_durationChanged);
    connect(ui->PlayerControls->player, &QMediaPlayer::metaDataChanged,
            ui->PlayerControls, &PlayerControlsWidget::updateInfoLabels);
    connect(ui->PlayerControls, &PlayerControlsWidget::repeatChanged,
            this, [this](int mode){
        currentPlaylist->setRepeat(mode);
        qInfo() << mode;
    });
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::playNewPlaylist(const QList<Track> &tracks, int startIndex)
{
    if (tracks.isEmpty()) {
        qWarning() << "Attempted to play empty playlist";
        return;
    }

    // Replace current playlist
    currentPlaylist->clear();
    currentPlaylist->addTracks(tracks);
    currentPlaylist->setCurrentIndex(startIndex);

    // Start playing first track
    Track track = currentPlaylist->currentTrack();
    playTrack(track);
}

void MainWindow::playTrack(const Track &track)
{
    if (track.isRemote) {
        // Build streaming URL
        QString streamUrl = QString("%1/rest/stream.view?id=%2&u=%3&p=%4&v=1.16.1&c=QBar")
                                .arg(serverUrl)
                                .arg(track.id)
                                .arg(username)
                                .arg(password);
        ui->PlayerControls->player->setSource(QUrl(streamUrl));
    } else {
        // Local file
        ui->PlayerControls->player->setSource(QUrl::fromLocalFile(track.filePath));
    }

    // Update UI with track info (you may need to add this method to PlayerControlsWidget)
    // ui->PlayerControls->updateTrackInfo(track);

    ui->PlayerControls->player->play();
}

void MainWindow::onNextRequested()
{
    if (currentPlaylist->hasNext()) {
        Track nextTrack = currentPlaylist->nextTrack();
        playTrack(nextTrack);
    } else {
        ui->PlayerControls->player->stop();
    }
}

void MainWindow::onPreviousRequested()
{
    if (currentPlaylist->hasPrev()) {
        Track prevTrack = currentPlaylist->prevTrack();
        playTrack(prevTrack);
    }
}

void MainWindow::onMediaStatusChanged(QMediaPlayer::MediaStatus status)
{
    // When song ends, automatically play next track
    if (status == QMediaPlayer::EndOfMedia) {
        onNextRequested();
    }
}

void MainWindow::changeRoot()
{
    ui->FileBrowser->changeRoot();
}

void MainWindow::actionAbout()
{
    QMessageBox msgBox;
    msgBox.setText("QBar v0.1\n"
                   "Authored By Harrison Voss\n"
                   "Heavily inspired by FooBar2000");
    msgBox.setIcon(QMessageBox::Information);
    msgBox.exec();
}

void MainWindow::actionExit()
{
    QMessageBox msgBox;
    msgBox.setText("Do you want to exit?");
    msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Ok);
    int ret = msgBox.exec();

    if (ret == QMessageBox::Ok) {
        QApplication::quit();
    }
}

void MainWindow::remoteModeToggle()
{
    // Validate credentials before switching to remote mode
    if (username.isEmpty() || password.isEmpty()) {
        QMessageBox msgBox;
        msgBox.setText("Username or password is empty!");
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.exec();
        return;
    }

    // Ensure stacks are synchronized
    Q_ASSERT(ui->viewStack->currentIndex() == ui->browserStack->currentIndex());

    int currentMode = ui->browserStack->currentIndex();
    int newMode = (currentMode == 0) ? 1 : 0;

    ui->browserStack->setCurrentIndex(newMode);
    ui->viewStack->setCurrentIndex(newMode);

    // Update menu text
    ui->actionRemoteModeSwitch->setText(newMode == 0 ? "Remote Mode" : "Local Mode");
}
