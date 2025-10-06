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
    userIsSeeking = false;

    connect(ui->nextButton, &QPushButton::clicked,
            this, &PlayerControlsWidget::nextClicked);
    connect(ui->prevButton, &QPushButton::clicked,
            this, &PlayerControlsWidget::prevClicked);

    connect(player, &QMediaPlayer::positionChanged, this, &PlayerControlsWidget::on_positionChanged);
    connect(player, &QMediaPlayer::durationChanged, this, &PlayerControlsWidget::on_durationChanged);
    connect(ui->seekBar, &QSlider::sliderReleased, this, &PlayerControlsWidget::on_seekBar_sliderReleased);
    connect(ui->seekBar, &QSlider::sliderPressed,
            this, &PlayerControlsWidget::on_seekBar_sliderPressed);

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

    player->audioOutput()->setVolume(value/100);
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

void PlayerControlsWidget::on_durationChanged(qint64 duration)
{
    ui->seekBar->setMaximum(duration);
}

void PlayerControlsWidget::on_positionChanged(qint64 position)
{
    if (userIsSeeking) {return;}

    blockSignals(true);
    ui->seekBar->setValue(position);
    blockSignals(false);
}

void PlayerControlsWidget::on_seekBar_sliderMoved(int position)
{
    ui->seekBar->setValue(position);
}


void PlayerControlsWidget::on_seekBar_sliderReleased()
{
    userIsSeeking = false;
    player->setPosition(ui->seekBar->value());
}


void PlayerControlsWidget::on_seekBar_sliderPressed()
{
    userIsSeeking = true;
}

