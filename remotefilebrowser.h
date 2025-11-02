#ifndef REMOTEFILEBROWSER_H
#define REMOTEFILEBROWSER_H

#include <QWidget>
#include <qnetworkaccessmanager.h>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

namespace Ui {
class RemoteFileBrowser;
}

class RemoteFileBrowser : public QWidget
{
    Q_OBJECT

public:
    explicit RemoteFileBrowser(QWidget *parent = nullptr);
    ~RemoteFileBrowser();

private:
    Ui::RemoteFileBrowser *ui;
    QNetworkAccessManager *networkManager;
    void fetchArtists();
    void setupHeaders();

    enum ColIndex {
        COL_ARTIST = 0,
        COL_ALBUM_COUNT,
        COL_COUNT,
    };

private slots:
    void onArtistsReceived(QNetworkReply *reply);
};

#endif // REMOTEFILEBROWSER_H
