#include "Hud.h"
#include "../renderer/Drawing.h"
void drawCards(std::vector<float> &vertices, int selectedPlayerType, int hoveredCard, int coins,
               int currentWave) {
    bool rangedUnlocked = currentWave >= 2;
    bool defenseUnlocked = currentWave >= 2;

    if ((selectedPlayerType == 0 || hoveredCard == 0) && coins >= 10) {
        addRectangle(vertices, -0.76f, 0.93f, -0.54f, 0.72f, 1.0f, 1.0f, 1.0f);
    }
    if (coins >= 10) {
        addRectangle(vertices, -0.75f, 0.92f, -0.55f, 0.73f, 0.85f, 0.25f, 0.25f);
        drawText(vertices, "COST 10", -0.70f, 0.835f, 0.0035f, 1.0f, 1.0f, 1.0f);
    } else {
        addRectangle(vertices, -0.75f, 0.92f, -0.55f, 0.73f, 0.40f, 0.40f, 0.40f);
        drawText(vertices, "COST 10", -0.70f, 0.835f, 0.0035f, 0.75f, 0.75f, 0.75f);
    }
    drawText(vertices, "MELEE", -0.68f, 0.695f, 0.0035f, 1.0f, 1.0f, 1.0f);

    if ((selectedPlayerType == 1 || hoveredCard == 1) && rangedUnlocked && coins >= 20) {
        addRectangle(vertices, -0.51f, 0.93f, -0.29f, 0.72f, 1.0f, 1.0f, 1.0f);
    }
    if (rangedUnlocked && coins >= 20) {
        addRectangle(vertices, -0.50f, 0.92f, -0.30f, 0.73f, 0.25f, 0.45f, 0.85f);
        drawText(vertices, "COST 20", -0.45f, 0.835f, 0.0035f, 1.0f, 1.0f, 1.0f);
    } else {
        addRectangle(vertices, -0.50f, 0.92f, -0.30f, 0.73f, 0.40f, 0.40f, 0.40f);
        drawText(vertices, rangedUnlocked ? "COST 20" : "WAVE 2", -0.45f, 0.835f, 0.0035f, 0.75f, 0.75f,
                 0.75f);
    }
    drawText(vertices, "RANGED", -0.44f, 0.695f, 0.0035f, 1.0f, 1.0f, 1.0f);

    if ((selectedPlayerType == 2 || hoveredCard == 2) && defenseUnlocked && coins >= 30) {
        addRectangle(vertices, -0.26f, 0.93f, -0.04f, 0.72f, 1.0f, 1.0f, 1.0f);
    }
    if (defenseUnlocked && coins >= 30) {
        addRectangle(vertices, -0.25f, 0.92f, -0.05f, 0.73f, 0.85f, 0.65f, 0.20f);
        drawText(vertices, "COST 30", -0.20f, 0.835f, 0.0035f, 1.0f, 1.0f, 1.0f);
    } else {
        addRectangle(vertices, -0.25f, 0.92f, -0.05f, 0.73f, 0.40f, 0.40f, 0.40f);
        drawText(vertices, defenseUnlocked ? "COST 30" : "WAVE 2", -0.20f, 0.835f, 0.0035f, 0.75f, 0.75f,
                 0.75f);
    }
    drawText(vertices, "DEFENSE", -0.20f, 0.695f, 0.0035f, 1.0f, 1.0f, 1.0f);

    std::string coinStr = "COINS: " + std::to_string(coins);
    drawText(vertices, coinStr, 0.36f, 0.85f, 0.0035f, 1.0f, 0.9f, 0.1f);
}
