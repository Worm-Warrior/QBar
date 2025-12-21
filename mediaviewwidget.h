#ifndef MEDIAVIEWWIDGET_H
#define MEDIAVIEWWIDGET_H

#include <QWidget>
#include "playlist.h"

// Forward declaration
class MainWindow;

struct MediaFile {
    QString filePath;
    QString title;
    QString artist;
    QString album;
    QString duration;
    QString format;
    int trackNumber;
    qint64 fileSize;

    MediaFile() : trackNumber(0), fileSize(0) {}
};

namespace Ui {
class MediaViewWidget;
}

class MediaViewWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MediaViewWidget(QWidget *parent = nullptr);
    ~MediaViewWidget();

    void setMainWindow(MainWindow *mw);
    void displayFolder(const QString &folderPath);

signals:
    void selectionChanged(const QStringList &selectedFiles);

private slots:
    void onItemDoubleClicked(int row, int column);
    void onSelectionChanged();
    void onTableSorted(int index, Qt::SortOrder order);

private:
    Ui::MediaViewWidget *ui;
    MainWindow *mainWindow;
    QList<Track> currentFolderTracks;
    void rebuildPlaylistToUI();

    enum ColIndex {
        COL_TRACK = 0,
        COL_TITLE,
        COL_ARTIST,
        COL_ALBUM,
        COL_DURATION,
        COL_FORMAT,
        COL_PATH,
        COL_COUNT
    };

    void setupTableCols();
    void clearView();
    MediaFile parseMediaFile(const QString &filePath);
    QStringList getSupportedAudioFiles(const QString &folderPath);
    QStringList getSelectedFiles() const;
    Track mediaFileToTrack(const MediaFile &mediaFile);
};

#endif // MEDIAVIEWWIDGET_H
