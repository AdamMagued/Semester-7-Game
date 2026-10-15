#pragma once //this tells c++ to only read this file once, even if it is included multiple times

#include "../base/entities.h"
#include "../../fightStrat.h"

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

    int cost;
    fightStrat* strat = nullptr;

    float yOffset = 0.0f; // Height adjustment for jump and duck
    bool isJumping = false;
    int jumpTimer = 0;

    void jump() {
        if (playerType != DEFENSE && isJumping == false) {
            isJumping = true;
            jumpTimer = 25; // Hang time in air (about 0.4 seconds)
            yOffset = 0.14f; // Hop up in the air
        }
    }

    void duck() {
        if (playerType != DEFENSE) {
            isJumping = false; // Cancel jump mid-air (fast-drop!)
            jumpTimer = 0;
            yOffset = -0.06f; // Crouch down low immediately
        }
    }

    void stand() {
        if (isJumping == false) {
            yOffset = 0.0f; // Normal standing position
        }
    }

    void updateJump() {
        if (isJumping == true) {
            jumpTimer = jumpTimer - 1; // Count down the air time

            if (jumpTimer <= 0) {
                isJumping = false; // Landed on floor!
                yOffset = 0.0f;
            }
        }
    }

    void setStrat(fightStrat* s) {
        strat = s;
    }

    void doAttack() {
        if (strat != nullptr) {
            strat->attack(damage);
        }
    }
    


};