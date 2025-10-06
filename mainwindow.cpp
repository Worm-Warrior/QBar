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
    QMediaPlayer *player = new QMediaPlayer(this);
    QAudioOutput *audioOut = new QAudioOutput(this);
    player->setAudioOutput(audioOut);

    ui->PlayerControls->setPlayer(player);
    audioOut->setVolume(0.5);

    connect(ui->FileBrowser, &FileBrowserWidget::folderSelected,
            ui->MainView, &MediaViewWidget::displayFolder);

    connect(ui->MainView, &MediaViewWidget::fileDoubleClicked,
            ui->PlayerControls, &PlayerControlsWidget::setCurMusic);

    connect(ui->PlayerControls, &PlayerControlsWidget::nextClicked, this, &MainWindow::playNextTrack);
    connect(ui->PlayerControls, &PlayerControlsWidget::prevClicked, this, &MainWindow::playPrevTrack);
    connect(ui->PlayerControls->player, &QMediaPlayer::mediaStatusChanged, this, &MainWindow::loopTracks);

    connect(ui->PlayerControls->player, &QMediaPlayer::positionChanged, ui->PlayerControls, &PlayerControlsWidget::on_positionChanged);
    connect(ui->PlayerControls->player, &QMediaPlayer::durationChanged, ui->PlayerControls, &PlayerControlsWidget::on_durationChanged);


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
