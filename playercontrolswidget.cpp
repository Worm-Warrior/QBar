#include "playercontrolswidget.h"
#include "ui_playercontrolswidget.h"

PlayerControlsWidget::PlayerControlsWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::PlayerControlsWidget)
{
    ui->setupUi(this);
    ui->Volume->setMaximum(100);
    ui->Volume->setMinimum(0);
    ui->Volume->setPageStep(5);
    ui->Volume->setValue(50);

    connect(ui->nextButton, &QPushButton::clicked,
            this, &PlayerControlsWidget::nextClicked);
    connect(ui->prevButton, &QPushButton::clicked,
            this, &PlayerControlsWidget::prevClicked);
}

PlayerControlsWidget::~PlayerControlsWidget()
{
    delete ui;
}

void PlayerControlsWidget::setPlayer(QMediaPlayer *p) {
    player = p;
}

void PlayerControlsWidget::on_PlayPause_clicked()
{
    if (!player) return;

    if (player->isPlaying()) {
        player->pause();
        ui->PlayPause->setText("Play");
    } else {
        player->play();
        ui->PlayPause->setText("Pause");
    }
}


void PlayerControlsWidget::on_Mute_clicked()
{
    if (!player) return;

    if (player->audioOutput()->isMuted()) {
        player->audioOutput()->setMuted(false);
        ui->Mute->setText("Mute");
    } else {
        player->audioOutput()->setMuted(true);
        ui->Mute->setText("Unmute");
    }
}

// NOTE: use this instead of the other functions, because this will change on ALL input methods.
void PlayerControlsWidget::on_Volume_valueChanged(int value)
{
    if (!player) return;

    player->audioOutput()->setVolume(value * 0.01);
}


void PlayerControlsWidget::setCurMusic(QString filePath) {
    if (!player) return;

    QUrl path = QUrl::fromLocalFile(filePath);

    if (path.isLocalFile() && path.isValid()) {
        player->setSource(path);
        on_PlayPause_clicked();
    }
    QDebug::toString(player->duration());

}

void PlayerControlsWidget::playNext(QString filePath)
{
    setCurMusic(filePath);
}

void PlayerControlsWidget::playPrev(QString filePath)
{
    setCurMusic(filePath);
}
