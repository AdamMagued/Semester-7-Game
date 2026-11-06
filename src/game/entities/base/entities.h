#pragma once //this tells c++ to only read this file once, even if it is included multiple times

class EntityClass{
    public:

    int lane;
    int health;
    int damage;
    float speed;

    float x = 0.0f;
    float y = 0.0f;
    float width = 0.08f;
    float height = 0.08f;

    void setLane(int chosenLane) {
        lane = chosenLane;
        x = -0.75f; // Leftmost position of the lane
        if (lane == 0) {
            y = -0.243f;     // Top roadway lane
        } else if (lane == 1) {
            y = -0.531f;     // Middle roadway lane
        } else if (lane == 2) {
            y = -0.842f;     // Bottom roadway lane
        }
    }

    void move() {
        x = x + (speed * 0.002f);
    }


};
