//
// Created by 刘帅帅 on 2019/12/10.
//

#ifndef VIDEOPLAYER_PLAYERSTATUS_H
#define VIDEOPLAYER_PLAYERSTATUS_H

#include "PlayerListener.h"

class PlayerStatus {
public:
    bool exit;
    bool load;
    bool seek;
    PlayerListener *playerListner;
public:
    PlayerStatus();

    ~PlayerStatus();

    void setPlayerListener(PlayerListener *playerListener);
};

inline PlayerStatus::PlayerStatus() {
    exit = false;
    load = true;
    seek = false;
    playerListner = NULL;
}

inline void PlayerStatus::setPlayerListener(PlayerListener *playerListener) {
    this->playerListner = playerListener;
}

inline PlayerStatus::~PlayerStatus() {
    playerListner = NULL;
}


#endif //VIDEOPLAYER_PLAYERSTATUS_H
