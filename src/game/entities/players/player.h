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
    int jumpCooldown = 0; // Delay before the player can jump again
    int crouchDelayTimer = 0; // Delay before crouching when landing from mid-air

    void jump() {
        if (playerType != DEFENSE && isJumping == false && jumpCooldown <= 0) {
            isJumping = true;
            jumpTimer = 45; // Hang time in air (about 0.75 seconds)
            yOffset = 0.14f; // Hop up in the air
            crouchDelayTimer = 0;
        }
    }

    void duck() {
        if (playerType != DEFENSE) {
            if (isJumping == true) {
                // If mid-air, drop to standing normally first
                isJumping = false;
                jumpTimer = 0;
                jumpCooldown = 20;
                yOffset = 0.0f; // Land to standing normally first
                crouchDelayTimer = 8; // Short delay before crouching (about 0.13 seconds)
            } else if (crouchDelayTimer <= 0) {
                // Grounded and delay passed: crouch down low
                yOffset = -0.06f;
            }
        }
    }

    void stand() {
        if (isJumping == false) {
            yOffset = 0.0f; // Normal standing position
            crouchDelayTimer = 0;
        }
    }

    void updateJump() {
        if (isJumping == true) {
            jumpTimer = jumpTimer - 1; // Count down the air time

            if (jumpTimer <= 0) {
                isJumping = false; // Landed on floor!
                yOffset = 0.0f;
                jumpCooldown = 20; // Short ground delay before next jump (about 0.3 seconds)
            }
        } else if (jumpCooldown > 0) {
            jumpCooldown = jumpCooldown - 1; // Count down the ground delay
        }

        if (crouchDelayTimer > 0) {
            crouchDelayTimer = crouchDelayTimer - 1;
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