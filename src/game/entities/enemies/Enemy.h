#pragma once

#include "renderer/Renderer.h"

enum EnemyType {
    EASY,
    MEDIUM,
    HARD
};

class Enemy {
  private:
    EnemyType type;
    int lane;
    float posHorizontal;
    float width;
    float height;
    float speed;
    int health;
    int damage;
    glm::vec4 color;
  public:
    Enemy(int laneNumber, EnemyType enemyType);
    void update(float dt);
    void draw(Renderer& renderer);
};
