#include "playercontrolswidget.h"
#include "playlist.h"
#include "ui_playercontrolswidget.h"
#include <QTime>
#include <qaudiooutput.h>
#include <qmediametadata.h>

PlayerControlsWidget::PlayerControlsWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::PlayerControlsWidget)
    , player(nullptr)
    , userIsSeeking(false)
{
    ui->setupUi(this);

    // Setup volume slider
    ui->Volume->setMaximum(100);
    ui->Volume->setMinimum(0);
    ui->Volume->setPageStep(5);
    ui->Volume->setValue(50);

    // Setup icons
    ui->nextButton->setIcon(QIcon("../../icons/skip-next.png"));
    ui->prevButton->setIcon(QIcon("../../icons/skip-previous.png"));
    ui->PlayPause->setIcon(QIcon("../../icons/play-circle.png"));
    ui->Mute->setIcon(QIcon("../../icons/unmuted.png"));

    // Clear button text (we're using icons)
    ui->prevButton->setText("");
    ui->nextButton->setText("");

    // Setup track info labels
    ui->trackTitle->setText("");
    ui->trackTitle->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    ui->trackTitle->setTextInteractionFlags(Qt::NoTextInteraction);
    ui->trackTitle->setWordWrap(false);
    ui->trackTitle->setFixedWidth(200);

    ui->trackAlbum->setText("");
    ui->trackAlbum->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    ui->trackAlbum->setTextInteractionFlags(Qt::NoTextInteraction);
    ui->trackAlbum->setWordWrap(false);
    ui->trackAlbum->setFixedWidth(200);

    // Connect button signals
    connect(ui->nextButton, &QPushButton::clicked,
            this, &PlayerControlsWidget::nextClicked);
    connect(ui->prevButton, &QPushButton::clicked,
            this, &PlayerControlsWidget::prevClicked);

    // Connect seek bar signals
    connect(ui->seekBar, &QSlider::sliderPressed,
            this, &PlayerControlsWidget::on_seekBar_sliderPressed);
    connect(ui->seekBar, &QSlider::sliderReleased,
            this, &PlayerControlsWidget::on_seekBar_sliderReleased);
    connect(ui->seekBar, &QSlider::sliderMoved,
            this, &PlayerControlsWidget::on_seekBar_sliderMoved);
}

PlayerControlsWidget::~PlayerControlsWidget()
{
    delete ui;
}

void PlayerControlsWidget::setPlayer(QMediaPlayer *p)
{
    if (player) {
        disconnect(player, &QMediaPlayer::playbackStateChanged,
                   this, &PlayerControlsWidget::onPlaybackStateChanged);
    }
    player = p;

    if (player) {
        connect(player, &QMediaPlayer::playbackStateChanged,
                this, &PlayerControlsWidget::onPlaybackStateChanged);
    }
}

void PlayerControlsWidget::on_PlayPause_clicked()
{
    if (!player) {
        qWarning() << "Player not set!";
        return;
    }

    qInfo() << "Button clicked! Current state:" << player->playbackState();

    // Block signals temporarily so state change doesn't trigger during this function

    if (player->playbackState() == QMediaPlayer::PlayingState) {
        qInfo() << "Calling pause()";
        player->pause();
        ui->PlayPause->setText("Play");
        ui->PlayPause->setIcon(QIcon("../../icons/play-circle.png"));
    } else {
        qInfo() << "Calling play()";
        player->play();
        ui->PlayPause->setText("Pause");
        ui->PlayPause->setIcon(QIcon("../../icons/pause-circle.png"));
    }

}

void PlayerControlsWidget::on_Mute_clicked()
{
    if (!player || !player->audioOutput()) {
        qWarning() << "Player or audio output not set!";
        return;
    }

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

void PlayerControlsWidget::on_Volume_valueChanged(int value)
{
    if (!player || !player->audioOutput()) {
        return;
    }

    player->audioOutput()->setVolume(value * 0.01);
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
        return;
    }

    // Update seek bar without triggering signals
    ui->seekBar->blockSignals(true);
    ui->seekBar->setValue(position);
    ui->seekBar->blockSignals(false);

    // Update time display
    QTime currentTime(0, 0);
    currentTime = currentTime.addMSecs(position);
    ui->curTime->setText(currentTime.toString("mm:ss"));
}

void PlayerControlsWidget::on_seekBar_sliderMoved(int position)
{
    // Update the slider value as user drags
    ui->seekBar->setValue(position);

    // Optionally show the time at this position
    QTime seekTime(0, 0);
    seekTime = seekTime.addMSecs(position);
    ui->curTime->setText(seekTime.toString("mm:ss"));
}

void PlayerControlsWidget::on_seekBar_sliderPressed()
{
    userIsSeeking = true;
}

void PlayerControlsWidget::on_seekBar_sliderReleased()
{
    userIsSeeking = false;

    if (player) {
        player->setPosition(ui->seekBar->value());
    }
}

void PlayerControlsWidget::updateInfoLabels()
{
    if (!player) {
        return;
    }

    QString trackTitle = player->metaData().stringValue(QMediaMetaData::Title);
    QString album = player->metaData().stringValue(QMediaMetaData::AlbumTitle);

    // Fallback to "Unknown" if metadata is empty
    if (trackTitle.isEmpty()) {
        trackTitle = "Unknown Track";
    }
    if (album.isEmpty()) {
        album = "Unknown Album";
    }

    // Elide text if it's too long
    QFontMetrics fmTitle(ui->trackTitle->font());
    QFontMetrics fmAlbum(ui->trackAlbum->font());
    QString elidedTitle = fmTitle.elidedText(trackTitle, Qt::ElideRight, ui->trackTitle->width());
    QString elidedAlbum = fmAlbum.elidedText(album, Qt::ElideRight, ui->trackAlbum->width());

    ui->trackTitle->setText(elidedTitle);
    ui->trackAlbum->setText(elidedAlbum);
}

void PlayerControlsWidget::onPlaybackStateChanged(QMediaPlayer::PlaybackState state)
{
    qInfo() << "State changed to:" << state;

    if (state == QMediaPlayer::PlayingState) {
        ui->PlayPause->setText("Pause");
        ui->PlayPause->setIcon(QIcon("../../icons/pause-circle.png"));
    } else {
        ui->PlayPause->setText("Play");
        ui->PlayPause->setIcon(QIcon("../../icons/play-circle.png"));
    }
}

void PlayerControlsWidget::on_Repeat_clicked()
{
    repeatMode = (repeatMode + 1) % 3;

    switch (repeatMode){
    case REPEAT_OFF:
        ui->Repeat->setText("Repeat: Off");
        break;
    case REPEAT_PLAYLIST:
        ui->Repeat->setText("Repeat: Playlist");
        break;
    case REPEAT_SINGLE:
        ui->Repeat->setText("Repeat: Single");
        break;
    default:
        ui->Repeat->setText("Should not happen");
        break;
    }

    emit repeatChanged(repeatMode);
}


void PlayerControlsWidget::on_Shuffle_clicked()
{
    shuffleOn = !shuffleOn;

    if (shuffleOn) {
        ui->Shuffle->setText("Shuffle: On");
    } else {
        ui->Shuffle->setText("Shuffle: Off");
    }

    emit shuffleChanged(shuffleOn);
}

