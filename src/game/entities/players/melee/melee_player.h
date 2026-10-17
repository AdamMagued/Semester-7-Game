#pragma once //this tells c++ to only read this file once, even if it is included multiple times
#include "../player.h" 



class melee_player : public playerClass {
    public:
    int attackCooldown = 0;

    melee_player(){
        health = 5;
        cost = 10;
        playerType = MELEE;
        damage = 1;
        speed = 1.0f;
        strat = new meleeStrat();
    }
};
