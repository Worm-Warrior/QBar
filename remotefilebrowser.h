#ifndef REMOTEFILEBROWSER_H
#define REMOTEFILEBROWSER_H

#include <QWidget>
#include <qnetworkaccessmanager.h>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <qtreewidget.h>

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
    QHash<QString, QString> artistIdMap;
    void onItemExpanded(QTreeWidgetItem *item);
    void fetchArtistAlbums(QString artistId);
    void onNetworkReply(QNetworkReply *reply);

    enum ColIndex {
        COL_NAME = 0,
        COL_ALBUM_COUNT,
        COL_COUNT,
    };

private slots:
    void handleArtistsRecived(QNetworkReply *reply);
    void handleArtistAlbumsRecived(QNetworkReply *reply);
};

#endif // REMOTEFILEBROWSER_H
