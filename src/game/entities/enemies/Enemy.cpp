#include "Enemy.h"
#include "core/Constants.h"

Enemy::Enemy(int laneNumber){
    posHorizontal = 16;
    lane = laneNumber;
    width = 1;
    height = 1.2f;
};

void Enemy::draw(Renderer& renderer){
    float laneYcenter = topOfPlayfield + (lane*laneHeight) - (laneHeight/2);
    renderer.drawTriangle(posHorizontal, laneYcenter, width, height, glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
};