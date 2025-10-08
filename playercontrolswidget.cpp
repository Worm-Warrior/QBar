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


    //ui->nextButton->setLayoutDirection(Qt::RightToLeft);
    ui->nextButton->setIcon(QIcon("../../icons/skip-next.png"));
    ui->prevButton->setIcon(QIcon("../../icons/skip-previous.png"));
    ui->PlayPause->setIcon(QIcon("../../icons/play-circle.png"));
    ui->prevButton->setText("");
    ui->nextButton->setText("");

    ui->Mute->setIcon(QIcon("../../icons/unmuted.png"));
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
        ui->PlayPause->setIcon(QIcon("../../icons/play-circle.png"));
    } else {
        player->play();
        ui->PlayPause->setText("Pause");
        ui->PlayPause->setIcon(QIcon("../../icons/pause-circle.png"));
    }
}


void PlayerControlsWidget::on_Mute_clicked()
{
    if (!player) return;

    if (player->audioOutput()->isMuted()) {
        player->audioOutput()->setMuted(false);
        ui->Mute->setText("Mute");
        ui->Mute->setIcon(QIcon("../../icons/unmuted.png"));
    } else {
        player->audioOutput()->setMuted(true);
        ui->Mute->setText("Unmute");
        ui->Mute->setIcon(QIcon("../../icons/muted.png"));
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

void PlayerControlsWidget::on_durationChanged(qint64 duration)
{
    ui->seekBar->setMaximum(duration);

    QTime totalTime(0, 0);
    totalTime = totalTime.addMSecs(duration);
    ui->maxDuration->setText(totalTime.toString("mm:ss"));
}

void PlayerControlsWidget::on_positionChanged(qint64 position)
{
    if (userIsSeeking) {
        //TODO: make it so that the user can see the time update in real time?
        return;
    }

    blockSignals(true);
    ui->seekBar->setValue(position);
    blockSignals(false);

    QTime currentTime(0, 0);
    currentTime = currentTime.addMSecs(position);
    ui->curTime->setText(currentTime.toString("mm:ss"));
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

