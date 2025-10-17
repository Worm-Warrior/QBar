#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    QString username;
    QString password;
    QString serverUrl;

private slots:
    void changeRoot();
    void actionAbout();
    void actionExit();
    void playNextTrack();
    void playPrevTrack();
    void loopTracks();
    void remoteModeToggle();

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
