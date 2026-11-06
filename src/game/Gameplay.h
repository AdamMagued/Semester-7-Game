#pragma once
#include <vector>
#include "GameTypes.h"
#include "entities/players/melee/melee_player.h"
#include "entities/players/ranged/ranged_player.h"
#include "entities/players/defense/defense_player.h"
#include "entities/enemies/Enemy.h"

bool hasActiveEnemies(const std::vector<Enemy> &);
float getDamageAmount(int, int);
bool hasDefenseInFront(playerClass *, const std::vector<playerClass *> &);
void updatePlayers(std::vector<playerClass *> &players, std::vector<Enemy> &enemies,
                   std::vector<Projectile> &projectiles, std::vector<Collectible> &collectibles, bool playing,
                   int gameWidth, int gameHeight, float contactDistance);
void updatePlayerShots(std::vector<Projectile> &projectiles, std::vector<Enemy> &enemies,
                       std::vector<Collectible> &collectibles, std::vector<Impact> &impacts, bool playing);
void updateCoins(std::vector<Collectible> &collectibles, const std::vector<playerClass *> &players,
                 int &coins, bool playing);
void updateEnemies(std::vector<Enemy> &enemies, std::vector<playerClass *> &players,
                   std::vector<Projectile> &enemyProjectiles, bool playing, bool &isGameOver,
                   float contactDistance, int rangedCycleFrames);
void updateEnemyShots(std::vector<Projectile> &enemyProjectiles, std::vector<playerClass *> &players,
                      std::vector<Impact> &impacts, bool playing);
