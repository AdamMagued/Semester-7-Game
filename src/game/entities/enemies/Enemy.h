#pragma once

#include "renderer/Renderer.h"

class Enemy {    
  private:             
    int lane;
    float posHorizontal;
    float width;
    float height;
  public:
    Enemy(int laneNumber);
    void draw(Renderer& renderer);
};