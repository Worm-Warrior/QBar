#ifndef PLAYERCONTROLSWIDGET_H
#define PLAYERCONTROLSWIDGET_H

#include <QWidget>
#include <QtMultimedia/QtMultimedia>

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
    void setCurMusic(QString filePath);
    void playNext(QString filePath);
    void playPrev(QString filePath);
    QMediaPlayer *player = nullptr;
    void on_durationChanged(qint64 position);
    void on_positionChanged(qint64 duration);
    void updateInfoLabels();

private slots:
    void on_PlayPause_clicked();
    void on_Mute_clicked();
    void on_Volume_valueChanged(int value);
    void on_seekBar_sliderMoved(int position);
    void on_seekBar_sliderReleased();
    void on_seekBar_sliderPressed();

signals:
    void nextClicked();
    void prevClicked();
    void playClicked();
    void pauseClicked();

private:
    Ui::PlayerControlsWidget *ui;
    bool userIsSeeking;

};

#endif // PLAYERCONTROLSWIDGET_H
