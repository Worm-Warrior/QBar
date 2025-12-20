#ifndef PLAYLIST_H
#define PLAYLIST_H

#include <QObject>

struct Track {
    QString id;
    QString title;
    QString artist;
    QString album;
    int trackNumber;
    int duration;
    // NOTE:
    // I am thinking we just use this as either a
    // URL or file system path, depending on isRemote
    QString filePath;
    bool isRemote;
    // Might need this for the more complex managing of playlist / shuffle.
    bool beenPlayed;
    Track() : trackNumber(0), duration(0), isRemote(0), beenPlayed(false) {}
};

enum repeatModes {
    REPEAT_OFF = 0,
    REPEAT_PLAYLIST = 1,
    REPEAT_SINGLE = 2,
};

class Playlist : public QObject
{
    Q_OBJECT
public:
    explicit Playlist(QObject *parent = nullptr);

    // Adding
    void addTrack(const Track &track);
    void addTracks(const QList<Track> &tracks);
    void clear();

    // Nav
    Track currentTrack() const;
    Track nextTrack();
    Track prevTrack();
    bool hasNext() const;
    bool hasPrev() const;

    // index stuff
    void setCurrentIndex(int index);
    int currentIndex() const {return curIndex;}
    int count() const;

    // repeat and shuffle
    void setShuffle(bool enabled);
    void setRepeat(int selector);
    bool isShuffleOn() const {return shuffle;}
    int getRepeatMode() const {return repeat;}

    QList<Track> tracks() const;

private:
    QList<Track> trackList;
    int curIndex;
    int shufflePos;
    bool shuffle;
    int repeat;
    QList<int> shuffleOrder;

    void generateShuffleOrder();

signals:
    void trackChanged(const Track &track);
    void playlistChanged();
};

#endif // PLAYLIST_H
