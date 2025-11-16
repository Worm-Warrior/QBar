#include "playlist.h"
#include <QRandomGenerator>

Playlist::Playlist(QObject *parent)
    : QObject(parent),
    curIndex(-1),
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
        int curShuffle = shuffleOrder.indexOf(curIndex);
        if (curShuffle < shuffleOrder.count()-1) {
            curIndex = shuffleOrder[curShuffle+1];
        } else if (repeat == REPEAT_SINGLE) {
            return currentTrack();
        } else if (repeat == REPEAT_PLAYLIST) {
            curIndex = shuffleOrder[0];
        } else {
            return Track();
        }
    } else {
        if (curIndex < trackList.count()-1) {
            curIndex++;
        } else if (REPEAT_SINGLE) {
            // if we are repeating a single song we just get current one back.
            return currentTrack();
        } else if (REPEAT_PLAYLIST) {
            // reset the playlist.
            curIndex = 0;
        }
    }

    Track track = currentTrack();
    emit trackChanged(track);
    return track;
}

Track Playlist::prevTrack() {
    if (trackList.isEmpty()) {return Track();}

    if (shuffle) {
        int curShuffle = shuffleOrder.indexOf(curIndex);

        if (curShuffle > 0) {
            curIndex = shuffleOrder[curShuffle-1];
        } else if (repeat == REPEAT_SINGLE) {
            return currentTrack();
        } else if (repeat == REPEAT_PLAYLIST) {
            curIndex = shuffleOrder.last();
        } else {
            return Track();
        }
    } else {
        if (curIndex > 0) {
            curIndex--;
        } else if (repeat == REPEAT_SINGLE) {
            return currentTrack();
        } else if (repeat == REPEAT_PLAYLIST) {
            curIndex = trackList.count()-1;
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
    } else {
        return curIndex < trackList.count()-1;
    }
}

bool Playlist::hasPrev() const {
    if (repeat == REPEAT_SINGLE || repeat == REPEAT_PLAYLIST) {
        return true;
    } else {
        return curIndex > 0;
    }
}

void Playlist::setCurrentIndex(int index) {
    if (index >= 0 && index < trackList.count()-1) {
        curIndex = index;
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
    if (selector != REPEAT_OFF || selector != REPEAT_SINGLE || selector != REPEAT_PLAYLIST) {
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
