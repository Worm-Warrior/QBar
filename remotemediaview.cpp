#include "remotemediaview.h"
#include "ui_remotemediaview.h"
#include "mainwindow.h"
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QHeaderView>
#include <QTimer>
#include "appconfig.h"

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
    connect(ui->mediaView->horizontalHeader(), &QHeaderView::sortIndicatorChanged, this, &RemoteMediaView::onTableSorted);
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
    ui->mediaView->horizontalHeader()->resizeSections(QHeaderView::ResizeToContents);
    ui->mediaView->setSortingEnabled(true);
    // ui->mediaView->sortByColumn(COL_TRACK, Qt::AscendingOrder);
    ui->mediaView->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
    ui->mediaView->horizontalHeader()->setSectionResizeMode(COL_DURATION, QHeaderView::ResizeToContents);

    auto *h = ui->mediaView->horizontalHeader();
    h->setStretchLastSection(false);
    h->setSectionResizeMode(QHeaderView::Interactive);

    h->setSectionResizeMode(COL_TRACK,    QHeaderView::ResizeToContents);
    h->setSectionResizeMode(COL_DURATION, QHeaderView::ResizeToContents);
    h->setSectionResizeMode(COL_NAME,     QHeaderView::Stretch);

}

void RemoteMediaView::fetchAlbum(QString id)
{
    if (id.isEmpty()) {
        qWarning() << "Attempted to fetch album with empty ID";
        return;
    }

    QString url = QString("%1/rest/getAlbum.view?"
                          "id=%2&u=%3&p=%4&v=1.16.1&c=QBar&f=json")
                      .arg(AppConfig::serverURL())
                      .arg(id)
                      .arg(AppConfig::username())
                      .arg(AppConfig::password());



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

    // For this view, we need to disable sorting while parsing the album.
    // Otherwise we get bad data when trying to get the sorting to work.
    ui->mediaView->setSortingEnabled(false);

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
        auto *item = new QTableWidgetItem;
        item->setData(Qt::DisplayRole, track.trackNumber);
        item->setData(Qt::UserRole, track.trackNumber);
        ui->mediaView->setItem(i, COL_TRACK, item);

        ui->mediaView->setItem(i, COL_NAME,
                               new QTableWidgetItem(track.title));
        ui->mediaView->setItem(i, COL_ALBUM,
                               new QTableWidgetItem(track.album));
        ui->mediaView->setItem(i, COL_DURATION,
                               new QTableWidgetItem(durationStr));
        ui->mediaView->setItem(i, COL_ID,
                               new QTableWidgetItem(track.id));
    }

    // At the end, make enable sorting and do the header resizing.
    ui->mediaView->setSortingEnabled(true);
    ui->mediaView->sortByColumn(COL_TRACK, Qt::AscendingOrder);
    ui->mediaView->resizeColumnsToContents();
    
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

void RemoteMediaView::onTableSorted(int index, Qt::SortOrder order) {
    Q_UNUSED(index);
    Q_UNUSED(order);
    
    qInfo() << "Calling resort!";

    QTimer::singleShot(0, this, &RemoteMediaView::rebuildPlaylistToUI);
}


// TODO: we need to make this only happen when we play a new album
// Right now it is happening on sort of a NON-PLAYING album
void RemoteMediaView::rebuildPlaylistToUI() {
    QList<Track> newOrder;
    for (int row = 0; row < ui->mediaView->rowCount(); ++row) {
        QTableWidgetItem *idItem = ui->mediaView->item(row, COL_ID);
        if (idItem) {
            QString id = idItem->text();
            qInfo() << "track_id = " << id;
            for (const Track &track : currentAlbumTracks) {
                if (track.id == id) {
                    newOrder.append(track);
                    break;
                }
            }
        }
    }

    qInfo() << currentAlbumTracks.size();
    qInfo() << newOrder.size();
    
    currentAlbumTracks.clear();
    for (const Track &track : newOrder) {
        currentAlbumTracks.append(track);
        qInfo() << track.title;
    }

    qInfo() << currentAlbumTracks.size();

    mainWindow->updatePlaylist(currentAlbumTracks);
}

void RemoteMediaView::selectedNewTrack(const Track &track) {
    int row = 0;

    for (const Track &t : currentAlbumTracks) {
        if (t.id == track.id) {
            break;
        }
        ++row;
    }

    ui->mediaView->selectRow(row);
}
