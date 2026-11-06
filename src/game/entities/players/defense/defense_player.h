#pragma once

#include "../player.h"
class defense_player : public playerClass {
  public:
    defense_player() {
        health = 7;
        maxHealth = 7;
        cost = 30;
        playerType = DEFENSE;
        damage = 0;
        speed = 0.0f;
    }
};
