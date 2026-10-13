#pragma once //this tells c++ to only read this file once, even if it is included multiple times
#include "../player.h"

class ranged_player : public playerClass {
    public:
    ranged_player(){
        health = 5;
        cost = 20;
        playerType = RANGED;
        damage = 1;
        speed = 1.0f;
    }
};