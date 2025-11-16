#ifndef REMOTEMEDIAVIEW_H
#define REMOTEMEDIAVIEW_H

#include <QWidget>
#include <QNetworkAccessManager>
#include <qtreewidget.h>
#include "mainwindow.h"
#include "playlist.h"

namespace Ui {
class RemoteMediaView;
}

class RemoteMediaView : public QWidget
{
    Q_OBJECT

public:
    explicit RemoteMediaView(QWidget *parent = nullptr);
    ~RemoteMediaView();
    void fetchAlbum(QString id);
    void setMainWindow(MainWindow *mw);

private:
    Ui::RemoteMediaView *ui;
    QNetworkAccessManager *networkManager;
    MainWindow *mainWindow;
    void setupHeaderCols();
    void displayAlbum(QJsonObject response);
    void onNetworkReply(QNetworkReply *r);
    void handleAlbumRequest(QNetworkReply *r);
    void onItemDoubleClicked(int col, int row);
    int m_curIndex;

    QList<Track> currentAlbumTracks;

    enum ColIndex {
        COL_TRACK = 0,
        COL_NAME,
        COL_ALBUM,
        COL_DURATION,
        COL_ID,
        COL_COUNT,
    };
signals:
    void songSelected(QString trackId);
    void playAlbum(const QList<Track> &tracks, int startIndex);
};

#endif // REMOTEMEDIAVIEW_H
