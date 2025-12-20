#include "playlist.h"
#include <QRandomGenerator>

Playlist::Playlist(QObject *parent)
    : QObject(parent),
    curIndex(-1),
    shufflePos(-1),
    shuffle(false),
    repeat(REPEAT_OFF)
{
}

// IDK if this will be used for what I want right now, but it should be here
// anyway.
void Playlist::addTrack(const Track &track)
{
    trackList.append(track);
    emit playlistChanged();
}

void Playlist::addTracks(const QList<Track> &tracks)
{
    trackList.append(tracks);
    if (shuffle) {
        generateShuffleOrder();
    }
    emit playlistChanged();
}

void Playlist::clear() {
    trackList.clear();
    curIndex = -1;
    shuffleOrder.clear();
    emit playlistChanged();
}

Track Playlist::currentTrack() const {
    return trackList[curIndex];
}

Track Playlist::nextTrack() {
    if (trackList.isEmpty()) {
        return Track();
    }

    if (shuffle) {
        if (repeat == REPEAT_SINGLE) {
            return currentTrack();
        }

        shufflePos++;

        if (shufflePos >= shuffleOrder.count()) {
            if (repeat == REPEAT_PLAYLIST) {
                shufflePos = 0;
            } else {
                return Track();  // End of playlist
            }
        }

        curIndex = shuffleOrder[shufflePos];
    } else {
        // Normal logic
        if (curIndex < trackList.count() - 1) {
            curIndex++;
        } else if (repeat == REPEAT_SINGLE) {
            return currentTrack();
        } else if (repeat == REPEAT_PLAYLIST) {
            curIndex = 0;
        } else {
            return Track();
        }
    }

    Track track = currentTrack();
    emit trackChanged(track);
    return track;
}

Track Playlist::prevTrack() {
    if (trackList.isEmpty()) {
        return Track();
    }

    if (shuffle) {
        if (repeat == REPEAT_SINGLE) {
            return currentTrack();
        }

        shufflePos--;

        if (shufflePos< 0) {
            if (repeat == REPEAT_PLAYLIST) {
                shufflePos= shuffleOrder.count() - 1;
            } else {
                return Track();
            }
        }

        curIndex = shuffleOrder[shufflePos];
    } else {
        // Normal logic
        if (curIndex > 0) {
            curIndex--;
        } else if (repeat == REPEAT_SINGLE) {
            return currentTrack();
        } else if (repeat == REPEAT_PLAYLIST) {
            curIndex = trackList.count() - 1;
        } else {
            return Track();
        }
    }

    Track track = currentTrack();
    emit trackChanged(track);
    return track;
}

bool Playlist::hasNext() const {
    if (repeat == REPEAT_SINGLE || repeat == REPEAT_PLAYLIST) {
        return true;
    }

    if (shuffle) {
        return shufflePos< shuffleOrder.count() - 1;
    } else {
        return curIndex < trackList.count() - 1;
    }
}

bool Playlist::hasPrev() const {
    if (repeat == REPEAT_SINGLE || repeat == REPEAT_PLAYLIST) {
        return true;
    }

    if (shuffle) {
        return shufflePos> 0;
    } else {
        return curIndex > 0;
    }
}

void Playlist::setCurrentIndex(int index) {
    if (index >= 0 && index < trackList.count()) {
        curIndex = index;

        if (shuffle && !shuffleOrder.isEmpty()) {
            // Find this track's position in shuffle order
            shufflePos= shuffleOrder.indexOf(index);
            if (shufflePos== -1) {
                shufflePos= 0;
            }
        }

        emit trackChanged(currentTrack());
    }
}

void Playlist::setShuffle(bool enabled) {
    shuffle = enabled;
    if (enabled && !trackList.isEmpty()) {
        generateShuffleOrder();
    }
}

void Playlist::setRepeat(int selector) {
    if (selector != REPEAT_OFF && selector != REPEAT_SINGLE && selector != REPEAT_PLAYLIST) {
        return;
    } else {
        repeat = selector;
    }
}

void Playlist::generateShuffleOrder() {
    shuffleOrder.clear();

    for (int i = 0; i < trackList.count(); i++) {
        shuffleOrder.append(i);
    }

    // Fisher-Yates
    for (int i = shuffleOrder.count()-1; i > 0; i--) {
        int j = QRandomGenerator::global()->bounded(i+1);
        shuffleOrder.swapItemsAt(i, j);
    }
}

QList<Track> Playlist::tracks() const {
    return trackList;
}
