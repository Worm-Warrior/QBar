#ifndef REMOTEMEDIAVIEW_H
#define REMOTEMEDIAVIEW_H

#include <QWidget>
#include <QNetworkAccessManager>
#include <qtreewidget.h>

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

private:
    Ui::RemoteMediaView *ui;
    QNetworkAccessManager *networkManager;
    void setupHeaderCols();
    void displayAlbum(QJsonObject response);
    void onNetworkReply(QNetworkReply *r);
    void handleAlbumRequest(QNetworkReply *r);
    void onItemDoubleClicked(int col, int row);
    int m_curIndex;

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
};

#endif // REMOTEMEDIAVIEW_H
