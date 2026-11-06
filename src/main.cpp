#include <algorithm>
#include <cmath>
#include <cstdio>
#include <string>
#define GLAD_GL_IMPLEMENTATION
#include <glad/gl.h>
#undef GLAD_GL_IMPLEMENTATION
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include "../assets/textures/stb_image.h"
#include "game/entities/players/melee/melee_player.h"
#include "game/entities/players/defense/defense_player.h"
#include "game/entities/players/ranged/ranged_player.h"
#include "game/entities/enemies/Enemy.h"
#include "game/conversation_cutscene.h"
#include "game/Gameplay.h"
#include "game/Lanes.h"
#include "game/CutsceneDrawing.h"
#include "renderer/Drawing.h"
#include "renderer/Sprites.h"
#include "renderer/Objects.h"
#include "ui/Hud.h"

int main() {
    if (!glfwInit()) {
        std::cerr << "Failed to init GLFW\n";
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow *window = glfwCreateWindow(1280, 720, "Lane Defense", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create window\n";
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    GLFWcursor *arrowCursor = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);
    if (arrowCursor)
        glfwSetCursor(window, arrowCursor);

    int version = gladLoadGL(glfwGetProcAddress);
    if (version == 0) {
        printf("Failed to initialize OpenGL context\n");
        return -1;
    }

    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
    bool isFullscreen = false;
    bool wasF11Down = false;
    int framebufferWidth;
    int framebufferHeight;
    glfwGetFramebufferSize(window, &framebufferWidth, &framebufferHeight);
    framebufferSizeCallback(window, framebufferWidth, framebufferHeight);

    GLuint shaderProgram = createProgram(vertexShaderSrc, fragmentShaderSrc);
    GLuint playerProgram = createProgram(playerVertexShaderSrc, playerFragmentShaderSrc);
    if (!shaderProgram || !playerProgram) {
        glfwTerminate();
        return -1;
    }
    GLint damageLocation = glGetUniformLocation(playerProgram, "damageAmount");
    GLint brightnessLocation = glGetUniformLocation(playerProgram, "brightness");

    stbi_set_flip_vertically_on_load(true);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    GLuint menuBackgroundTexture = loadTexture("assets/textures/ui/main_menu.jpg");
    GLuint endingBackgroundTexture = loadTexture("assets/textures/ui/ending.jpg");

    const char *playerAnimationFiles[3][4] = {
        {"Gangsters_2/Idle.png", "Gangsters_2/Run.png", "Gangsters_2/Jump.png", "Gangsters_2/Attack_1.png"},
        {"Gangsters_1/Idle.png", "Gangsters_1/Run.png", "Gangsters_1/Jump.png", "Gangsters_1/Shot.png"},
        {"Gangsters_3/Idle.png", "Gangsters_3/Run.png", "Gangsters_3/Jump.png", "Gangsters_3/Attack.png"}};
    GLuint playerAnimationTextures[3][4] = {};
    int playerAnimationFrames[3][4] = {};
    for (int type = 0; type < 3; ++type) {
        for (int animation = 0; animation < 4; ++animation) {
            char path[160];
            snprintf(path, sizeof(path), "assets/textures/characters/gangsters/%s",
                     playerAnimationFiles[type][animation]);
            int width = 0;
            int height = 0;
            playerAnimationTextures[type][animation] = loadTexture(path, &width, &height);
            if (height > 0 && width >= height) {
                playerAnimationFrames[type][animation] = width / height;
            }
        }
    }

    GLuint backgroundTexture = loadTexture("assets/textures/environment/cyberpunk_street.jpg");

    GLuint robotEnemyTextures[3] = {loadTexture("assets/textures/characters/robot_enemy/Robot Idle.png"),
                                    loadTexture("assets/textures/characters/robot_enemy/Robot run.png"),
                                    loadTexture("assets/textures/characters/robot_enemy/robot shooting.png")};
    GLuint originalEnemyTextures[7] = {0};
    for (int i = 0; i < 7; ++i) {
        char path[128];
        snprintf(path, sizeof(path), "assets/textures/characters/enemy_ranged_%d.png", i + 1);
        originalEnemyTextures[i] = loadTexture(path);
    }
    GLuint spearTexture = loadTexture("assets/textures/characters/enemy_spear.png");

    GLuint mechWalkTexture = loadTexture("assets/textures/enemies/hard_melee/walk.png");
    GLuint mechAttackTexture = loadTexture("assets/textures/enemies/hard_melee/attack.png");

    std::vector<float> vertices;

    std::vector<playerClass *> players;
    std::vector<Impact> impacts;
    std::vector<Projectile> projectiles;
    std::vector<Projectile> enemyProjectiles;
    std::vector<Collectible> collectibles;
    std::vector<Enemy> enemies;
    std::srand(static_cast<unsigned int>(std::time(nullptr)));
    const int enemiesPerWave[3] = {3, 5, 10};
    const int framesBetweenSpawns[3] = {360, 240, 240};
    const int rangedCycleFrames = 450;
    int currentWave = 1;
    int enemiesSpawned = 0;
    int spawnTimer = 0;
    float waveTransitionTimer = 2.0f;
    int gameplayTicks = 0;

    int selectedPlayerType = -1;
    int placementLane = -1;
    bool placingDefense = false;
    const int startingCoins = 30;
    int coins = startingCoins;
    bool wasMouseDown = false;
    bool selectedLanes[3] = {false, false, false};
    bool wasPlayerKeyDown = false;
    bool isPaused = false;
    bool wasEscDown = false;
    bool isGameOver = false;
    bool hasWon = false;
    bool isMainMenu = true;
    bool restartRequested = false;
    bool showingConversation = false;
    ConversationCutscene conversation;
    bool wasEnterDown = false;
    bool wasSpaceDown = false;
    const float contactDistance = 0.11f;

    GLuint VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    GLuint playerVAO, playerVBO;
    glGenVertexArrays(1, &playerVAO);
    glGenBuffers(1, &playerVBO);
    glBindVertexArray(playerVAO);
    glBindBuffer(GL_ARRAY_BUFFER, playerVBO);
    glBufferData(GL_ARRAY_BUFFER, 24 * sizeof(float), nullptr, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindVertexArray(0);

    const double updateStep = 1.0 / 60.0;
    double lastFrameTime = glfwGetTime();
    double accumulatedTime = updateStep;

    while (!glfwWindowShouldClose(window)) {
        double currentFrameTime = glfwGetTime();
        double elapsedTime = currentFrameTime - lastFrameTime;
        lastFrameTime = currentFrameTime;
        if (elapsedTime > 0.25) {
            elapsedTime = 0.25;
        }
        accumulatedTime = accumulatedTime + elapsedTime;
        if (accumulatedTime < updateStep) {
            glfwWaitEventsTimeout(updateStep - accumulatedTime);
            continue;
        }

        accumulatedTime = accumulatedTime - updateStep;

        vertices.clear();

        double mouseX, mouseY;
        int windowWidth, windowHeight;
        int framebufferWidth, framebufferHeight;
        glfwGetCursorPos(window, &mouseX, &mouseY);
        glfwGetWindowSize(window, &windowWidth, &windowHeight);
        glfwGetFramebufferSize(window, &framebufferWidth, &framebufferHeight);

        int mouseState = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
        bool mouseClicked = false;
        if (mouseState == GLFW_PRESS && !wasMouseDown) {
            mouseClicked = true;
        }
        wasMouseDown = (mouseState == GLFW_PRESS);
        bool enterDown = (glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS);
        bool enterPressed = enterDown && !wasEnterDown;
        wasEnterDown = enterDown;
        bool spaceDown = glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS;
        bool spacePressed = spaceDown && !wasSpaceDown;
        wasSpaceDown = spaceDown;

        bool f11Down = glfwGetKey(window, GLFW_KEY_F11) == GLFW_PRESS;
        if (f11Down && !wasF11Down) {
            isFullscreen = !isFullscreen;
            if (isFullscreen) {
                GLFWmonitor *activeMonitor = glfwGetPrimaryMonitor();
                const GLFWvidmode *activeMode = activeMonitor ? glfwGetVideoMode(activeMonitor) : nullptr;
                if (activeMode)
                    glfwSetWindowMonitor(window, activeMonitor, 0, 0, activeMode->width, activeMode->height,
                                         activeMode->refreshRate);
            } else {
                glfwSetWindowMonitor(window, nullptr, 100, 80, 1280, 720, 0);
            }
        }
        wasF11Down = f11Down;
        glfwGetWindowSize(window, &windowWidth, &windowHeight);
        glfwGetFramebufferSize(window, &framebufferWidth, &framebufferHeight);
        int gameWidth = std::min(framebufferWidth, framebufferHeight * 16 / 9);
        int gameHeight = std::min(framebufferHeight, framebufferWidth * 9 / 16);
        int gameLeft = (framebufferWidth - gameWidth) / 2;
        int gameBottom = (framebufferHeight - gameHeight) / 2;
        if (gameWidth == 0 || gameHeight == 0 || windowWidth == 0 || windowHeight == 0) {
            glfwPollEvents();
            continue;
        }

        float framebufferMouseX = static_cast<float>(mouseX * framebufferWidth / windowWidth);
        float framebufferMouseY = static_cast<float>(mouseY * framebufferHeight / windowHeight);
        float x = 2.0f * (framebufferMouseX - gameLeft) / gameWidth - 1.0f;
        float y = 1.0f - 2.0f * (framebufferMouseY - gameBottom) / gameHeight;
        glViewport(gameLeft, gameBottom, gameWidth, gameHeight);

        if ((isPaused || isGameOver || hasWon) && glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS) {
            isMainMenu = true;
        }
        if (isMainMenu) {
            std::vector<float> menuVertices;
            drawText(menuVertices, "LANE DEFENSE", -0.25f, 0.40f, 0.008f);
            bool startClicked = drawMenuButton(menuVertices, "START", 0.18f, x, y, mouseClicked);
            bool exitClicked = drawMenuButton(menuVertices, "EXIT", -0.05f, x, y, mouseClicked);
            if (startClicked || enterPressed) {
                isMainMenu = false;
                conversation.start(currentFrameTime);
                showingConversation = true;
            }
            if (exitClicked) {
                glfwSetWindowShouldClose(window, GLFW_TRUE);
            }
            accumulatedTime = 0.0;
            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferData(GL_ARRAY_BUFFER, menuVertices.size() * sizeof(float), menuVertices.data(),
                         GL_DYNAMIC_DRAW);
            glClearColor(0, 0, 0, 1);
            glClear(GL_COLOR_BUFFER_BIT);
            if (menuBackgroundTexture) {
                const float menuQuad[] = {-1, 1, 0, 1, 1, 1,  1, 1, 1,  -1, 1, 0,
                                          -1, 1, 0, 1, 1, -1, 1, 0, -1, -1, 0, 0};
                glUseProgram(playerProgram);
                glUniform1f(damageLocation, 0.0f);
                glUniform1f(brightnessLocation, 1.0f);
                glUniform1i(glGetUniformLocation(playerProgram, "playerImage"), 0);
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, menuBackgroundTexture);
                glBindBuffer(GL_ARRAY_BUFFER, playerVBO);
                glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(menuQuad), menuQuad);
                glBindVertexArray(playerVAO);
                glDrawArrays(GL_TRIANGLES, 0, 6);
            }
            drawGeometry(menuVertices, shaderProgram, VAO, VBO);
            glfwSwapBuffers(window);
            glfwPollEvents();
            continue;
        }

        if (showingConversation) {
            bool skipHovered = (x >= 0.68f && x <= 0.98f && y >= 0.80f && y <= 0.96f);
            bool escPressed = (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS);

            if (escPressed || (mouseClicked && skipHovered)) {
                wasEscDown = escPressed;
                showingConversation = false;
                restartRequested = true;
                accumulatedTime = 0.0;
                lastFrameTime = currentFrameTime;
                continue;
            } else if (spacePressed || (mouseClicked && !skipHovered)) {
                conversation.advance(currentFrameTime);
                if (!conversation.isRunning()) {
                    showingConversation = false;
                    restartRequested = true;
                    accumulatedTime = 0.0;
                    lastFrameTime = currentFrameTime;
                    continue;
                }
            }

            conversation.update(currentFrameTime);
            if (!conversation.isRunning()) {
                showingConversation = false;
                restartRequested = true;
                accumulatedTime = 0.0;
                lastFrameTime = currentFrameTime;
                continue;
            }

            drawConversation(conversation, currentFrameTime, gameLeft, gameBottom, gameWidth, gameHeight,
                             backgroundTexture, playerProgram, damageLocation, brightnessLocation, playerVBO,
                             playerVAO, shaderProgram, VBO, VAO, playerAnimationTextures,
                             playerAnimationFrames, robotEnemyTextures);
            glfwSwapBuffers(window);
            glfwPollEvents();
            accumulatedTime = 0.0;
            lastFrameTime = currentFrameTime;
            continue;
        }

        int hoveredCard = -1;
        bool rangedUnlocked = currentWave >= 2;
        bool defenseUnlocked = currentWave >= 2;
        if (!isPaused && !isGameOver && !hasWon && y >= 0.73f && y <= 0.92f) {
            if (x >= -0.75f && x <= -0.55f && coins >= 10) {
                hoveredCard = 0;
            } else if (x >= -0.50f && x <= -0.30f && rangedUnlocked && coins >= 20) {
                hoveredCard = 1;
            } else if (x >= -0.25f && x <= -0.05f && defenseUnlocked && coins >= 30) {
                hoveredCard = 2;
            }
        }
        if ((selectedPlayerType == 0 && coins < 10) ||
            (selectedPlayerType == 1 && (!rangedUnlocked || coins < 20)) ||
            (selectedPlayerType == 2 && (!defenseUnlocked || coins < 30))) {
            selectedPlayerType = -1;
            placingDefense = false;
        }

        int hoveredLane = -1;
        if (x >= -0.80f && x <= 0.98f) {
            for (int lane = 0; lane < laneCount; ++lane) {
                if (y <= laneTops[lane] && y >= laneBottoms[lane]) {
                    hoveredLane = lane;
                    break;
                }
            }
        }

        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            if (!wasEscDown) {
                wasEscDown = true;
                if (!isGameOver && !hasWon) {
                    isPaused = !isPaused;
                }
            }
        } else {
            wasEscDown = false;
        }

        if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS && (isPaused || isGameOver || hasWon)) {
            restartRequested = true;
        }
        if (restartRequested) {
            for (size_t p = 0; p < players.size(); p++) {
                delete players[p];
            }
            players.clear();
            impacts.clear();
            projectiles.clear();
            enemyProjectiles.clear();
            collectibles.clear();

            enemies.clear();
            currentWave = 1;
            enemiesSpawned = 0;
            spawnTimer = 0;
            waveTransitionTimer = 0.0f;

            gameplayTicks = 0;
            coins = startingCoins;
            selectedPlayerType = -1;
            placementLane = -1;
            placingDefense = false;
            selectedLanes[0] = false;
            selectedLanes[1] = false;
            selectedLanes[2] = false;
            isPaused = false;
            isGameOver = false;
            hasWon = false;
            restartRequested = false;
            accumulatedTime = 0.0;
            lastFrameTime = glfwGetTime();
        }

        for (size_t j = 0; j < enemies.size(); j++) {
            if (enemies[j].active && enemies[j].x <= -0.80f) {
                isGameOver = true;
                break;
            }
        }

        if (!isPaused && !isGameOver && !hasWon) {

            if (enemiesSpawned == enemiesPerWave[currentWave - 1] && !hasActiveEnemies(enemies) &&
                currentWave < 3) {
                currentWave = currentWave + 1;
                enemiesSpawned = 0;
                spawnTimer = 0;
                waveTransitionTimer = 2.0f;
            }

            if (waveTransitionTimer > 0.0f) {
                waveTransitionTimer -= static_cast<float>(updateStep);
            }

            if (waveTransitionTimer <= 0.0f && enemiesSpawned < enemiesPerWave[currentWave - 1]) {
                if (spawnTimer > 0) {
                    spawnTimer = spawnTimer - 1;
                }
                if (spawnTimer == 0) {
                    EnemyType type = EASY;
                    if (currentWave == 2) {
                        type = (enemiesSpawned % 2 == 0) ? EASY : MEDIUM;
                    } else if (currentWave == 3) {

                        int slot = enemiesSpawned % 5;
                        if (slot < 2) {
                            type = EASY;
                        } else if (slot < 4) {
                            type = MEDIUM;
                        } else {
                            type = HARD;
                        }
                    }
                    int lane = std::rand() % 3;
                    enemies.push_back(Enemy(lane, type));
                    enemiesSpawned = enemiesSpawned + 1;
                    spawnTimer = framesBetweenSpawns[currentWave - 1];
                }
            }
        }

        rangedUnlocked = currentWave >= 2;
        defenseUnlocked = currentWave >= 2;
        int playerToPlace = -1;
        bool isPlayerKeyDown = false;
        if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) {
            playerToPlace = 0;
            isPlayerKeyDown = true;
        } else if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) {
            playerToPlace = 1;
            isPlayerKeyDown = true;
        } else if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS) {
            playerToPlace = 2;
            isPlayerKeyDown = true;
        }
        if (wasPlayerKeyDown || isPaused || isGameOver || hasWon) {
            playerToPlace = -1;
        }
        wasPlayerKeyDown = isPlayerKeyDown;

        if (!isPaused && !isGameOver && !hasWon && mouseClicked) {
            if (hoveredCard != -1) {
                playerToPlace = hoveredCard;
            } else if (hoveredLane != -1 && placingDefense && defenseUnlocked && coins >= 30) {
                defense_player *player = new defense_player();
                player->setLane(hoveredLane);
                player->x = std::clamp(x, -0.72f, 0.90f);
                players.push_back(player);
                coins -= 30;
                placingDefense = false;
            } else if (hoveredLane != -1) {
                selectedLanes[hoveredLane] = !selectedLanes[hoveredLane];
                if (selectedLanes[hoveredLane]) {
                    placementLane = hoveredLane;
                } else if (placementLane == hoveredLane) {
                    placementLane = -1;
                    for (int lane = 0; lane < 3; lane++) {
                        if (selectedLanes[lane]) {
                            placementLane = lane;
                            break;
                        }
                    }
                }
            }
        }

        if (playerToPlace == 2 && defenseUnlocked && coins >= 30) {
            placingDefense = !placingDefense;
            selectedPlayerType = 2;
        } else if (playerToPlace == 0 || playerToPlace == 1) {
            placingDefense = false;
        }

        if (placementLane != -1) {
            if (playerToPlace == 0 && coins >= 10) {
                melee_player *p = new melee_player();
                p->setLane(placementLane);
                players.push_back(p);
                coins = coins - 10;
                selectedPlayerType = 0;
            } else if (playerToPlace == 1 && rangedUnlocked && coins >= 20) {
                ranged_player *p = new ranged_player();
                p->setLane(placementLane);
                players.push_back(p);
                coins = coins - 20;
                selectedPlayerType = 1;
            }
        }

        if (!isPaused && !isGameOver && !hasWon) {
            if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS ||
                glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
                for (size_t i = 0; i < players.size(); i++) {
                    if (selectedLanes[players[i]->lane]) {
                        players[i]->jump();
                    } else {
                        players[i]->stand();
                    }
                }
            } else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS ||
                       glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
                for (size_t i = 0; i < players.size(); i++) {
                    if (selectedLanes[players[i]->lane]) {
                        players[i]->duck();
                    } else {
                        players[i]->stand();
                    }
                }
            } else {
                for (size_t i = 0; i < players.size(); i++) {
                    players[i]->stand();
                }
            }
        }

        updatePlayers(players, enemies, projectiles, collectibles, !isPaused && !isGameOver && !hasWon,
                      gameWidth, gameHeight, contactDistance);
        updatePlayerShots(projectiles, enemies, collectibles, impacts, !isPaused && !isGameOver && !hasWon);
        updateCoins(collectibles, players, coins, !isPaused && !isGameOver && !hasWon);
        updateEnemies(enemies, players, enemyProjectiles, !isPaused && !isGameOver && !hasWon, isGameOver,
                      contactDistance, rangedCycleFrames);

        if (!isPaused && !isGameOver && !hasWon) {
            for (Impact &impact : impacts)
                --impact.ticks;
            impacts.erase(std::remove_if(impacts.begin(), impacts.end(),
                                         [](const Impact &impact) { return impact.ticks <= 0; }),
                          impacts.end());
        }

        updateEnemyShots(enemyProjectiles, players, impacts, !isPaused && !isGameOver && !hasWon);

        if (!isPaused && !isGameOver && !hasWon && currentWave == 3 && enemiesSpawned == enemiesPerWave[2] &&
            !hasActiveEnemies(enemies)) {
            hasWon = true;
        }

        drawObjects(vertices, players, projectiles, collectibles);
        drawCards(vertices, selectedPlayerType, hoveredCard, coins, currentWave);

        if (waveTransitionTimer > 0.0f) {
            const char *title = currentWave == 1 ? "WAVE I" : currentWave == 2 ? "WAVE II" : "WAVE III";
            float waveR = currentWave == 1 ? 0.2f : currentWave == 2 ? 1.0f : 1.0f;
            float waveG = currentWave == 1 ? 1.0f : currentWave == 2 ? 0.9f : 0.2f;
            float waveB = currentWave == 3 ? 0.2f : 0.15f;
            drawText(vertices, title, -0.13f, 0.06f, 0.008f, waveR, waveG, waveB);
        }

        drawText(vertices, "WAVE " + std::to_string(currentWave) + " OF 3", 0.36f, 0.76f, 0.0035f);
        if (enemiesSpawned == enemiesPerWave[currentWave - 1]) {
            drawText(vertices, "DEFEAT REMAINING ENEMIES", 0.36f, 0.69f, 0.0035f);
        } else {
            drawText(vertices,
                     "SPAWNED " + std::to_string(enemiesSpawned) + " OF " +
                         std::to_string(enemiesPerWave[currentWave - 1]),
                     0.36f, 0.69f, 0.0035f);
        }

        if (!isPaused && !isGameOver && !hasWon)
            ++gameplayTicks;
        const size_t sceneVertexCount = vertices.size();
        if (hasWon) {
            addRectangle(vertices, -0.82f, 0.53f, 0.82f, 0.51f, 0.55f, 0.0f, 0.03f);
            addRectangle(vertices, -0.82f, -0.55f, 0.82f, -0.57f, 0.55f, 0.0f, 0.03f);
            drawText(vertices, "THE END", -0.20f, 0.30f, 0.009f, 0.65f, 0.0f, 0.04f);
            drawText(vertices, "ALL THREE WAVES CLEARED", -0.40f, -0.18f, 0.0035f, 0.65f, 0.0f, 0.04f);
        }

        if (isPaused) {
            addRectangle(vertices, -0.45f, 0.48f, 0.45f, -0.50f, 0.08f, 0.08f, 0.12f);
            drawText(vertices, "PAUSED", -0.16f, 0.36f, 0.005f);
            if (drawMenuButton(vertices, "CONTINUE", 0.22f, x, y, mouseClicked)) {
                isPaused = false;
            }
            if (drawMenuButton(vertices, "RESTART", -0.01f, x, y, mouseClicked)) {
                restartRequested = true;
            }
            if (drawMenuButton(vertices, "MAIN MENU", -0.24f, x, y, mouseClicked)) {
                isMainMenu = true;
            }
        }

        if (isGameOver) {
            addRectangle(vertices, -0.45f, 0.48f, 0.45f, -0.50f, 0.15f, 0.05f, 0.05f);
            drawText(vertices, "GAME OVER", -0.22f, 0.36f, 0.005f, 1.0f, 0.25f, 0.25f);
            drawText(vertices, "HOUSE BREACHED", -0.30f, 0.20f, 0.0035f, 1.0f, 0.85f, 0.85f);
        }

        if (isGameOver || hasWon) {
            if (drawMenuButton(vertices, "RESTART", 0.03f, x, y, mouseClicked)) {
                restartRequested = true;
            }
            if (drawMenuButton(vertices, "MAIN MENU", -0.20f, x, y, mouseClicked)) {
                isMainMenu = true;
            }
        }

        if (accumulatedTime >= updateStep) {
            glfwPollEvents();
            continue;
        }

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_DYNAMIC_DRAW);

        glClearColor(0, 0, 0, 1);
        glClear(GL_COLOR_BUFFER_BIT);
        if (hasWon && endingBackgroundTexture) {
            const float cover[] = {-1, 1, 0, 1, 1, 1,  1, 1, 1,  -1, 1, 0,
                                   -1, 1, 0, 1, 1, -1, 1, 0, -1, -1, 0, 0};
            glUseProgram(playerProgram);
            glUniform1f(damageLocation, 0.0f);
            glUniform1f(brightnessLocation, 1.0f);
            glUniform1i(glGetUniformLocation(playerProgram, "playerImage"), 0);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, endingBackgroundTexture);
            glBindBuffer(GL_ARRAY_BUFFER, playerVBO);
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(cover), cover);
            glBindVertexArray(playerVAO);
            glDrawArrays(GL_TRIANGLES, 0, 6);
            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_DYNAMIC_DRAW);
            glUseProgram(shaderProgram);
            glBindVertexArray(VAO);
            glDrawArrays(GL_TRIANGLES, static_cast<GLint>(sceneVertexCount / 6),
                         static_cast<GLsizei>((vertices.size() - sceneVertexCount) / 6));
            glfwSwapBuffers(window);
            glfwPollEvents();
            continue;
        }
        if (waveTransitionTimer > 0.0f) {
            int shake = static_cast<int>(std::sin(gameplayTicks * 70.0 / 60.0) * 9.0);
            glViewport(gameLeft + shake, gameBottom, gameWidth, gameHeight);
        }

        const float backgroundQuad[] = {-1, 1, 0, 1, 1, 1,  1, 1, 1,  -1, 1, 0,
                                        -1, 1, 0, 1, 1, -1, 1, 0, -1, -1, 0, 0};
        glUseProgram(playerProgram);
        glUniform1f(damageLocation, 0.0f);
        glUniform1f(brightnessLocation, 1.0f);
        glUniform1f(brightnessLocation, 1.0f);
        glUniform1i(glGetUniformLocation(playerProgram, "playerImage"), 0);
        glActiveTexture(GL_TEXTURE0);
        glBindVertexArray(playerVAO);
        glBindBuffer(GL_ARRAY_BUFFER, playerVBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(backgroundQuad), backgroundQuad);
        if (backgroundTexture) {
            glBindTexture(GL_TEXTURE_2D, backgroundTexture);
            glDrawArrays(GL_TRIANGLES, 0, 6);
        }

        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(sceneVertexCount / 6));

        std::vector<float> laneFills;
        std::vector<float> laneBorders;
        for (int lane = 0; lane < 3; ++lane) {
            bool selected = selectedLanes[lane];
            bool hovered = lane == hoveredLane && !isPaused && !isGameOver && !hasWon;
            if (!selected && !hovered)
                continue;
            float red = selected ? 1.0f : 0.18f;
            float green = selected ? 0.78f : 0.88f;
            float blue = selected ? 0.12f : 1.0f;
            addRectangle(laneFills, -0.80f, laneTops[lane], 0.98f, laneBottoms[lane], red, green, blue);
            addLaneOutline(laneBorders, laneTops[lane], laneBottoms[lane], selected);
            if (selected) {
                drawText(laneBorders, "SELECTED", -0.76f, laneTops[lane] - 0.035f, 0.004f, red, green, blue);
            }
        }
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, laneFills.size() * sizeof(float), laneFills.data(), GL_DYNAMIC_DRAW);
        glBlendColor(0, 0, 0, 0.22f);
        glBlendFunc(GL_CONSTANT_ALPHA, GL_ONE_MINUS_CONSTANT_ALPHA);
        glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(laneFills.size() / 6));
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glBufferData(GL_ARRAY_BUFFER, laneBorders.size() * sizeof(float), laneBorders.data(),
                     GL_DYNAMIC_DRAW);
        glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(laneBorders.size() / 6));

        drawSprites(players, enemies, enemyProjectiles, gameplayTicks, gameWidth, gameHeight,
                    rangedCycleFrames, playerAnimationTextures, playerAnimationFrames, robotEnemyTextures,
                    originalEnemyTextures, mechWalkTexture, mechAttackTexture, spearTexture,
                    {playerProgram, damageLocation, brightnessLocation, playerVBO, playerVAO});

        std::vector<float> characterOverlay;
        for (const Projectile &shot : enemyProjectiles) {
            if (!shot.active || !shot.isLow)
                continue;
            addRectangle(characterOverlay, shot.x - 0.024f, shot.y + 0.012f, shot.x + 0.018f, shot.y - 0.012f,
                         0.05f, 0.55f, 0.9f);
            addRectangle(characterOverlay, shot.x - 0.015f, shot.y + 0.005f, shot.x + 0.024f, shot.y - 0.005f,
                         0.75f, 1, 1);
        }
        for (const Impact &impact : impacts) {
            float size = 0.008f + (8 - impact.ticks) * 0.003f;
            addRectangle(characterOverlay, impact.x - size, impact.y + 0.006f, impact.x + size,
                         impact.y - 0.006f, 1, 0.9f, 0.4f);
            addRectangle(characterOverlay, impact.x - 0.004f, impact.y + size * 1.5f, impact.x + 0.004f,
                         impact.y - size * 1.5f, 1, 1, 0.8f);
        }
        for (playerClass *player : players) {
            if (player->health <= 0)
                continue;
            float px = player->x;
            float py = player->y + player->yOffset;
            if (player->playerType == playerClass::DEFENSE) {
                addRectangle(characterOverlay, px + 0.018f, py + 0.055f, px + 0.058f, py - 0.06f, 0.10f,
                             0.18f, 0.24f);
                addRectangle(characterOverlay, px + 0.023f, py + 0.045f, px + 0.053f, py - 0.048f, 0.25f,
                             0.55f, 0.65f);
                addRectangle(characterOverlay, px + 0.036f, py + 0.04f, px + 0.041f, py - 0.043f, 0.75f,
                             0.95f, 1);
            } else if (player->playerType == playerClass::MELEE) {
                int cooldown = static_cast<melee_player *>(player)->attackCooldown;
                float swing = 0.0f;
                if (cooldown > 35)
                    swing = (50 - cooldown) / 15.0f;
                else if (cooldown > 0)
                    swing = 1.0f;
                float tipX = px + 0.035f + swing * 0.06f;
                float tipY = py + 0.075f - swing * 0.065f;
                addTriangle(characterOverlay, px + 0.025f, py, tipX, tipY, tipX + 0.008f, tipY, 0.65f, 0.80f,
                            0.85f);
                addTriangle(characterOverlay, px + 0.025f, py, tipX + 0.008f, tipY, px + 0.033f, py, 0.65f,
                            0.80f, 0.85f);
                addRectangle(characterOverlay, px + 0.023f, py + 0.003f, px + 0.035f, py - 0.01f, 0.15f,
                             0.15f, 0.18f);
            } else {
                addRectangle(characterOverlay, px + 0.018f, py + 0.012f, px + 0.07f, py + 0.002f, 0.16f, 0.2f,
                             0.25f);
                addRectangle(characterOverlay, px + 0.03f, py + 0.002f, px + 0.038f, py - 0.017f, 0.3f, 0.25f,
                             0.18f);
            }
            float top = player->y + player->yOffset + 0.18f;
            float fraction = static_cast<float>(player->health) / player->maxHealth;
            addRectangle(characterOverlay, player->x - 0.04f, top, player->x + 0.04f, top - 0.012f, 0.6f,
                         0.1f, 0.1f);
            addRectangle(characterOverlay, player->x - 0.04f, top, player->x - 0.04f + 0.08f * fraction,
                         top - 0.012f, 0.1f, 0.85f, 0.2f);
        }
        for (const Enemy &enemy : enemies) {
            if (!enemy.active)
                continue;
            float top = enemy.y + 0.14f;
            float fraction = static_cast<float>(enemy.health) / enemy.maxHealth;
            addRectangle(characterOverlay, enemy.x - 0.04f, top, enemy.x + 0.04f, top - 0.012f, 0.6f, 0.1f,
                         0.1f);
            addRectangle(characterOverlay, enemy.x - 0.04f, top, enemy.x - 0.04f + 0.08f * fraction,
                         top - 0.012f, 0.1f, 0.85f, 0.2f);
        }
        drawGeometry(characterOverlay, shaderProgram, VAO, VBO);

        if (isPaused || isGameOver || hasWon) {
            drawGeometry(vertices, shaderProgram, VAO, VBO, sceneVertexCount);
        }

        glViewport(0, 0, framebufferWidth, framebufferHeight);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    for (size_t i = 0; i < players.size(); i++) {
        delete players[i];
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);
    glDeleteProgram(playerProgram);
    if (menuBackgroundTexture)
        glDeleteTextures(1, &menuBackgroundTexture);
    if (endingBackgroundTexture)
        glDeleteTextures(1, &endingBackgroundTexture);
    for (int type = 0; type < 3; ++type)
        glDeleteTextures(4, playerAnimationTextures[type]);
    if (backgroundTexture)
        glDeleteTextures(1, &backgroundTexture);
    glDeleteTextures(3, robotEnemyTextures);
    glDeleteTextures(7, originalEnemyTextures);
    if (spearTexture)
        glDeleteTextures(1, &spearTexture);
    if (mechWalkTexture)
        glDeleteTextures(1, &mechWalkTexture);
    if (mechAttackTexture)
        glDeleteTextures(1, &mechAttackTexture);
    glDeleteVertexArrays(1, &playerVAO);
    glDeleteBuffers(1, &playerVBO);

    if (arrowCursor)
        glfwDestroyCursor(arrowCursor);
    glfwTerminate();
    return 0;
}
