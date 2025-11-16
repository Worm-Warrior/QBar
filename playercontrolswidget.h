#ifndef PLAYERCONTROLSWIDGET_H
#define PLAYERCONTROLSWIDGET_H

#include <QWidget>
#include <QMediaPlayer>

namespace Ui {
class PlayerControlsWidget;
}

class PlayerControlsWidget : public QWidget
{
    Q_OBJECT

public:
    explicit PlayerControlsWidget(QWidget *parent = nullptr);
    ~PlayerControlsWidget();

    void setPlayer(QMediaPlayer *p);

    // Public slots for MainWindow to update UI
    void on_positionChanged(qint64 position);
    void on_durationChanged(qint64 duration);
    void updateInfoLabels();

    // Public access to player (MainWindow needs this)
    QMediaPlayer *player;

signals:
    void nextClicked();
    void prevClicked();

private slots:
    void on_PlayPause_clicked();
    void on_Mute_clicked();
    void on_Volume_valueChanged(int value);
    void on_seekBar_sliderMoved(int position);
    void on_seekBar_sliderReleased();
    void on_seekBar_sliderPressed();
    void onPlaybackStateChanged(QMediaPlayer::PlaybackState state);

private:
    Ui::PlayerControlsWidget *ui;
    bool userIsSeeking;
};

#endif // PLAYERCONTROLSWIDGET_H
