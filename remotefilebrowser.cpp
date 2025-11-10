#include "remotefilebrowser.h"
#include "ui_remotefilebrowser.h"
#include <qlabel.h>

RemoteFileBrowser::RemoteFileBrowser(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::RemoteFileBrowser)
{
    ui->setupUi(this);
    networkManager = new QNetworkAccessManager(this);

    connect(networkManager, &QNetworkAccessManager::finished, this, &RemoteFileBrowser::onNetworkReply);

    connect(ui->remoteFileBrowser, &QTreeWidget::itemExpanded, this, &RemoteFileBrowser::onItemExpanded);

    connect(ui->remoteFileBrowser, &QTreeWidget::itemClicked, this, &RemoteFileBrowser::onItemSelected);

    setupHeaders();

    fetchArtists();
}

RemoteFileBrowser::~RemoteFileBrowser()
{
    delete ui;
}

void RemoteFileBrowser::fetchArtists() {
    QString url = ("http://192.168.4.165:4533/rest/getArtists.view?type=frequent&u=admin&p=rat&v=1.16.1&c=QBar&f=json");

    QNetworkRequest rq(url);
    QNetworkReply *r = networkManager->get(rq);
    r->setProperty("requestType", "artists");
}

void RemoteFileBrowser::onNetworkReply(QNetworkReply *reply) {
    if (reply->error() != QNetworkReply::NoError) {
        qInfo() << "Error:" << reply->errorString();
        reply->deleteLater();
        return;
    }

    QString requestType = reply->property("requestType").toString();

    if (requestType == "artists") {
        handleArtistsRecived(reply);
    } else if (requestType == "albums") {
        handleArtistAlbumsRecived(reply);
    }

    reply->deleteLater();
}

void RemoteFileBrowser::handleArtistsRecived(QNetworkReply *reply) {
    if (reply->error() == QNetworkReply::NoError) {
        ui->remoteFileBrowser->clear();
        QByteArray response = reply->readAll();
        QJsonDocument doc = QJsonDocument::fromJson(response);
        QJsonObject obj = doc.object();

        //qInfo() << "JSON RESPONSE:" << doc.toJson(QJsonDocument::Indented);

        QJsonObject subsonic_res = obj["subsonic-response"].toObject();
        QJsonObject artists = subsonic_res["artists"].toObject();
        QJsonArray indexArray = artists["index"].toArray();

        // TODO:
        // indexArray[n] is the char of starting name so [1] is the 'a' char.
        // Then after that we do toObject()["artist"] to get all the artists with that starting char.
        // Then we can loop over that and display the artists.

        for (int i = 0; i < indexArray.size(); i++) {
            QJsonArray artists = indexArray[i].toObject()["artist"].toArray();
            //qInfo() << artists[i].toObject()["id"];
            //qInfo() << indexArray[i].toObject()["artist"].toArray()[0];

            for (int j = 0; j < artists.size(); j++) {
                QTreeWidgetItem *item = new QTreeWidgetItem(ui->remoteFileBrowser);
                QString artistName = artists[j].toObject()["name"].toString();
                double albumCount = artists[j].toObject()["albumCount"].toDouble();
                QString id = artists[j].toObject()["id"].toString();
                //qInfo() << artists[j].toObject();
                //qInfo() << artists[j].toObject()["name"].toString() << "\t" << artists[j].toObject()["albumCount"].toDouble();
                item->setText(COL_NAME, artistName);
                item->setText(COL_COUNT, QString::number(albumCount));
                item->setData(COL_NAME, Qt::UserRole, id);
                item->setData(COL_NAME, Qt::UserRole+1, "artist");

                QTreeWidgetItem *dummy = new QTreeWidgetItem(item);
                dummy->setText(0, "LOADING...");

                artistIdMap.insert(artistName, id);
            }
        }

    } else {
        qInfo() << "error" << reply->errorString() << Qt::endl;
    }

    //qInfo() << artistIdMap << Qt::endl;

}
void RemoteFileBrowser::setupHeaders() {
    ui->remoteFileBrowser->setColumnCount(COL_COUNT);

    QStringList headers;

    headers << "Artist" << "Albums";

    ui->remoteFileBrowser->setHeaderLabels(headers);

    ui->remoteFileBrowser->setSortingEnabled(true);
    ui->remoteFileBrowser->sortByColumn(COL_NAME, Qt::SortOrder::AscendingOrder);

    ui->remoteFileBrowser->header()->setSectionResizeMode(COL_NAME, QHeaderView::Stretch);
    ui->remoteFileBrowser->header()->setSectionResizeMode(COL_COUNT, QHeaderView::ResizeToContents);
}

void RemoteFileBrowser::onItemExpanded(QTreeWidgetItem *item) {
    QString type = item->data(COL_NAME, Qt::UserRole+1).toString();

    if (type == "artist") {
        bool albumsAlreadyLoaded = item->data(COL_NAME, Qt::UserRole+2).toBool();

        if (!albumsAlreadyLoaded) {
            QString artistId = item->data(COL_NAME, Qt::UserRole).toString();
            fetchArtistAlbums(artistId, item);
        } else {
            return;
        }
    }
}

void RemoteFileBrowser::onItemSelected(QTreeWidgetItem *item, int row) {
    if (item->childCount() != 0) {
        return;
    }
    QString id = item->data(COL_NAME, Qt::UserRole).toString();

    emit albumSelected(id);
}

void RemoteFileBrowser::fetchArtistAlbums(QString artistId, QTreeWidgetItem *parent) {
    //qInfo() << artistId;
    QString url = "http://192.168.4.165:4533/rest/getArtist.view?id=" + artistId + "&u=admin&p=rat&v=1.16.1&c=QBar&f=json";
    QNetworkRequest rq(url);
    QNetworkReply *r = networkManager->get(rq);
    r->setProperty("requestType", "albums");
    r->setProperty("artistId", artistId);
    // Store the parent item in the reply!
    r->setProperty("parentItem", QVariant::fromValue(parent));
}


void RemoteFileBrowser::handleArtistAlbumsRecived(QNetworkReply *reply) {
    // Convert to the right type
    QTreeWidgetItem *parent = reply->property("parentItem").value<QTreeWidgetItem*>();

    qInfo() << parent->text(COL_NAME);

    QByteArray response = reply->readAll();
    QJsonDocument doc = QJsonDocument::fromJson(response);
    QJsonObject obj = doc.object();

    qInfo() << obj;

    QJsonObject subsonicResponse = obj["subsonic-response"].toObject();
    QJsonObject artist = subsonicResponse["artist"].toObject();
    QJsonArray albums = artist["album"].toArray();

    for (int i = 0; i < parent->childCount(); i++) {
        QTreeWidgetItem *dummy = parent->child(i);
        qInfo() << dummy->text(COL_NAME);
        parent->removeChild(dummy);
    }

    for (int i = 0; i < albums.size(); i++) {
        QTreeWidgetItem *album = new QTreeWidgetItem();
        album->setText(COL_NAME,albums[i].toObject()["name"].toString() + " - (" + QString::number(albums[i].toObject()["songCount"].toInt()) + ")");
        album->setData(COL_NAME, Qt::UserRole, albums[i].toObject()["id"].toString());
        parent->addChild(album);
    }

    parent->setData(COL_NAME, Qt::UserRole+2, true);

}
