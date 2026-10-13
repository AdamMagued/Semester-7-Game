#pragma once //this tells c++ to only read this file once, even if it is included multiple times

#include "../base/entities.h"

class playerClass: public EntityClass{
    public:
    enum playerRange{
        MELEE,
        RANGED,
        DEFENSE
    };

    enum playerState{
        vulnerable,
        invulnerable,
        defeated
    };

    playerRange playerType;
    playerState playerStatus;

    int playerCost;
    int playerDurability; //how many hits the player can take before defeat




};