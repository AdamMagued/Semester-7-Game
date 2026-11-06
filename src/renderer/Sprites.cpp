#include "Sprites.h"
void drawSprites(const std::vector<playerClass *> &players, const std::vector<Enemy> &enemies,
                 const std::vector<Projectile> &enemyProjectiles, int gameplayTicks, int gameWidth,
                 int gameHeight, int rangedCycleFrames, const GLuint playerAnimationTextures[3][4],
                 const int playerAnimationFrames[3][4], const GLuint robotEnemyTextures[3],
                 const GLuint originalEnemyTextures[7], GLuint mechWalkTexture, GLuint mechAttackTexture,
                 GLuint spearTexture, const SpriteBuffers &buffers) {
    for (size_t i = 0; i < players.size(); i++) {
        if (players[i]->health <= 0)
            continue;
        int type = static_cast<int>(players[i]->playerType);
        int animation = players[i]->playerType == playerClass::DEFENSE ? 0 : (players[i]->isJumping ? 2 : 1);
        if (players[i]->playerType == playerClass::MELEE) {
            melee_player *melee = static_cast<melee_player *>(players[i]);
            if (melee->attackCooldown > 0)
                animation = 3;
        }
        if (players[i]->playerType == playerClass::RANGED &&
            static_cast<ranged_player *>(players[i])->attackAnimationTimer > 0)
            animation = 3;
        GLuint texture = playerAnimationTextures[type][animation];
        int frameCount = playerAnimationFrames[type][animation];
        if (!texture || frameCount == 0)
            continue;

        int frame = (gameplayTicks * 8 / 60) % frameCount;

        float u0 = (frame + 20.0f / 128.0f) / frameCount;
        float u1 = (frame + 108.0f / 128.0f) / frameCount;
        float vTop = 1.0f - 24.0f / 128.0f;
        float halfWidth = 0.15f * 88.0f / 104.0f * gameHeight / gameWidth;
        float left = players[i]->x - halfWidth;
        float right = players[i]->x + halfWidth;
        float top = players[i]->y + players[i]->yOffset + 0.15f;
        float bottom = players[i]->y + players[i]->yOffset - 0.15f;
        if (players[i]->yOffset < 0)
            bottom = players[i]->y - 0.15f;
        const float quad[] = {left, top, u0, vTop, right, top,    u1, vTop, right, bottom, u1, 0,
                              left, top, u0, vTop, right, bottom, u1, 0,    left,  bottom, u0, 0};
        glUseProgram(buffers.playerProgram);
        glUniform1f(buffers.damageLocation, getDamageAmount(players[i]->health, players[i]->maxHealth));
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);
        glUniform1i(glGetUniformLocation(buffers.playerProgram, "playerImage"), 0);
        glBindBuffer(GL_ARRAY_BUFFER, buffers.playerVBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(quad), quad);
        glBindVertexArray(buffers.playerVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }

    for (size_t j = 0; j < enemies.size(); j++) {
        if (enemies[j].active) {
            int shotFrame = enemies[j].attackTimer % rangedCycleFrames;
            bool firing =
                enemies[j].type != HARD && enemies[j].attackTimer >= rangedCycleFrames && shotFrame < 30;
            bool useAdventurer = enemies[j].type == MEDIUM;
            bool useMech = enemies[j].type == HARD && mechWalkTexture && mechAttackTexture;
            int animation = firing ? 2 : 1;
            int frameCount[] = {2, 8, 5};
            int frame = firing ? shotFrame / 6 : (enemies[j].attackTimer / 8) % frameCount[animation];
            GLuint enemyTexture = 0;
            float u0, u1;
            float quadLeft = enemies[j].x - 0.07f;
            float quadRight = enemies[j].x + 0.07f;
            float quadTop = enemies[j].y + 0.105f;
            float quadBottom = enemies[j].y - 0.105f;
            if (useMech) {
                bool fighting = enemies[j].isFighting;
                enemyTexture = fighting ? mechAttackTexture : mechWalkTexture;
                int mechFrames = fighting ? 7 : 8;
                int mechFrame =
                    fighting ? (enemies[j].attackTimer % Enemy::meleeAttackFrames) * 7 / Enemy::meleeAttackFrames : (enemies[j].attackTimer / 6) % 8;
                u0 = static_cast<float>(mechFrame) / mechFrames;
                u1 = static_cast<float>(mechFrame + 1) / mechFrames;

                float pixelWidth = 0.21f * 9.0f / 16.0f / 32.0f;
                float frameWidth = fighting ? 48.0f : 32.0f;
                float bodyCentre = fighting ? 10.5f : 13.5f;
                quadRight = enemies[j].x + bodyCentre * pixelWidth;
                quadLeft = quadRight - frameWidth * pixelWidth;
            } else if (useAdventurer) {
                enemyTexture = robotEnemyTextures[animation];
                u0 = static_cast<float>(frame) / frameCount[animation];
                u1 = static_cast<float>(frame + 1) / frameCount[animation];
            } else {
                int oldFrame = (enemies[j].attackTimer / 50) % 3;
                if (oldFrame == 2)
                    oldFrame = 6;
                if (firing)
                    oldFrame = shotFrame < 15 ? 4 : 5;
                enemyTexture = originalEnemyTextures[oldFrame];
                u0 = 0.0f;
                u1 = 1.0f;
            }
            if (enemyTexture) {
                const float quad[] = {quadLeft, quadTop, u1,        1.0f,       quadRight, quadTop,
                                      u0,       1.0f,    quadRight, quadBottom, u0,        0.0f,

                                      quadLeft, quadTop, u1,        1.0f,       quadRight, quadBottom,
                                      u0,       0.0f,    quadLeft,  quadBottom, u1,        0.0f};
                glUseProgram(buffers.playerProgram);
                glUniform1f(buffers.damageLocation, getDamageAmount(enemies[j].health, enemies[j].maxHealth));
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, enemyTexture);
                glUniform1i(glGetUniformLocation(buffers.playerProgram, "playerImage"), 0);
                glBindBuffer(GL_ARRAY_BUFFER, buffers.playerVBO);
                glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(quad), quad);
                glBindVertexArray(buffers.playerVAO);
                glDrawArrays(GL_TRIANGLES, 0, 6);
            }
        }
    }

    if (spearTexture) {
        glUseProgram(buffers.playerProgram);
        glUniform1f(buffers.damageLocation, 0.0f);
        glUniform1f(buffers.brightnessLocation, 1.0f);
        for (size_t k = 0; k < enemyProjectiles.size(); k++) {
            if (enemyProjectiles[k].active && !enemyProjectiles[k].isLow) {
                const float left = enemyProjectiles[k].x - 0.045f;
                const float right = enemyProjectiles[k].x + 0.045f;
                const float top = enemyProjectiles[k].y + 0.008f;
                const float bottom = enemyProjectiles[k].y - 0.008f;

                const float quad[] = {
                    left, top, 1.0f, 1.0f, right, top,    0.0f, 1.0f, right, bottom, 0.0f, 0.0f,

                    left, top, 1.0f, 1.0f, right, bottom, 0.0f, 0.0f, left,  bottom, 1.0f, 0.0f};
                glUseProgram(buffers.playerProgram);
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, spearTexture);
                glUniform1i(glGetUniformLocation(buffers.playerProgram, "playerImage"), 0);
                glBindBuffer(GL_ARRAY_BUFFER, buffers.playerVBO);
                glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(quad), quad);
                glBindVertexArray(buffers.playerVAO);
                glDrawArrays(GL_TRIANGLES, 0, 6);
            }
        }
    }
}
