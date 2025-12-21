#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "serverinputbox.h"
#include <QtMultimedia/QMediaPlayer>
#include <QtMultimedia/QAudioOutput>
#include <QMessageBox>
#include "appconfig.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // TODO: might need to make sure that a config exists, else make an empty one
    // because it might be UB if we don't zero it out beforehand.
    qInfo() << AppConfig::username() << AppConfig::password() << AppConfig::serverURL();

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
    connect(ui->actionServerSettings, &QAction::triggered,
            this, &MainWindow::ServerSettings);

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

    connect(ui->PlayerControls, &PlayerControlsWidget::shuffleChanged,
            this, [this](bool mode){
        currentPlaylist->setShuffle(mode);
        qInfo() << mode;
    });

    setWindowTitle("QBar Music Player");
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
                                .arg(AppConfig::serverURL())
                                .arg(track.id)
                                .arg(AppConfig::username())
                                .arg(AppConfig::password());
        ui->PlayerControls->player->setSource(QUrl(streamUrl));
    } else {
        // Local file
        ui->PlayerControls->player->setSource(QUrl::fromLocalFile(track.filePath));
    }

    ui->PlayerControls->player->play();

    emit newTrackPlayed(track);

    setWindowTitle(track.artist + " | [ " + track.album + " ] | " + track.title);
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
    if (ui->viewStack->currentIndex() == 0 && (AppConfig::username().isEmpty() || AppConfig::password().isEmpty())) {
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

void MainWindow::ServerSettings()
{
    serverinputbox input;

    if (input.exec() == QDialog::Accepted)
    {
        QString serverUrl = input.url();
        QString username = input.username();
        QString password = input.password();

        qInfo() << username << password << serverUrl;

        QSettings settings("Voss Software", "QBar");
        settings.beginGroup("Server");
        settings.setValue("username", username);
        settings.setValue("password", password);
        settings.setValue("server_url", serverUrl);
        settings.endGroup();

        qInfo() << AppConfig::username() << AppConfig::password() << AppConfig::serverURL();
    }
}

void MainWindow::updatePlaylist(const QList<Track> &tracks) {
    qInfo() << "updating playlist!!";

    // If shuffle is on, the UI will not show the ordering anyway, we don't care.
    if (currentPlaylist->isShuffleOn()) {
        return;
    }

    // If we called this, it means that we had no playlist before.
    // So we don't need to swap any indexes, just new list!
    if (currentPlaylist->currentIndex() == -1) {
        currentPlaylist->clear(); // IDK if this is needed, but we should just in case!
        currentPlaylist->addTracks(tracks);
        return;
    }


    // Otherwise, we need to translate the old playlist playing index to the new one.
    // This is so that when the user does next/prev it acts as expected!
    Track old = currentPlaylist->currentTrack();
    currentPlaylist->clear();
    currentPlaylist->addTracks(tracks);

    QList<Track> t = currentPlaylist->tracks();

    for (int i = 0; i < t.size(); ++i) {
        if (t[i].title == old.title) {
            currentPlaylist->setCurrentIndex(i);
            break;
        }
    }
}

void MainWindow::newTrackPlayed(const Track &track) {
    if (ui->viewStack->currentIndex() == 0 && ui->browserStack->currentIndex() == 0) {
        qInfo() << "new local track selected: " << track.title;
        ui->MainView->selectNewTrack(track);
    } else {
        ui->RemoteView->selectedNewTrack(track);
    }
}
