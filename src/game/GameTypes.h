#pragma once

struct Projectile {
    float x;
    float y;
    int lane;
    float speed;
    bool active;
    bool isLow = false;
};

struct Impact {
    float x;
    float y;
    int ticks;
};

struct Collectible {
    float x;
    float y;
    int lane;
    int value;
    bool active;
};
