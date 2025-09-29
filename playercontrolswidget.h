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

private slots:
    void on_PlayPause_clicked();

    void on_Mute_clicked();

    void on_Volume_valueChanged(int value);

private:
    Ui::PlayerControlsWidget *ui;
    QMediaPlayer *player = nullptr;
};

#endif // PLAYERCONTROLSWIDGET_H
