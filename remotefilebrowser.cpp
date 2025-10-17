#include "remotefilebrowser.h"
#include "ui_remotefilebrowser.h"

RemoteFileBrowser::RemoteFileBrowser(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::RemoteFileBrowser)
{
    ui->setupUi(this);
    networkManager = new QNetworkAccessManager(this);

    connect(networkManager, &QNetworkAccessManager::finished, this, &RemoteFileBrowser::onArtistsReceived);

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
        QByteArray response = reply->readAll();

        QJsonDocument doc = QJsonDocument::fromJson(response);
        QJsonObject obj = doc.object();

        qInfo() << "JSON RESPONSE:" << doc.toJson(QJsonDocument::Indented);

        QJsonObject subsonic_res = obj["subsonic-response"].toObject();
        QJsonObject artists = subsonic_res["artists"].toObject();
        QJsonArray indexArray = artists["index"].toArray();

    } else {
        qInfo() << "error" << reply->errorString() << Qt::endl;
    }
}
