#include "Gameplay.h"
#include <algorithm>
#include <cmath>

bool hasActiveEnemies(const std::vector<Enemy> &enemies) {
    for (size_t i = 0; i < enemies.size(); i++) {
        if (enemies[i].active) {
            return true;
        }
    }
    return false;
}

float getDamageAmount(int health, int maxHealth) {
    float damage = 1.0f - (float)health / (float)maxHealth;
    if (damage < 0.0f)
        damage = 0.0f;
    if (damage > 1.0f)
        damage = 1.0f;
    return damage;
}

bool hasDefenseInFront(playerClass *player, const std::vector<playerClass *> &players) {
    if (player->health <= 0 || player->playerType == playerClass::DEFENSE) {
        return false;
    }
    for (size_t i = 0; i < players.size(); i++) {
        if (players[i]->health > 0 && players[i]->playerType == playerClass::DEFENSE &&
            players[i]->lane == player->lane && players[i]->x > player->x) {
            return true;
        }
    }
    return false;
}

static void dropCoin(Enemy &enemy, std::vector<Collectible> &collectibles) {
    enemy.active = false;
    collectibles.push_back({enemy.x, enemy.y, enemy.lane, 10, true});
}

void updatePlayers(std::vector<playerClass *> &players, std::vector<Enemy> &enemies,
                   std::vector<Projectile> &projectiles, std::vector<Collectible> &collectibles, bool playing,
                   int gameWidth, int gameHeight, float contactDistance) {

    for (size_t i = 0; i < players.size();) {
        if (players[i]->health <= 0) {
            delete players[i];
            players.erase(players.begin() + i);
            continue;
        }
        bool isPlayerBlocked = false;

        if (playing) {

            for (size_t j = 0; j < enemies.size(); j++) {
                if (enemies[j].active && enemies[j].lane == players[i]->lane) {
                    float dist = enemies[j].x - players[i]->x;
                    if (dist >= 0.0f && dist <= contactDistance) {
                        isPlayerBlocked = true;
                        break;
                    }
                }
            }

            if (players[i]->playerType == playerClass::MELEE) {
                melee_player *mp = static_cast<melee_player *>(players[i]);
                if (mp->attackCooldown > 0) {
                    mp->attackCooldown = mp->attackCooldown - 1;
                }

                for (size_t j = 0; j < enemies.size(); j++) {
                    if (enemies[j].active && enemies[j].lane == mp->lane) {
                        float dist = enemies[j].x - mp->x;
                        if (dist >= 0.0f && dist <= contactDistance) {
                            if (mp->attackCooldown <= 0) {
                                enemies[j].health = enemies[j].health - mp->damage;
                                mp->attackCooldown = 50;

                                if (enemies[j].health <= 0) {
                                    dropCoin(enemies[j], collectibles);
                                }
                            }
                            break;
                        }
                    }
                }
            }

            if (isPlayerBlocked == false) {
                players[i]->move();
            }
            players[i]->updateJump();
            float exitMargin = std::max(0.10f, 0.15f * 88.0f / 104.0f * gameHeight / gameWidth);
            if (players[i]->playerType != playerClass::DEFENSE && players[i]->x > 1.0f + exitMargin) {
                delete players[i];
                players.erase(players.begin() + i);
                continue;
            }
        }

        bool protectedByDefense = hasDefenseInFront(players[i], players);
        players[i]->isProtected = protectedByDefense;

        if (playing && players[i]->playerType == playerClass::RANGED) {
            ranged_player *rp = static_cast<ranged_player *>(players[i]);
            if (rp->shouldShoot()) {
                Projectile proj;
                proj.x = rp->x;
                proj.y = rp->y + rp->yOffset;
                proj.lane = rp->lane;
                proj.speed = 0.008f;
                proj.active = true;
                projectiles.push_back(proj);
            }
        }
        ++i;
    }
}
void updatePlayerShots(std::vector<Projectile> &projectiles, std::vector<Enemy> &enemies,
                       std::vector<Collectible> &collectibles, std::vector<Impact> &impacts, bool playing) {

    for (size_t i = 0; i < projectiles.size(); i++) {
        if (projectiles[i].active == true) {
            if (playing) {
                projectiles[i].x = projectiles[i].x + projectiles[i].speed;

                for (size_t j = 0; j < enemies.size(); j++) {
                    if (enemies[j].active && enemies[j].lane == projectiles[i].lane) {
                        float dist = std::abs(projectiles[i].x - enemies[j].x);
                        if (dist < 0.05f) {
                            projectiles[i].active = false;
                            impacts.push_back({projectiles[i].x, projectiles[i].y, 8});
                            enemies[j].health = enemies[j].health - 1;
                            if (enemies[j].health <= 0) {
                                dropCoin(enemies[j], collectibles);
                            }
                            break;
                        }
                    }
                }

                if (projectiles[i].x > 0.95f) {
                    projectiles[i].active = false;
                }
            }
        }
    }
}
void updateCoins(std::vector<Collectible> &collectibles, const std::vector<playerClass *> &players,
                 int &coins, bool playing) {

    for (size_t i = 0; i < collectibles.size(); i++) {
        if (collectibles[i].active == true) {

            if (playing) {
                for (size_t p = 0; p < players.size(); p++) {
                    if (players[p]->health > 0 && players[p]->lane == collectibles[i].lane) {
                        float dist = std::abs(players[p]->x - collectibles[i].x);
                        if (dist < 0.06f) {
                            coins = coins + collectibles[i].value;
                            collectibles[i].active = false;
                            break;
                        }
                    }
                }
            }
        }
    }
}
void updateEnemies(std::vector<Enemy> &enemies, std::vector<playerClass *> &players,
                   std::vector<Projectile> &enemyProjectiles, bool playing, bool &isGameOver,
                   float contactDistance, int rangedCycleFrames) {

    for (size_t j = 0; j < enemies.size(); j++) {
        if (enemies[j].active == true) {
            if (playing) {
                enemies[j].attackTimer = enemies[j].attackTimer + 1;
                int blockingPlayer = -1;
                float closestDistance = contactDistance;

                for (size_t p = 0; p < players.size(); p++) {
                    if (players[p]->health > 0 && players[p]->lane == enemies[j].lane) {
                        float dist = enemies[j].x - players[p]->x;
                        if (dist >= 0.0f && dist <= closestDistance) {
                            blockingPlayer = static_cast<int>(p);
                            closestDistance = dist;
                        }
                    }
                }
                enemies[j].isFighting = (blockingPlayer != -1);

                if (enemies[j].type == HARD && blockingPlayer != -1 && enemies[j].attackTimer % 50 == 10) {
                    players[blockingPlayer]->isProtected =
                        hasDefenseInFront(players[blockingPlayer], players);
                    players[blockingPlayer]->takeDamage(enemies[j].damage);
                }

                if (blockingPlayer == -1) {
                    enemies[j].moveLeft();
                    if (enemies[j].x <= -0.80f)
                        isGameOver = true;
                }
                if (!isGameOver && enemies[j].type != HARD &&
                    enemies[j].attackTimer % rangedCycleFrames == 0) {
                    Projectile spear;
                    spear.x = enemies[j].x;
                    spear.y = enemies[j].y + 0.035f;
                    if (enemies[j].type == MEDIUM) {
                        spear.isLow = true;
                        spear.y = enemies[j].y - 0.065f;
                    }
                    spear.lane = enemies[j].lane;
                    spear.speed = 0.0075f;
                    spear.active = true;
                    enemyProjectiles.push_back(spear);
                }
            }
        }
    }
}
void updateEnemyShots(std::vector<Projectile> &enemyProjectiles, std::vector<playerClass *> &players,
                      std::vector<Impact> &impacts, bool playing) {

    for (size_t k = 0; k < enemyProjectiles.size(); k++) {
        if (enemyProjectiles[k].active == true) {
            if (playing) {
                enemyProjectiles[k].x = enemyProjectiles[k].x - enemyProjectiles[k].speed;

                int targetPlayer = -1;
                for (size_t p = 0; p < players.size(); p++) {
                    if (players[p]->health > 0 && players[p]->lane == enemyProjectiles[k].lane) {
                        float dist = std::abs(enemyProjectiles[k].x - players[p]->x);
                        if (dist < 0.05f) {
                            if (players[p]->playerType != playerClass::DEFENSE) {
                                float bodyBottom = players[p]->y + players[p]->yOffset - 0.14f;
                                float bodyTop = players[p]->y + players[p]->yOffset + 0.055f;
                                if (players[p]->yOffset < 0) {
                                    bodyBottom = players[p]->y - 0.14f;
                                    bodyTop = players[p]->y - 0.005f;
                                }
                                float shotY = enemyProjectiles[k].y;
                                if (shotY + 0.008f < bodyBottom || shotY - 0.008f > bodyTop) {
                                    continue;
                                }
                            }
                            if (targetPlayer == -1 || players[p]->x > players[targetPlayer]->x) {
                                targetPlayer = static_cast<int>(p);
                            }
                        }
                    }
                }
                if (targetPlayer != -1) {
                    enemyProjectiles[k].active = false;
                    impacts.push_back({enemyProjectiles[k].x, enemyProjectiles[k].y, 8});
                    players[targetPlayer]->isProtected = hasDefenseInFront(players[targetPlayer], players);
                    players[targetPlayer]->takeDamage(1);
                }

                if (enemyProjectiles[k].x < -0.90f) {
                    enemyProjectiles[k].active = false;
                }
            }
        }
    }
}
