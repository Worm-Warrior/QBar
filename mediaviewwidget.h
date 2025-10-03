#ifndef MEDIAVIEWWIDGET_H
#define MEDIAVIEWWIDGET_H

#include <QWidget>
#include <qboxlayout.h>
#include <qtablewidget.h>

struct MediaFile {
    QString filePath;
    QString title;
    QString artist;
    QString album;
    QString duration;
    QString format;
    int trackNumber;
    qint64 fileSize;
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

    void displayFolder(const QString &folderPath);

    void addMediaFile(const MediaFile &mediaFile);

    void clearView();

    QStringList getSelectedFiles() const;
private slots:
    void onItemDoubleClicked(int row, int column);
    void onSelectionChanged();

private:
    Ui::MediaViewWidget *ui;
    void setupTableCols();
    MediaFile parseMediaFile(const QString &filePath);
    QStringList getSupportedAudioFiles(const QString &folderPath);

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

signals:
    void fileDoubleClicked(const QString &filePath);
    void selectionChanged(const QStringList &selectedFiles);
};

#endif // MEDIAVIEWWIDGET_H
