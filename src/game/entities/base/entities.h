#pragma once
#include "../../Lanes.h"

class EntityClass {
  public:
    int lane;
    int health;
    int damage;
    float speed;

    float x = 0.0f;
    float y = 0.0f;

    void setLane(int chosenLane) {
        lane = chosenLane;
        x = -0.75f;
        y = laneCenters[lane];
    }

    void move() { x = x + (speed * 0.002f); }
};
