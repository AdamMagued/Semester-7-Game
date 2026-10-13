#pragma once //this tells c++ to only read this file once, even if it is included multiple times

#include "../base/entities.h"
#include <fightStrat.h>

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

<<<<<<< HEAD
    int playerCost;
    int playerDurability; //how many hits the player can take before defeat

    fightStrat* strat;
=======
    int cost; 
   
>>>>>>> 7faf45451a060ba6f524cbd37b77f5165fc5cda1

    void setStrat(fightStrat* s) {
        strat = s;
    }

    void doAttack() {
        if (strat != nullptr) {
            strat->attack(damage);
        }
    }
    


};