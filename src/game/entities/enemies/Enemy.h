#pragma once
#include "../base/entities.h"

enum EnemyType {
    EASY,
    MEDIUM,
    HARD
};

class Enemy : public EntityClass {
public:
    EnemyType type;
    bool active = true;

    Enemy(int laneNumber, EnemyType enemyType = EASY) {
        lane = laneNumber;
        type = enemyType;
        active = true;
        x = 0.90f;        // Start on the right side of the lane
        speed = 0.0012f;  // Moves left towards the house base

        if (lane == 0) {
            y = 0.41f;    // Top lane
        } else if (lane == 1) {
            y = -0.04f;   // Middle lane
        } else if (lane == 2) {
            y = -0.55f;   // Bottom lane
        }

        if (type == EASY) {
            health = 5;
            damage = 1;
        } else if (type == MEDIUM) {
            health = 5;
            damage = 1;
        } else if (type == HARD) {
            health = 7;
            damage = 2;
            speed = 0.0018f;
        }
    }

    void moveLeft() {
        if (active == true) {
            x = x - speed;
            if (x < -0.85f) {
                x = -0.85f; // Stop at the house
            }
        }
    }
};