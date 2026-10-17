#include "Enemy.h"

Enemy::Enemy(int laneNumber){
    posHorizontal = 16;
    lane = laneNumber;
    width = 1;
    height = 1.2;
};

void Enemy::draw(){};