#pragma once //this tells c++ to only read this file once, even if it is included multiple times

#include "../player.h"
class defense_player : public playerClass {
    public:
    defense_player(){
        health = 7;
        cost = 30;
        playerType = DEFENSE;
        damage = 0;
        speed = 0.0f;
    }
};