#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QtMultimedia/QtMultimedia>
#include <QtMultimedia/QAudioOutput>
#include <QMessageBox>
#include <qpushbutton.h>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Menu actions
    connect(ui->chooseRoot, &QAction::triggered, this, &MainWindow::changeRoot);
    connect(ui->actionAbout, &QAction::triggered, this, &MainWindow::actionAbout);
    connect(ui->actionExit, &QAction::triggered, this, &MainWindow::actionExit);
    connect(ui->actionRemoteModeSwitch, &QAction::triggered, this, &MainWindow::remoteModeToggle);
    connect(ui->actionRemoteModeSwitch, &QAction::toggled, this, &MainWindow::remoteModeToggle);

    // Setup player
    QMediaPlayer *player = new QMediaPlayer(this);
    QAudioOutput *audioOut = new QAudioOutput(this);
    player->setAudioOutput(audioOut);
    audioOut->setVolume(0.5);
    ui->PlayerControls->setPlayer(player);

    // Create global playlist
    currentPlaylist = new Playlist(this);

    // Hard code credentials for now
    username = "admin";
    password = "rat";

    // Setup stacks
    ui->browserStack->setCurrentIndex(0);
    ui->viewStack->setCurrentIndex(0);

    // Give views access to MainWindow
    ui->RemoteView->setMainWindow(this);
    ui->MainView->setMainWindow(this);  // You'll need to add this method to MediaViewWidget

    // LOCAL mode connections
    connect(ui->FileBrowser, &FileBrowserWidget::folderSelected,
            ui->MainView, &MediaViewWidget::displayFolder);
    // Remove old direct connection to PlayerControls:
    // connect(ui->MainView, &MediaViewWidget::fileDoubleClicked,
    //         ui->PlayerControls, &PlayerControlsWidget::setCurMusic);

    // REMOTE mode connections
    connect(ui->RemoteBrowser, &RemoteFileBrowser::albumSelected,
            ui->RemoteView, &RemoteMediaView::fetchAlbum);
    // Remove old direct connection:
    // connect(ui->RemoteView, &RemoteMediaView::songSelected,
    //         ui->PlayerControls, &PlayerControlsWidget::playRemoteMusic);

    // PLAYER CONTROLS connections - route through MainWindow
    connect(ui->PlayerControls, &PlayerControlsWidget::nextClicked,
            this, &MainWindow::onNextRequested);
    connect(ui->PlayerControls, &PlayerControlsWidget::prevClicked,
            this, &MainWindow::onPreviousRequested);
    connect(ui->PlayerControls->player, &QMediaPlayer::mediaStatusChanged,
            this, &MainWindow::onMediaStatusChanged);

    // UI update connections (these can stay direct)
    connect(ui->PlayerControls->player, &QMediaPlayer::positionChanged,
            ui->PlayerControls, &PlayerControlsWidget::on_positionChanged);
    connect(ui->PlayerControls->player, &QMediaPlayer::durationChanged,
            ui->PlayerControls, &PlayerControlsWidget::on_durationChanged);
    connect(ui->PlayerControls->player, &QMediaPlayer::metaDataChanged,
            ui->PlayerControls, &PlayerControlsWidget::updateInfoLabels);

    qInfo() << "Current index:" << ui->browserStack->currentIndex();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::changeRoot() {
    ui->FileBrowser->changeRoot();
}

void MainWindow::actionAbout() {
    QMessageBox msgBox;
    msgBox.setText("QBar v0.1\n"
                   "Authored By Harrison Voss\n"
                   "Heavily inspired by FooBar200");
    msgBox.setIcon(QMessageBox::Information);
    msgBox.exec();
}


void MainWindow::actionExit() {
    QMessageBox msgBox;
    msgBox.setText("Do you want to exit?");
    msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Ok);
    int ret = msgBox.exec();

    if (ret == QMessageBox::Ok) {
        exit(0);
    } else {
        return;
    }
}

void MainWindow::playNextTrack()
{
    if (ui->PlayerControls->shouldRepeat) {
        ui->PlayerControls->player->setPosition(0);
        return;
    }
    QString next = ui->MainView->getNextFile();
    if (!next.isEmpty()) {
        ui->PlayerControls->setCurMusic(next);
    }

}

void MainWindow::playPrevTrack()
{
    if (ui->PlayerControls->shouldRepeat) {
        ui->PlayerControls->player->setPosition(0);
        return;
    }

    QString prev = ui->MainView->getPrevFile();
    if (!prev.isEmpty()) {
        ui->PlayerControls->setCurMusic(prev);
    }
}

void MainWindow::loopTracks()
{
    if (ui->PlayerControls->player->mediaStatus() == QMediaPlayer::EndOfMedia)
    {
        playNextTrack();
    }
}

void MainWindow::remoteModeToggle() {

    // This will need to be more robust later, but for now this will do.
    if (username.isEmpty() || password.isEmpty()) {
        QMessageBox msgBox;
        msgBox.setText("Your username or password is empty!");
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.exec();

        return;
    }

    // This should never happen, but just in case!
    Q_ASSERT(ui->viewStack->currentIndex()
             == ui->browserStack->currentIndex());
    // At this point they are both the same so it does not matter.
    int currentMode = ui->browserStack->currentIndex();

    int newMode = (currentMode == 0) ? 1 : 0;

    ui->browserStack->setCurrentIndex(newMode);
    ui->viewStack->setCurrentIndex(newMode);

    ui->actionRemoteModeSwitch->setText(newMode == 0 ? "Remote Mode" : "Local Mode");
}

void MainWindow::playNewPlaylist(const QList<Track> &tracks, int startIndex) {
    currentPlaylist->clear();
    currentPlaylist->addTracks(tracks);
    currentPlaylist->setCurrentIndex(startIndex);

    Track track = currentPlaylist->currentTrack();
    playTrack(track);
}

void MainWindow::playTrack(const Track &track) {
    if (track.id.isEmpty() && track.filePath.isEmpty()) {
        return;
    }

    if (track.isRemote) {
        QString streamUrl = QString("http://192.168.4.165:4533/rest/stream.view?"
                                    "id=%1&u=%2&p=%3&v=1.16.1&c=QBar")
                                .arg(track.id)
                                .arg(username)
                                .arg(password);
        ui->PlayerControls->player->setSource(QUrl(streamUrl));
    } else {
        ui->PlayerControls->player->setSource(QUrl::fromLocalFile(track.filePath));
    }

    // Update player controls UI
    // ui->PlayerControls->updateTrackInfo(track);
    ui->PlayerControls->player->play();
}

void MainWindow::onNextRequested() {
    if (currentPlaylist->hasNext()) {
        Track nextTrack = currentPlaylist->nextTrack();
        playTrack(nextTrack);
    } else {
        ui->PlayerControls->player->stop();
    }
}

void MainWindow::onPreviousRequested() {
    if (currentPlaylist->hasPrev()) {
        Track prevTrack = currentPlaylist->prevTrack();
        playTrack(prevTrack);
    }
}

void MainWindow::onMediaStatusChanged(QMediaPlayer::MediaStatus status) {
    // When song ends, play next automatically
    if (status == QMediaPlayer::EndOfMedia) {
        onNextRequested();
    }
}
