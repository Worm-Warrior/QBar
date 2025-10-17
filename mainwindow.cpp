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
    connect(ui->chooseRoot, &QAction::triggered, this, &MainWindow::changeRoot);
    connect(ui->actionAbout, &QAction::triggered, this, &MainWindow::actionAbout);
    connect(ui->actionExit, &QAction::triggered, this, &MainWindow::actionExit);
    connect(ui->actionRemoteModeSwitch, &QAction::triggered, this, &MainWindow::remoteModeToggle);
    QMediaPlayer *player = new QMediaPlayer(this);
    QAudioOutput *audioOut = new QAudioOutput(this);
    player->setAudioOutput(audioOut);

    // Hard code these for now.
    username = "admin";
    password = "rat";

    ui->PlayerControls->setPlayer(player);
    audioOut->setVolume(0.5);


    ui->browserStack->setCurrentIndex(0);
    ui->viewStack->setCurrentIndex(0);

    ui->RemoteBrowser->show();

    connect(ui->actionRemoteModeSwitch, &QAction::toggled, this, &MainWindow::remoteModeToggle);

    connect(ui->FileBrowser, &FileBrowserWidget::folderSelected,
            ui->MainView, &MediaViewWidget::displayFolder);

    connect(ui->MainView, &MediaViewWidget::fileDoubleClicked,
            ui->PlayerControls, &PlayerControlsWidget::setCurMusic);

    connect(ui->PlayerControls, &PlayerControlsWidget::nextClicked, this, &MainWindow::playNextTrack);
    connect(ui->PlayerControls, &PlayerControlsWidget::prevClicked, this, &MainWindow::playPrevTrack);
    connect(ui->PlayerControls->player, &QMediaPlayer::mediaStatusChanged, this, &MainWindow::loopTracks);

    connect(ui->PlayerControls->player, &QMediaPlayer::positionChanged, ui->PlayerControls, &PlayerControlsWidget::on_positionChanged);
    connect(ui->PlayerControls->player, &QMediaPlayer::durationChanged, ui->PlayerControls, &PlayerControlsWidget::on_durationChanged);

    connect(ui->PlayerControls->player, &QMediaPlayer::metaDataChanged,
            ui->PlayerControls, &PlayerControlsWidget::updateInfoLabels);


    QString streamUrl = QString("http://192.168.4.165:4533/rest/stream.view?id=%1&u=%2&p=%3&v=1.16.1&c=QtPlayer")
                            .arg("bf6add46d366f6b30734bb22a741459d")
                            .arg(username)
                            .arg(password);

    //ui->PlayerControls->player->setSource(QUrl(streamUrl));
    //ui->PlayerControls->player->play();

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
    QString next = ui->MainView->getNextFile();
    if (!next.isEmpty()) {
        ui->PlayerControls->setCurMusic(next);
    }

}

void MainWindow::playPrevTrack()
{
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
