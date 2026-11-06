#pragma once
#include "../player.h"

class melee_player : public playerClass {
  public:
    int attackCooldown = 0;

    melee_player() {
        health = 5;
        cost = 10;
        playerType = MELEE;
        damage = 1;
        speed = 1.0f;
    }
};
