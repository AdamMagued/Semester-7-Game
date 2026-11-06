#include "Objects.h"
#include "Drawing.h"
void drawObjects(std::vector<float> &vertices, const std::vector<playerClass *> &players,
                 const std::vector<Projectile> &projectiles, const std::vector<Collectible> &collectibles) {
    for (size_t i = 0; i < players.size(); ++i) {
        if (players[i]->health <= 0 || !players[i]->isProtected)
            continue;
        float drawY = players[i]->y + players[i]->yOffset;

        addRectangle(vertices, players[i]->x + 0.05f, drawY + 0.07f, players[i]->x + 0.06f, drawY - 0.07f,
                     0.2f, 0.5f, 1.0f);
    }
    for (size_t i = 0; i < projectiles.size(); ++i) {
        if (!projectiles[i].active)
            continue;

        addRectangle(vertices, projectiles[i].x - 0.02f, projectiles[i].y + 0.015f, projectiles[i].x + 0.02f,
                     projectiles[i].y - 0.015f, 0.9f, 0.50f, 0.05f);
        addRectangle(vertices, projectiles[i].x - 0.012f, projectiles[i].y + 0.006f,
                     projectiles[i].x + 0.014f, projectiles[i].y - 0.006f, 1, 1, 0.75f);
        addTriangle(vertices, projectiles[i].x + 0.014f, projectiles[i].y + 0.015f, projectiles[i].x + 0.03f,
                    projectiles[i].y, projectiles[i].x + 0.014f, projectiles[i].y - 0.015f, 1, 0.85f, 0.1f);
    }
    for (size_t i = 0; i < collectibles.size(); ++i) {
        if (!collectibles[i].active)
            continue;

        float cx = collectibles[i].x;
        float cy = collectibles[i].y;
        addRectangle(vertices, cx - 0.016f, cy + 0.025f, cx + 0.016f, cy - 0.025f, 0.65f, 0.32f, 0.04f);
        addRectangle(vertices, cx - 0.022f, cy + 0.015f, cx + 0.022f, cy - 0.015f, 0.65f, 0.32f, 0.04f);
        addRectangle(vertices, cx - 0.012f, cy + 0.019f, cx + 0.012f, cy - 0.019f, 1.0f, 0.78f, 0.12f);
        addRectangle(vertices, cx - 0.005f, cy + 0.012f, cx + 0.002f, cy - 0.012f, 1.0f, 0.97f, 0.65f);
    }
}
