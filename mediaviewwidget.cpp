#include "mediaviewwidget.h"
#include "ui_mediaviewwidget.h"
#include "mainwindow.h"
#include <QDir>
#include <QDirIterator>
#include <QFileInfo>
#include <QHeaderView>
#include <QTimer>
#include <taglib/fileref.h>
#include <taglib/tag.h>
#include <taglib/audioproperties.h>

MediaViewWidget::MediaViewWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::MediaViewWidget)
    , mainWindow(nullptr)
{
    ui->setupUi(this);

    setupTableCols();

    // Setup table behavior
    ui->mediaView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->mediaView->setSelectionMode(QAbstractItemView::ExtendedSelection);
    ui->mediaView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->mediaView->setAlternatingRowColors(true);
    ui->mediaView->setSortingEnabled(true);
    ui->mediaView->verticalHeader()->setVisible(false);
    ui->mediaView->horizontalHeader()->setStretchLastSection(true);

    // Connections
    connect(ui->mediaView, &QTableWidget::cellDoubleClicked,
            this, &MediaViewWidget::onItemDoubleClicked);
    connect(ui->mediaView, &QTableWidget::itemSelectionChanged,
            this, &MediaViewWidget::onSelectionChanged);
    connect(ui->mediaView->horizontalHeader(), &QHeaderView::sortIndicatorChanged,
            this, &MediaViewWidget::onTableSorted);
}

MediaViewWidget::~MediaViewWidget()
{
    delete ui;
}

void MediaViewWidget::setMainWindow(MainWindow *mw)
{
    mainWindow = mw;
}

void MediaViewWidget::setupTableCols()
{
    ui->mediaView->setColumnCount(COL_COUNT);

    QStringList headers;
    headers << "Track" << "Title" << "Artist" << "Album"
            << "Duration" << "Format" << "Path";
    ui->mediaView->setHorizontalHeaderLabels(headers);

    // Set column widths
    ui->mediaView->setColumnWidth(COL_TRACK, 50);
    ui->mediaView->setColumnWidth(COL_TITLE, 200);
    ui->mediaView->setColumnWidth(COL_ARTIST, 150);
    ui->mediaView->setColumnWidth(COL_ALBUM, 150);
    ui->mediaView->setColumnWidth(COL_DURATION, 80);
    ui->mediaView->setColumnWidth(COL_FORMAT, 80);

    // Hide path column (we store it but don't display it)
    ui->mediaView->setColumnHidden(COL_PATH, true);
}

void MediaViewWidget::displayFolder(const QString &folderPath)
{
    if (folderPath.isEmpty()) {
        qWarning() << "Attempted to display empty folder path";
        return;
    }

    clearView();
    currentFolderTracks.clear();

    // Get all audio files recursively
    QStringList audioFiles = getSupportedAudioFiles(folderPath);

    if (audioFiles.isEmpty()) {
        qInfo() << "No audio files found in" << folderPath;
        return;
    }

    ui->mediaView->setRowCount(audioFiles.size());
    ui->mediaView->setSortingEnabled(false);

    // Parse and display each file
    for (int i = 0; i < audioFiles.size(); ++i) {
        MediaFile mediaFile = parseMediaFile(audioFiles[i]);

        // Convert to Track and store
        Track track = mediaFileToTrack(mediaFile);
        currentFolderTracks.append(track);

        // Populate table row
        auto *trackItem = new QTableWidgetItem();
        trackItem->setData(Qt::DisplayRole, mediaFile.trackNumber);
        trackItem->setData(Qt::UserRole, mediaFile.trackNumber);

        ui->mediaView->setItem(i, COL_TRACK, trackItem);
        ui->mediaView->setItem(i, COL_TITLE,
                               new QTableWidgetItem(mediaFile.title));
        ui->mediaView->setItem(i, COL_ARTIST,
                               new QTableWidgetItem(mediaFile.artist));
        ui->mediaView->setItem(i, COL_ALBUM,
                               new QTableWidgetItem(mediaFile.album));
        ui->mediaView->setItem(i, COL_DURATION,
                               new QTableWidgetItem(mediaFile.duration));
        ui->mediaView->setItem(i, COL_FORMAT,
                               new QTableWidgetItem(mediaFile.format));
        ui->mediaView->setItem(i, COL_PATH,
                               new QTableWidgetItem(mediaFile.filePath));
    }

    ui->mediaView->setSortingEnabled(true);
    //ui->mediaView->sortByColumn(COL_TRACK, Qt::AscendingOrder);
    //ui->mediaView->verticalHeader()->resizeSections(QHeaderView::ResizeToContents);
    ui->mediaView->horizontalHeader()->resizeSections(QHeaderView::ResizeToContents);


    qInfo() << "Loaded" << audioFiles.size() << "audio files from" << folderPath;
    
    //rebuildPlaylistToUI();
}

void MediaViewWidget::clearView()
{
    ui->mediaView->setRowCount(0);
}

QStringList MediaViewWidget::getSupportedAudioFiles(const QString &folderPath)
{
    QStringList audioFiles;
    QStringList filters;
    filters << "*.mp3" << "*.flac" << "*.wav" << "*.ogg"
            << "*.m4a" << "*.aac" << "*.wma" << "*.opus";

    QDirIterator it(folderPath, filters, QDir::Files, QDirIterator::Subdirectories);
    while (it.hasNext()) {
        audioFiles.append(it.next());
    }

    return audioFiles;
}

QStringList MediaViewWidget::getSelectedFiles() const
{
    QStringList selectedFiles;
    QList<QTableWidgetItem*> selectedItems = ui->mediaView->selectedItems();
    QSet<int> selectedRows;

    for (QTableWidgetItem *item : selectedItems) {
        selectedRows.insert(item->row());
    }

    for (int row : selectedRows) {
        QTableWidgetItem *pathItem = ui->mediaView->item(row, COL_PATH);
        if (pathItem) {
            selectedFiles.append(pathItem->text());
        }
    }

    return selectedFiles;
}

MediaFile MediaViewWidget::parseMediaFile(const QString &filePath)
{
    MediaFile mediaFile;
    mediaFile.filePath = filePath;

    QFileInfo fileInfo(filePath);
    mediaFile.format = fileInfo.suffix().toUpper();
    mediaFile.fileSize = fileInfo.size();

    // Parse metadata using TagLib
    TagLib::FileRef f(filePath.toUtf8().constData());

    if (!f.isNull() && f.tag()) {
        mediaFile.title = QString::fromStdString(f.tag()->title().to8Bit(true));
        mediaFile.artist = QString::fromStdString(f.tag()->artist().to8Bit(true));
        mediaFile.album = QString::fromStdString(f.tag()->album().to8Bit(true));
        mediaFile.trackNumber = f.tag()->track();

        if (f.audioProperties()) {
            int totalSeconds = f.audioProperties()->length();
            int minutes = totalSeconds / 60;
            int seconds = totalSeconds % 60;
            mediaFile.duration = QString("%1:%2")
                                     .arg(minutes, 2, 10, QChar('0'))
                                     .arg(seconds, 2, 10, QChar('0'));
        }
    }

    // Fallback to filename if metadata is empty
    if (mediaFile.title.isEmpty()) {
        mediaFile.title = fileInfo.fileName();
    }
    if (mediaFile.artist.isEmpty()) {
        mediaFile.artist = "Unknown Artist";
    }
    if (mediaFile.album.isEmpty()) {
        mediaFile.album = "Unknown Album";
    }

    return mediaFile;
}

Track MediaViewWidget::mediaFileToTrack(const MediaFile &mediaFile)
{
    Track track;
    track.id = "";  // Not used for local files
    track.title = mediaFile.title;
    track.artist = mediaFile.artist;
    track.album = mediaFile.album;
    track.trackNumber = mediaFile.trackNumber;

    // Parse duration string back to seconds
    QStringList timeParts = mediaFile.duration.split(':');
    if (timeParts.size() == 2) {
        track.duration = timeParts[0].toInt() * 60 + timeParts[1].toInt();
    }

    track.filePath = mediaFile.filePath;
    track.isRemote = false;

    return track;
}

void MediaViewWidget::onItemDoubleClicked(int row, int column)
{
    Q_UNUSED(column);

    if (!mainWindow) {
        qWarning() << "MainWindow not set!";
        return;
    }

    if (currentFolderTracks.isEmpty()) {
        qWarning() << "No tracks loaded";
        return;
    }

    if (row < 0 || row >= currentFolderTracks.count()) {
        qWarning() << "Invalid row:" << row;
        return;
    }

    // Tell MainWindow to play all tracks in current folder, starting at selected track
    QTableWidgetItem *item = ui->mediaView->item(row,COL_PATH);

    if (!item) {
        qWarning() << "No path item at row: " << row;
        return;
    }

    QString path = item->text();

    int index = -1;

    for (int i = 0; i < currentFolderTracks.count(); i++) {
        if (currentFolderTracks[i].filePath == path) {
            index = i;
            break;
        }
    }

    if (index == -1) {
        qWarning() << "did not find path of file in row: " << row;
        return;
    }

    mainWindow->playNewPlaylist(currentFolderTracks, index);
}

void MediaViewWidget::onSelectionChanged()
{
    emit selectionChanged(getSelectedFiles());
}

// The user has changed the sort of the UI, rebuild playlist.
void MediaViewWidget::onTableSorted(int index, Qt::SortOrder order) {
    Q_UNUSED(index);
    Q_UNUSED(order);

    if (mainWindow->playState.currentPath != currentView) {
        return;
    }

    qInfo() << "Calling resort!";

    QTimer::singleShot(0, this, &MediaViewWidget::rebuildPlaylistToUI);
}

// TODO: we need to make this only happen when we play a new folder
// Right now it is happening on sort of a NON-PLAYING folder
void MediaViewWidget::rebuildPlaylistToUI() {
    QList<Track> newOrder;
    for (int row = 0; row < ui->mediaView->rowCount(); ++row) {
        QTableWidgetItem *pathItem = ui->mediaView->item(row, COL_PATH);
        if (pathItem) {
            QString path = pathItem->text();
            for (const Track &track : currentFolderTracks) {
                if (track.filePath == path) {
                    newOrder.append(track);
                    break;
                }
            }
        }
    }

        // Have new order, now add to the old track list

        qInfo() << currentFolderTracks.size();
        qInfo() << newOrder.size();

        currentFolderTracks.clear();
        for (const Track &track : newOrder) {
            currentFolderTracks.append(track);
            qInfo() << track.title;
        }
        qInfo() << currentFolderTracks.size();

        mainWindow->updatePlaylist(currentFolderTracks);
}

void MediaViewWidget::selectNewTrack(const Track &track) {
    int row = 0;

    for (Track &t : currentFolderTracks) {
        if (t.title == track.title) {
            break;
        }
        ++row;
    }

    ui->mediaView->selectRow(row);
}

void MediaViewWidget::onFolderSelected(const QString &folderPath) {
    currentView = folderPath;

    if (currentView == mainWindow->playState.currentPath) {
        ui->mediaView->selectRow(mainWindow->getCurrentTrackIndex());
    }
}
