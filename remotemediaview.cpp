#include "remotemediaview.h"
#include "ui_remotemediaview.h"
#include <qjsonarray.h>
#include <qjsondocument.h>
#include <qjsonobject.h>
#include <qnetworkreply.h>
#include <qnetworkrequest.h>
#include <qtreewidget.h>

RemoteMediaView::RemoteMediaView(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::RemoteMediaView)
{
    ui->setupUi(this);
    ui->mediaView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->mediaView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->mediaView->verticalHeader()->setVisible(false);
    networkManager = new QNetworkAccessManager(this);

    setupHeaderCols();

    connect(networkManager, &QNetworkAccessManager::finished, this, &RemoteMediaView::onNetworkReply);
}

RemoteMediaView::~RemoteMediaView()
{
    delete ui;
}

void RemoteMediaView::setupHeaderCols() {
    ui->mediaView->setColumnCount(COL_COUNT);

    QStringList headers;
    headers << "Track" << "Title" << "Album" << "Duration";

    //ui->mediaView->resizeColumnsToContents();

    ui->mediaView->setHorizontalHeaderLabels(headers);
    ui->mediaView->setEditTriggers(QAbstractItemView::NoEditTriggers);
}

void RemoteMediaView::fetchAlbum(QString id) {
    QString url = ("http://192.168.4.165:4533/rest/getAlbum.view?id="+id+"&u=admin&p=rat&v=1.16.1&c=QBar&f=json");

    qInfo() << id;

    QNetworkRequest rq(url);
    QNetworkReply *r = networkManager->get(rq);
    r->setProperty("requestType", "albumList");
}

void RemoteMediaView::onNetworkReply(QNetworkReply *r) {
    if (r->error() != QNetworkReply::NoError) {
        qInfo() << "Error:" << r->errorString();
        r->deleteLater();
        return;
    }

    QString reqType = r->property("requestType").toString();

    if (reqType == "albumList") {
        qInfo() << "album list requested";
        handleAlbumRequest(r);
    } else if (reqType == "playTrack") {
        qInfo() << "play requested";
    }
}

void RemoteMediaView::handleAlbumRequest(QNetworkReply *r) {

    QByteArray response = r->readAll();
    QJsonDocument doc = QJsonDocument::fromJson(response);
    QJsonObject obj = doc.object();

    QJsonObject album = obj["subsonic-response"].toObject()["album"].toObject();

    QJsonArray songs = album["song"].toArray();

    ui->mediaView->setRowCount(songs.count());
    ui->mediaView->clearContents();

    qInfo() << songs;

    for (int i = 0; i < songs.count(); i++) {
        auto *trackItem = new QTableWidgetItem();

        QJsonObject song = songs[i].toObject();

        QString trackTitle = song["title"].toString();
        int trackNumber = song["track"].toInt();
        QString albumName = song["album"].toString();
        int durationSeconds = song["duration"].toInt();

        qInfo() << trackTitle + " - " + QString::number(trackNumber) + " -- " + albumName +" : " + QString::number(durationSeconds);
        trackItem->setData(Qt::DisplayRole, trackNumber);
        trackItem->setData(Qt::UserRole, trackNumber);

        ui->mediaView->setItem(i, COL_TRACK, trackItem);
        ui->mediaView->setItem(i, COL_NAME,
                               new QTableWidgetItem(trackTitle));
        ui->mediaView->setItem(i, COL_ALBUM,
                               new QTableWidgetItem(albumName));
    }
}
