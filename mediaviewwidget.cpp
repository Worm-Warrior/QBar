#include "mediaviewwidget.h"
#include "ui_mediaviewwidget.h"
#include <qdir.h>
#include <qdiriterator.h>

MediaViewWidget::MediaViewWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::MediaViewWidget)
{
    ui->setupUi(this);
    setupTableCols();
    // -1 means no file playing
    m_curIndex = -1;

    ui->mediaView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->mediaView->setSelectionMode(QAbstractItemView::ExtendedSelection);
    ui->mediaView->setAlternatingRowColors(true);
    ui->mediaView->setSortingEnabled(true);
    ui->mediaView->verticalHeader()->setVisible(false);

    ui->mediaView->horizontalHeader()->setStretchLastSection(true);
    ui->mediaView->horizontalHeader()->resizeSections(QHeaderView::Interactive);

    connect(ui->mediaView, &QTableWidget::cellDoubleClicked,
            this, &MediaViewWidget::onItemDoubleClicked);
    connect(ui->mediaView, &QTableWidget::itemSelectionChanged,
            this, &MediaViewWidget::onSelectionChanged);
}

QString MediaViewWidget::getCurrentFile() const
{
    if (m_curIndex >= 0 && m_curIndex < ui->mediaView->rowCount())
    {
        QTableWidgetItem *pathItem = ui->mediaView->item(m_curIndex, COL_PATH);
        if (pathItem)
        {
            return pathItem->text();
        }
    }
    return QString();
}

void MediaViewWidget::setupTableCols() {
    ui->mediaView->setColumnCount(COL_COUNT);

    QStringList headers;

    headers << "Track" << "Title" << "Artist" << "Album"
            << "Duration" << "Format" << "Path";

    ui->mediaView->setHorizontalHeaderLabels(headers);

    ui->mediaView->setColumnWidth(COL_TRACK, 50);
    ui->mediaView->setColumnWidth(COL_TITLE, 200);
    ui->mediaView->setColumnWidth(COL_ARTIST, 150);
    ui->mediaView->setColumnWidth(COL_ALBUM, 150);
    ui->mediaView->setColumnWidth(COL_DURATION, 80);
    ui->mediaView->setColumnWidth(COL_FORMAT, 80);

    ui->mediaView->setColumnHidden(COL_PATH, true);
    ui->mediaView->setEditTriggers(QAbstractItemView::NoEditTriggers);
}

void MediaViewWidget::displayFolder(const QString &folderPath) {
    clearView();

    // These are what we need if we want to get the next and prev!
    QStringList audioFiles = getSupportedAudioFiles(folderPath);

    ui->mediaView->setRowCount(audioFiles.size());

    ui->mediaView->setSortingEnabled(false);

    for (int i = 0; i < audioFiles.size(); ++i) {
        MediaFile mediaFile = parseMediaFile(audioFiles[i]);

        ui->mediaView->setItem(i, COL_TRACK,
                           new QTableWidgetItem(QString::number(mediaFile.trackNumber)));
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

        ui->mediaView->item(i, COL_TRACK)->setData(Qt::UserRole, mediaFile.trackNumber);
    }

    ui->mediaView->setSortingEnabled(true);

    ui->mediaView->sortByColumn(COL_TRACK, Qt::AscendingOrder);

    emit newPlayList(audioFiles);
}

MediaViewWidget::~MediaViewWidget()
{
    delete ui;
}

void MediaViewWidget::addMediaFile(const MediaFile &mediaFile) {
    int row = ui->mediaView->rowCount();
    ui->mediaView->insertRow(row);

    ui->mediaView->setItem(row, COL_TRACK,
                           new QTableWidgetItem(QString::number(mediaFile.trackNumber)));

    ui->mediaView->setItem(row, COL_TITLE,
                           new QTableWidgetItem(mediaFile.title));

    ui->mediaView->setItem(row, COL_ARTIST,
                           new QTableWidgetItem(mediaFile.artist));

    ui->mediaView->setItem(row, COL_ALBUM,
                           new QTableWidgetItem(mediaFile.album));

    ui->mediaView->setItem(row, COL_DURATION,
                           new QTableWidgetItem(mediaFile.duration));

    ui->mediaView->setItem(row, COL_FORMAT,
                           new QTableWidgetItem(mediaFile.format));

    ui->mediaView->setItem(row, COL_PATH,
                           new QTableWidgetItem(mediaFile.filePath));

    ui->mediaView->item(row, COL_TRACK)->setData(Qt::UserRole, mediaFile.trackNumber);
}

void MediaViewWidget::clearView() {
    ui->mediaView->setRowCount(0);
}

QStringList MediaViewWidget::getSupportedAudioFiles(const QString &folderPath) {
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

QStringList MediaViewWidget::getSelectedFiles() const {
    QStringList selectedFiles;
    QList<QTableWidgetItem*> selectedItems = ui->mediaView->selectedItems();
    QSet<int> selectedRows;

    for (QTableWidgetItem *item : selectedItems)
    {
        selectedRows.insert(item->row());
    }

    for (int row : selectedRows)
    {
        QTableWidgetItem *pathItem = ui->mediaView->item(row, COL_PATH);
        if (pathItem) {
            selectedFiles.append(pathItem->text());
        }
    }

    return selectedFiles;
}

MediaFile MediaViewWidget::parseMediaFile(const QString &filePath) {
    MediaFile mediaFile;
    mediaFile.filePath = filePath;

    QFileInfo fileInfo(filePath);

    // Basic info from file system
    mediaFile.format = fileInfo.suffix().toUpper();
    mediaFile.fileSize = fileInfo.size();

    // Extract basic info from filename
    QString baseName = fileInfo.baseName();
    mediaFile.title = baseName;

    // Try to parse track number from filename (common format: "01 - Song Name.mp3")
    QRegularExpression trackRegex("^(\\d+)");
    QRegularExpressionMatch match = trackRegex.match(baseName);
    if (match.hasMatch()) {
        mediaFile.trackNumber = match.captured(1).toInt();
        // Remove track number from title
        mediaFile.title = baseName.mid(match.capturedLength()).trimmed();
        if (mediaFile.title.startsWith("-") || mediaFile.title.startsWith(".")) {
            mediaFile.title = mediaFile.title.mid(1).trimmed();
        }
    } else {
        mediaFile.trackNumber = 0;
    }

    // Get parent directory as potential album name
    mediaFile.album = fileInfo.dir().dirName();

    // Placeholder values (replace with actual metadata parsing)
    mediaFile.artist = "Unknown Artist";
    mediaFile.duration = "0:00";

    return mediaFile;

}

void MediaViewWidget::onItemDoubleClicked(int row, int col) {
    Q_UNUSED(col);

    m_curIndex = row;
    ui->mediaView->selectRow(row);

    QTableWidgetItem *pathItem = ui->mediaView->item(row, COL_PATH);

    if (pathItem) {
        emit fileDoubleClicked(pathItem->text());
    }
}

void MediaViewWidget::onSelectionChanged() {
    emit selectionChanged(getSelectedFiles());
}
