#ifndef REMOTEMEDIAVIEW_H
#define REMOTEMEDIAVIEW_H

#include <QWidget>
#include <QNetworkAccessManager>
#include "playlist.h"

// Forward declaration to avoid circular dependency
class MainWindow;

namespace Ui {
class RemoteMediaView;
}

class RemoteMediaView : public QWidget
{
    Q_OBJECT

public:
    explicit RemoteMediaView(QWidget *parent = nullptr);
    ~RemoteMediaView();

    void setMainWindow(MainWindow *mw);
    void fetchAlbum(QString id);

private slots:
    void onNetworkReply(QNetworkReply *r);
    void onItemDoubleClicked(int row, int col);
    void onTableSorted(int index, Qt::SortOrder order);

private:
    Ui::RemoteMediaView *ui;
    QNetworkAccessManager *networkManager;
    MainWindow *mainWindow;
    QList<Track> currentAlbumTracks;
    void rebuildPlaylistToUI();

    enum ColIndex {
        COL_TRACK = 0,
        COL_NAME,
        COL_ALBUM,
        COL_DURATION,
        COL_ID,
        COL_COUNT
    };

    void setupHeaderCols();
    void handleAlbumRequest(QNetworkReply *r);
};

#endif // REMOTEMEDIAVIEW_H
