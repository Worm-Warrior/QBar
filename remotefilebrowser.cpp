#include "remotefilebrowser.h"
#include "ui_remotefilebrowser.h"
#include <qlabel.h>

RemoteFileBrowser::RemoteFileBrowser(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::RemoteFileBrowser)
{
    ui->setupUi(this);
    networkManager = new QNetworkAccessManager(this);

    connect(networkManager, &QNetworkAccessManager::finished, this, &RemoteFileBrowser::onArtistsReceived);

    setupHeaders();

    fetchArtists();
}

RemoteFileBrowser::~RemoteFileBrowser()
{
    delete ui;
}

void RemoteFileBrowser::fetchArtists() {
    QString url = ("http://192.168.4.165:4533/rest/getArtists.view?type=frequent&u=admin&p=rat&v=1.16.1&c=QBarn&f=json");

    QNetworkRequest rq(url);
    networkManager->get(rq);
}

void RemoteFileBrowser::onArtistsReceived(QNetworkReply *reply) {
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
                qInfo() << artists[j].toObject();
                //qInfo() << artists[j].toObject()["name"].toString() << "\t" << artists[j].toObject()["albumCount"].toDouble();
                item->setText(COL_ARTIST, artistName);
                item->setText(COL_ALBUM_COUNT, QString::number(albumCount));
                item->setData(COL_ARTIST, Qt::UserRole, artists[j].toObject()["id"].toString());
                item->setData(COL_ARTIST, Qt::UserRole+1, "artist");

                QTreeWidgetItem *dummy = new QTreeWidgetItem(item);
                dummy->setText(0, "LOADING");
            }
        }

    } else {
        qInfo() << "error" << reply->errorString() << Qt::endl;
    }

}
void RemoteFileBrowser::setupHeaders() {
    ui->remoteFileBrowser->setColumnCount(COL_COUNT);

    QStringList headers;

    headers << "Artist" << "Albums";

    ui->remoteFileBrowser->setHeaderLabels(headers);

    ui->remoteFileBrowser->setSortingEnabled(true);
    ui->remoteFileBrowser->sortByColumn(COL_ARTIST, Qt::SortOrder::AscendingOrder);
}
