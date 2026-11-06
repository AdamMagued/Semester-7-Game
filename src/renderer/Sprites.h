#pragma once
#include "Drawing.h"
#include "../game/Gameplay.h"
struct SpriteBuffers {
    GLuint playerProgram;
    GLint damageLocation;
    GLint brightnessLocation;
    GLuint playerVBO;
    GLuint playerVAO;
};
void drawSprites(const std::vector<playerClass *> &players, const std::vector<Enemy> &enemies,
                 const std::vector<Projectile> &enemyProjectiles, int gameplayTicks, int gameWidth,
                 int gameHeight, int rangedCycleFrames, const GLuint playerAnimationTextures[3][4],
                 const int playerAnimationFrames[3][4], const GLuint robotEnemyTextures[3],
                 const GLuint originalEnemyTextures[7], GLuint mechWalkTexture, GLuint mechAttackTexture,
                 GLuint spearTexture, const SpriteBuffers &buffers);
