#pragma once
#include "../../Lanes.h"
#include "../base/entities.h"

enum EnemyType { EASY, MEDIUM, HARD };

class Enemy : public EntityClass {
  public:
    static constexpr int meleeAttackFrames = 90;
    EnemyType type;
    int maxHealth = 5;
    int attackTimer = 0;
    bool active = true;
    bool isFighting = false;

    Enemy(int laneNumber, EnemyType enemyType = EASY) {
        lane = laneNumber;
        type = enemyType;
        active = true;
        x = 0.90f;
        speed = 0.0006f;

        y = laneCenters[lane];

        if (type == EASY) {
            health = 5;
            damage = 1;
        } else if (type == MEDIUM) {
            health = 5;
            damage = 1;
        } else if (type == HARD) {
            health = 7;
            maxHealth = 7;
            damage = 2;
            speed = 0.0009f;
        }
    }

    void moveLeft() {
        if (active == true) {
            x = x - speed;
            if (x < -0.85f) {
                x = -0.85f;
            }
        }
    }
};
