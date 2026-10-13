#pragma once //this tells c++ to only read this file once, even if it is included multiple times

class EntityClass{
    public:

    int lane;
    int health;
    int damage;
    float speed;

    enum healthState{
        HEALTHY,
        INJURED,
        HEAVILY_INJURED,
        CRITICAL,
        DEAD
    
    };


};
