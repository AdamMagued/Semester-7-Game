#pragma once

#include "../base/entities.h"

class playerClass : public EntityClass {
  public:
    enum playerRange { MELEE, RANGED, DEFENSE };

    playerRange playerType;

    int cost;
    int maxHealth = 5;
    bool isProtected = false;
    virtual ~playerClass() = default;

    void takeDamage(int amount) {
        if (health <= 0 || isProtected) {
            return;
        }
        health = health - amount;
        if (health <= 0) {
            health = 0;
        }
    }

    float yOffset = 0.0f;
    bool isJumping = false;
    int jumpTimer = 0;
    int jumpCooldown = 0;
    int crouchDelayTimer = 0;

    void jump() {
        if (playerType != DEFENSE && isJumping == false && jumpCooldown <= 0) {
            isJumping = true;
            jumpTimer = 45;
            yOffset = 0.10f;
            crouchDelayTimer = 0;
        }
    }

    void duck() {
        if (playerType != DEFENSE) {
            if (isJumping == true) {

                isJumping = false;
                jumpTimer = 0;
                jumpCooldown = 20;
                yOffset = 0.0f;
                crouchDelayTimer = 8;
            } else if (crouchDelayTimer <= 0) {

                yOffset = -0.05f;
            }
        }
    }

    void stand() {
        if (isJumping == false) {
            yOffset = 0.0f;
            crouchDelayTimer = 0;
        }
    }

    void updateJump() {
        if (isJumping == true) {
            jumpTimer = jumpTimer - 1;

            if (jumpTimer <= 0) {
                isJumping = false;
                yOffset = 0.0f;
                jumpCooldown = 20;
            }
        } else if (jumpCooldown > 0) {
            jumpCooldown = jumpCooldown - 1;
        }

        if (crouchDelayTimer > 0) {
            crouchDelayTimer = crouchDelayTimer - 1;
        }
    }
};
