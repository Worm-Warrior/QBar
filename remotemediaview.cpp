#include "remotemediaview.h"
#include "ui_remotemediaview.h"
#include "mainwindow.h"
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QHeaderView>

RemoteMediaView::RemoteMediaView(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::RemoteMediaView)
    , mainWindow(nullptr)
{
    ui->setupUi(this);

    // Setup table view behavior
    ui->mediaView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->mediaView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->mediaView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->mediaView->verticalHeader()->setVisible(false);

    // Setup network manager
    networkManager = new QNetworkAccessManager(this);

    setupHeaderCols();

    // Connections
    connect(networkManager, &QNetworkAccessManager::finished,
            this, &RemoteMediaView::onNetworkReply);
    connect(ui->mediaView, &QTableWidget::cellDoubleClicked,
            this, &RemoteMediaView::onItemDoubleClicked);
}

RemoteMediaView::~RemoteMediaView()
{
    delete ui;
}

void RemoteMediaView::setMainWindow(MainWindow *mw)
{
    mainWindow = mw;
}

void RemoteMediaView::setupHeaderCols()
{
    ui->mediaView->setColumnCount(COL_COUNT);

    QStringList headers;
    headers << "Track" << "Title" << "Album" << "Duration" << "ID";
    ui->mediaView->setHorizontalHeaderLabels(headers);

    // Hide the ID column (we store it but don't display it)
    ui->mediaView->hideColumn(COL_ID);

    // Set resize modes
    ui->mediaView->horizontalHeader()->setSectionResizeMode(COL_TRACK, QHeaderView::ResizeToContents);
    ui->mediaView->horizontalHeader()->setSectionResizeMode(COL_NAME, QHeaderView::Stretch);
    ui->mediaView->horizontalHeader()->setSectionResizeMode(COL_ALBUM, QHeaderView::Stretch);
    ui->mediaView->horizontalHeader()->setSectionResizeMode(COL_DURATION, QHeaderView::ResizeToContents);
}

void RemoteMediaView::fetchAlbum(QString id)
{
    if (id.isEmpty()) {
        qWarning() << "Attempted to fetch album with empty ID";
        return;
    }

    // Build URL - credentials should come from MainWindow
    QString url = QString("http://192.168.4.165:4533/rest/getAlbum.view?"
                          "id=%1&u=admin&p=rat&v=1.16.1&c=QBar&f=json")
                      .arg(id);

    QNetworkRequest request(url);
    QNetworkReply *reply = networkManager->get(request);
    reply->setProperty("requestType", "albumList");

    qInfo() << "Fetching album:" << id;
}

void RemoteMediaView::onNetworkReply(QNetworkReply *reply)
{
    if (reply->error() != QNetworkReply::NoError) {
        qWarning() << "Network error:" << reply->errorString();
        reply->deleteLater();
        return;
    }

    QString requestType = reply->property("requestType").toString();

    if (requestType == "albumList") {
        handleAlbumRequest(reply);
    }

    reply->deleteLater();
}

void RemoteMediaView::handleAlbumRequest(QNetworkReply *reply)
{
    QByteArray response = reply->readAll();
    QJsonDocument doc = QJsonDocument::fromJson(response);
    QJsonObject obj = doc.object();

    QJsonObject subsonicResponse = obj["subsonic-response"].toObject();
    QJsonObject album = subsonicResponse["album"].toObject();
    QJsonArray songs = album["song"].toArray();

    // Clear previous album
    currentAlbumTracks.clear();
    ui->mediaView->setRowCount(0);
    ui->mediaView->setRowCount(songs.count());

    // Build track list and populate table
    for (int i = 0; i < songs.count(); i++) {
        QJsonObject song = songs[i].toObject();

        // Create Track struct
        Track track;
        track.id = song["id"].toString();
        track.title = song["title"].toString();
        track.artist = song["artist"].toString();
        track.album = song["album"].toString();
        track.trackNumber = song["track"].toInt();
        track.duration = song["duration"].toInt();
        track.isRemote = true;
        track.filePath = "";  // Not needed for remote tracks

        currentAlbumTracks.append(track);

        // Format duration as MM:SS
        int totalSeconds = track.duration;
        int minutes = totalSeconds / 60;
        int seconds = totalSeconds % 60;
        QString durationStr = QString("%1:%2")
                                  .arg(minutes, 2, 10, QChar('0'))
                                  .arg(seconds, 2, 10, QChar('0'));

        // Populate table row
        ui->mediaView->setItem(i, COL_TRACK,
                               new QTableWidgetItem(QString::number(track.trackNumber)));
        ui->mediaView->setItem(i, COL_NAME,
                               new QTableWidgetItem(track.title));
        ui->mediaView->setItem(i, COL_ALBUM,
                               new QTableWidgetItem(track.album));
        ui->mediaView->setItem(i, COL_DURATION,
                               new QTableWidgetItem(durationStr));
        ui->mediaView->setItem(i, COL_ID,
                               new QTableWidgetItem(track.id));
    }

    qInfo() << "Loaded" << currentAlbumTracks.count() << "tracks";
}

void RemoteMediaView::onItemDoubleClicked(int row, int col)
{
    Q_UNUSED(col);

    if (!mainWindow) {
        qWarning() << "MainWindow not set!";
        return;
    }

    if (currentAlbumTracks.isEmpty()) {
        qWarning() << "No tracks loaded";
        return;
    }

    if (row < 0 || row >= currentAlbumTracks.count()) {
        qWarning() << "Invalid row:" << row;
        return;
    }

    // Tell MainWindow to play this album starting at the selected track
    mainWindow->playNewPlaylist(currentAlbumTracks, row);
}
