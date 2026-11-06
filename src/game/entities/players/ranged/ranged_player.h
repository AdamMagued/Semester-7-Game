#pragma once
#include "../player.h"

class ranged_player : public playerClass {
  public:
    int shootTimer = 0;
    int attackAnimationTimer = 0;
    int shootCooldown = 120;

    ranged_player() {
        health = 5;
        cost = 20;
        playerType = RANGED;
        damage = 1;
        speed = 1.0f;
    }

    bool shouldShoot() {
        if (attackAnimationTimer > 0)
            --attackAnimationTimer;
        shootTimer = shootTimer + 1;
        if (shootTimer >= shootCooldown) {
            shootTimer = 0;
            attackAnimationTimer = 12;
            return true;
        }
        return false;
    }
};
