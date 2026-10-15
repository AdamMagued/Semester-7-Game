#include "Enemy.h"
#include "core/Constants.h"

Enemy::Enemy(int laneNumber, EnemyType enemyType){
    type = enemyType;
    lane = laneNumber;
    posHorizontal = 16;

    // Default values, the types below change what they need
    width = 1;
    height = 1.2f;
    speed = 1.0f;
    health = 5;
    damage = 1;
    color = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);

    if (type == MEDIUM) {
        color = glm::vec4(1.0f, 0.5f, 0.0f, 1.0f);
    }
    if (type == HARD) {
        width = 1.2f;
        height = 1.4f;
        speed = 2.5f;
        health = 7;
        damage = 2;
        color = glm::vec4(0.6f, 0.0f, 0.0f, 1.0f);
    }
};

void Enemy::update(float dt){
    posHorizontal = posHorizontal - speed * dt;
}

void Enemy::draw(Renderer& renderer){
    float laneYcenter = topOfPlayfield + (lane*laneHeight) - (laneHeight/2);
    renderer.drawTriangle(posHorizontal, laneYcenter, width, height, color);
};
