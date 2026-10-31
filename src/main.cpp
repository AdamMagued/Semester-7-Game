#define GLAD_GL_IMPLEMENTATION
#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#define STB_IMAGE_IMPLEMENTATION
#include "../assets/textures/stb_image.h"
#include "game/entities/players/melee/melee_player.h"
#include "game/entities/players/defense/defense_player.h"
#include "game/entities/players/ranged/ranged_player.h"
#include "game/entities/enemies/Enemy.h"

bool hasActiveEnemies(const std::vector<Enemy>& enemies) {
    for (size_t i = 0; i < enemies.size(); i++) {
        if (enemies[i].active) {
            return true;
        }
    }
    return false;
}

bool hasDefenseInFront(playerClass* player, const std::vector<playerClass*>& players) {
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

void addRectangle(std::vector<float>& vertices, float left, float top,
                  float right, float bottom, float red, float green, float blue) {
    const float rectangle[] = {
        left, top, 0.0f, red, green, blue,
        right, top, 0.0f, red, green, blue,
        right, bottom, 0.0f, red, green, blue,
        left, top, 0.0f, red, green, blue,
        right, bottom, 0.0f, red, green, blue,
        left, bottom, 0.0f, red, green, blue
    };
    vertices.insert(vertices.end(), rectangle, rectangle + 36);
}

struct Projectile {
    float x;
    float y;
    int lane;
    float speed;
    bool active;
    bool isLow = false;
};

struct Collectible {
    float x;
    float y;
    int lane;
    int value;
    bool active;
};

// Dead-simple 3x5 pixel font for plaintext rendering
static const char* getGlyph(char c) {
    switch (c) {
        case '0': return "###" "#.#" "#.#" "#.#" "###";
        case '1': return ".#." "##." ".#." ".#." "###";
        case '2': return "###" "..#" "###" "#.." "###";
        case '3': return "###" "..#" "###" "..#" "###";
        case '4': return "#.#" "#.#" "###" "..#" "..#";
        case '5': return "###" "#.." "###" "..#" "###";
        case '6': return "###" "#.." "###" "#.#" "###";
        case '7': return "###" "..#" "..#" "..#" "..#";
        case '8': return "###" "#.#" "###" "#.#" "###";
        case '9': return "###" "#.#" "###" "..#" "###";
        case ':': return "..." ".#." "..." ".#." "...";
        case 'A': return "###" "#.#" "###" "#.#" "#.#";
        case 'B': return "##." "#.#" "##." "#.#" "##.";
        case 'C': return "###" "#.." "#.." "#.." "###";
        case 'D': return "##." "#.#" "#.#" "#.#" "##.";
        case 'E': return "###" "#.." "###" "#.." "###";
        case 'F': return "###" "#.." "###" "#.." "#..";
        case 'G': return "###" "#.." "#.#" "#.#" "###";
        case 'H': return "#.#" "#.#" "###" "#.#" "#.#";
        case 'I': return "###" ".#." ".#." ".#." "###";
        case 'L': return "#.." "#.." "#.." "#.." "###";
        case 'M': return "#.#" "###" "#.#" "#.#" "#.#";
        case 'N': return "#.#" "###" "###" "#.#" "#.#";
        case 'O': return "###" "#.#" "#.#" "#.#" "###";
        case 'P': return "###" "#.#" "###" "#.." "#..";
        case 'R': return "###" "#.#" "###" "#.#" "#.#";
        case 'S': return "###" "#.." "###" "..#" "###";
        case 'T': return "###" ".#." ".#." ".#." ".#.";
        case 'U': return "#.#" "#.#" "#.#" "#.#" "###";
        case 'V': return "#.#" "#.#" "#.#" "#.#" ".#.";
        case 'W': return "#.#" "#.#" "#.#" "###" "#.#";
        case 'X': return "#.#" "#.#" ".#." "#.#" "#.#";
        case 'Y': return "#.#" "#.#" ".#." ".#." ".#.";
        case '=': return "..." "###" "..." "###" "...";
        case '*': return ".#." "###" ".#." "..." "...";
        case '^': return ".#." "#.#" "..." "..." "...";
        case '.': return "..." "..." "..." "..." ".#.";
        default:  return "..." "..." "..." "..." "...";
    }
}

static void drawText(std::vector<float>& vertices, const std::string& text, float startX, float startY, float pixelSize, float r = 1.0f, float g = 1.0f, float b = 1.0f) {
    float curX = startX;
    for (size_t i = 0; i < text.size(); i++) {
        char c = text[i];
        if (c == ' ') {
            curX = curX + pixelSize * 3.0f;
            continue;
        }
        const char* glyph = getGlyph(c);
        for (int row = 0; row < 5; row++) {
            for (int col = 0; col < 3; col++) {
                if (glyph[row * 3 + col] == '#') {
                    float px = curX + col * pixelSize;
                    float py = startY - row * pixelSize;
                    addRectangle(vertices, px, py, px + pixelSize, py - pixelSize, r, g, b);
                }
            }
        }
        curX = curX + pixelSize * 4.0f; // 3 pixels for character + 1 pixel gap
    }
}

bool drawMenuButton(std::vector<float>& vertices, const std::string& text,
                    float top, float mouseX, float mouseY, bool clicked) {
    bool hovered = false;
    if (mouseX >= -0.30f && mouseX <= 0.30f && mouseY <= top && mouseY >= top - 0.15f) {
        hovered = true;
    }
    float shade = 0.25f;
    if (hovered) {
        shade = 0.40f;
    }
    addRectangle(vertices, -0.30f, top, 0.30f, top - 0.15f, shade, shade, shade);
    drawText(vertices, text, -0.25f, top - 0.045f, 0.006f);
    if (hovered && clicked) {
        return true;
    }
    return false;
}

void framebufferSizeCallback(GLFWwindow*, int width, int height) {
    glViewport(0, 0, width, height);
}

const char* vertexShaderSrc = R"(
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;

out vec3 vertexColor;

void main() {
    gl_Position = vec4(aPos, 1.0);
    vertexColor = aColor;
}
)";

const char* fragmentShaderSrc = R"(
#version 330 core
in vec3 vertexColor;
out vec4 FragColor;

void main() {
    FragColor = vec4(vertexColor, 1.0);
}
)";

const char* playerVertexShaderSrc = R"(
#version 330 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aUV;
out vec2 uv;
void main() { gl_Position = vec4(aPos, 0.0, 1.0); uv = aUV; }
)";

const char* playerFragmentShaderSrc = R"(
#version 330 core
in vec2 uv;
out vec4 FragColor;
uniform sampler2D playerImage;
void main() { FragColor = texture(playerImage, uv); }
)";

static GLuint compileShader(GLenum type, const char* src) {
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);

    int success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char log[512];
        glGetShaderInfoLog(shader, 512, nullptr, log);
        std::cerr << "Shader compile error:\n" << log << "\n";
    }
    return shader;
}

GLuint loadTexture(const char* path) {
    int width, height, channels;
    unsigned char* image = stbi_load(path, &width, &height, &channels, 4);
    if (!image) {
        std::cerr << "Couldn't load " << path << ": " << stbi_failure_reason() << '\n';
        return 0;
    }

    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0,
                 GL_RGBA, GL_UNSIGNED_BYTE, image);
    stbi_image_free(image);
    return texture;
}

int main() {
    if (!glfwInit()) {
        std::cerr << "Failed to init GLFW\n";
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(800, 600, "Lane Defense", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create window\n";
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    int version = gladLoadGL(glfwGetProcAddress);
    if (version == 0) {
        printf("Failed to initialize OpenGL context\n");
        return -1;
    }

    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
    int framebufferWidth;
    int framebufferHeight;
    glfwGetFramebufferSize(window, &framebufferWidth, &framebufferHeight);
    framebufferSizeCallback(window, framebufferWidth, framebufferHeight);

    // --- Shader program ---
    GLuint vertexShader = compileShader(GL_VERTEX_SHADER, vertexShaderSrc);
    GLuint fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentShaderSrc);

    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    int success;
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        char log[512];
        glGetProgramInfoLog(shaderProgram, 512, nullptr, log);
        std::cerr << "Shader link error:\n" << log << "\n";
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    GLuint playerVS = compileShader(GL_VERTEX_SHADER, playerVertexShaderSrc);
    GLuint playerFS = compileShader(GL_FRAGMENT_SHADER, playerFragmentShaderSrc);
    GLuint playerProgram = glCreateProgram();
    glAttachShader(playerProgram, playerVS);
    glAttachShader(playerProgram, playerFS);
    glLinkProgram(playerProgram);
    glDeleteShader(playerVS);
    glDeleteShader(playerFS);

    int imageWidth, imageHeight, imageChannels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* image = stbi_load("assets/textures/characters/player.png",
                                     &imageWidth, &imageHeight, &imageChannels, 4);
    GLuint playerTexture = 0;
    if (image) {
        glGenTextures(1, &playerTexture);
        glBindTexture(GL_TEXTURE_2D, playerTexture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageWidth, imageHeight, 0,
                     GL_RGBA, GL_UNSIGNED_BYTE, image);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        stbi_image_free(image);
    } else {
        std::cerr << "Couldn't load player.png: " << stbi_failure_reason() << '\n';
    }

    const char* backgroundFiles[] = {
        "assets/textures/environment/parallax_forest_pack/layers/parallax-forest-back-trees.png",
        "assets/textures/environment/parallax_forest_pack/layers/parallax-forest-lights.png",
        "assets/textures/environment/parallax_forest_pack/layers/parallax-forest-middle-trees.png",
        "assets/textures/environment/parallax_forest_pack/layers/parallax-forest-front-trees.png"
    };
    GLuint backgroundTextures[4];
    for (int i = 0; i < 4; ++i)
        backgroundTextures[i] = loadTexture(backgroundFiles[i]);

    GLuint enemyRangedTextures[7] = {0};
    for (int i = 0; i < 7; ++i) {
        char texPath[128];
        snprintf(texPath, sizeof(texPath), "assets/textures/characters/enemy_ranged_%d.png", i + 1);
        enemyRangedTextures[i] = loadTexture(texPath);
    }
    GLuint spearTexture = loadTexture("assets/textures/characters/enemy_spear.png");

    // --- Playfield rectangles ---
    std::vector<float> vertices;
    addRectangle(vertices, -0.82f,  0.62f, 0.98f,  0.20f, 0.2f, 0.6f, 0.2f);
    addRectangle(vertices, -0.82f,  0.20f, 0.98f, -0.28f, 0.2f, 0.6f, 0.2f);
    addRectangle(vertices, -0.82f, -0.28f, 0.98f, -0.82f, 0.2f, 0.6f, 0.2f);
    addRectangle(vertices, -0.80f,  0.205f, 0.98f,  0.195f, 0.08f, 0.32f, 0.12f);
    addRectangle(vertices, -0.80f, -0.275f, 0.98f, -0.285f, 0.08f, 0.32f, 0.12f);
    addRectangle(vertices, -0.96f,  0.66f, -0.80f, -0.82f, 0.15f, 0.35f, 0.75f);
    addRectangle(vertices, -0.97f,  0.70f, -0.79f,  0.58f, 0.08f, 0.20f, 0.48f); // roof
    addRectangle(vertices, -0.94f,  0.43f, -0.90f,  0.33f, 0.65f, 0.85f, 1.0f);  // left window
    addRectangle(vertices, -0.86f,  0.43f, -0.82f,  0.33f, 0.65f, 0.85f, 1.0f);  // right window
    addRectangle(vertices, -0.90f, -0.82f, -0.85f, -0.53f, 0.08f, 0.20f, 0.48f); // doorway

    // Player, Projectiles, Enemies, Lane Selection, and UI Card setup
    std::vector<playerClass*> players;
    std::vector<Projectile> projectiles;
    std::vector<Projectile> enemyProjectiles;
    std::vector<Collectible> collectibles;
    std::vector<Enemy> enemies;
    std::srand(static_cast<unsigned int>(std::time(nullptr)));
    const int enemiesPerWave[3] = {3, 5, 10};
    const int framesBetweenSpawns = 180;
    int currentWave = 1;
    int enemiesSpawned = 0;
    int spawnTimer = 0;

    int selectedPlayerType = -1;
    int placementLane = -1;
    const int startingCoins = 10;
    int coins = startingCoins;
    bool wasMouseDown = false;   // To detect a fresh mouse click
    bool selectedLanes[3] = {false, false, false};
    bool wasPlayerKeyDown = false;
    bool isPaused = false;
    bool wasEscDown = false;
    bool isGameOver = false;
    bool hasWon = false;
    bool isMainMenu = true;
    bool restartRequested = false;
    bool wasEnterDown = false;
    const float contactDistance = 0.11f;

    GLuint VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    // Each vertex has 6 floats: 3 for position and 3 for color.
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(
        1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float),
        (void*)(3 * sizeof(float))
    );
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    GLuint playerVAO, playerVBO;
    glGenVertexArrays(1, &playerVAO);
    glGenBuffers(1, &playerVBO);
    glBindVertexArray(playerVAO);
    glBindBuffer(GL_ARRAY_BUFFER, playerVBO);
    glBufferData(GL_ARRAY_BUFFER, 24 * sizeof(float), nullptr, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindVertexArray(0);

    const size_t boardSize = vertices.size();
    const double updateStep = 1.0 / 60.0;
    double lastFrameTime = glfwGetTime();
    double accumulatedTime = updateStep;

    // --- Render loop ---
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
        // Gameplay always advances in steps of 1/60 second.
        accumulatedTime = accumulatedTime - updateStep;

        // Remove last frame's highlight before checking the mouse again.
        vertices.resize(boardSize);

        double mouseX, mouseY;
        int windowWidth, windowHeight;
        glfwGetCursorPos(window, &mouseX, &mouseY);
        glfwGetWindowSize(window, &windowWidth, &windowHeight);

        float x = 2.0f * static_cast<float>(mouseX) / windowWidth - 1.0f;
        float y = 1.0f - 2.0f * static_cast<float>(mouseY) / windowHeight;

        int mouseState = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
        bool mouseClicked = false;
        if (mouseState == GLFW_PRESS && !wasMouseDown) {
            mouseClicked = true;
        }
        wasMouseDown = (mouseState == GLFW_PRESS);
        bool enterDown = (glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS);
        bool enterPressed = enterDown && !wasEnterDown;
        wasEnterDown = enterDown;

        if ((isPaused || isGameOver || hasWon) && glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS) {
            isMainMenu = true;
        }
        if (isMainMenu) {
            std::vector<float> menuVertices;
            drawText(menuVertices, "LANE DEFENSE", -0.25f, 0.40f, 0.008f);
            bool startClicked = drawMenuButton(menuVertices, "START", 0.18f, x, y, mouseClicked);
            bool exitClicked = drawMenuButton(menuVertices, "EXIT", -0.05f, x, y, mouseClicked);
            drawText(menuVertices, "ENTER TO START", -0.25f, -0.35f, 0.005f);
            if (startClicked || enterPressed) {
                isMainMenu = false;
                restartRequested = true;
            }
            if (exitClicked) {
                glfwSetWindowShouldClose(window, GLFW_TRUE);
            }
            accumulatedTime = 0.0;
            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferData(GL_ARRAY_BUFFER, menuVertices.size() * sizeof(float), menuVertices.data(), GL_DYNAMIC_DRAW);
            glClearColor(0.08f, 0.08f, 0.08f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);
            glUseProgram(shaderProgram);
            glBindVertexArray(VAO);
            glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(menuVertices.size() / 6));
            glfwSwapBuffers(window);
            glfwPollEvents();
            continue;
        }

        int hoveredCard = -1;
        if (!isPaused && !isGameOver && !hasWon && y >= 0.73f && y <= 0.92f) {
            if (x >= -0.75f && x <= -0.55f && coins >= 10) {
                hoveredCard = 0;
            } else if (x >= -0.50f && x <= -0.30f && coins >= 20) {
                hoveredCard = 1;
            } else if (x >= -0.25f && x <= -0.05f && coins >= 30) {
                hoveredCard = 2;
            }
        }
        if ((selectedPlayerType == 0 && coins < 10) ||
            (selectedPlayerType == 1 && coins < 20) ||
            (selectedPlayerType == 2 && coins < 30)) {
            selectedPlayerType = -1;
        }

        // --- Draw UI Cards in top-left ---
        // Card 1: Melee (Cost 10) - Red Box
        if ((selectedPlayerType == 0 || hoveredCard == 0) && coins >= 10) {
            addRectangle(vertices, -0.76f, 0.93f, -0.54f, 0.72f, 1.0f, 1.0f, 1.0f); // White selection border
        }
        if (coins >= 10) {
            addRectangle(vertices, -0.75f, 0.92f, -0.55f, 0.73f, 0.85f, 0.25f, 0.25f); // Red card
            drawText(vertices, "COST 10", -0.70f, 0.835f, 0.0035f, 1.0f, 1.0f, 1.0f);
        } else {
            addRectangle(vertices, -0.75f, 0.92f, -0.55f, 0.73f, 0.40f, 0.40f, 0.40f); // Locked card
            drawText(vertices, "COST 10", -0.70f, 0.835f, 0.0035f, 0.75f, 0.75f, 0.75f);
        }
        drawText(vertices, "MELEE", -0.68f, 0.695f, 0.0035f, 1.0f, 1.0f, 1.0f); // Text just under the box

        // Card 2: Ranged (Cost 20) - Blue Box
        if ((selectedPlayerType == 1 || hoveredCard == 1) && coins >= 20) {
            addRectangle(vertices, -0.51f, 0.93f, -0.29f, 0.72f, 1.0f, 1.0f, 1.0f); // White selection border
        }
        if (coins >= 20) {
            addRectangle(vertices, -0.50f, 0.92f, -0.30f, 0.73f, 0.25f, 0.45f, 0.85f); // Blue card
            drawText(vertices, "COST 20", -0.45f, 0.835f, 0.0035f, 1.0f, 1.0f, 1.0f);
        } else {
            addRectangle(vertices, -0.50f, 0.92f, -0.30f, 0.73f, 0.40f, 0.40f, 0.40f); // Locked card
            drawText(vertices, "COST 20", -0.45f, 0.835f, 0.0035f, 0.75f, 0.75f, 0.75f);
        }
        drawText(vertices, "RANGED", -0.44f, 0.695f, 0.0035f, 1.0f, 1.0f, 1.0f); // Text just under the box

        // Card 3: Defense (Cost 30) - Gold Box
        if ((selectedPlayerType == 2 || hoveredCard == 2) && coins >= 30) {
            addRectangle(vertices, -0.26f, 0.93f, -0.04f, 0.72f, 1.0f, 1.0f, 1.0f); // White selection border
        }
        if (coins >= 30) {
            addRectangle(vertices, -0.25f, 0.92f, -0.05f, 0.73f, 0.85f, 0.65f, 0.20f); // Gold card
            drawText(vertices, "COST 30", -0.20f, 0.835f, 0.0035f, 1.0f, 1.0f, 1.0f);
        } else {
            addRectangle(vertices, -0.25f, 0.92f, -0.05f, 0.73f, 0.40f, 0.40f, 0.40f); // Locked card
            drawText(vertices, "COST 30", -0.20f, 0.835f, 0.0035f, 0.75f, 0.75f, 0.75f);
        }
        drawText(vertices, "DEFENSE", -0.20f, 0.695f, 0.0035f, 1.0f, 1.0f, 1.0f); // Text just under the box

        // Plaintext yellow coins display
        std::string coinStr = "COINS: " + std::to_string(coins);
        drawText(vertices, coinStr, 0.36f, 0.85f, 0.0035f, 1.0f, 0.9f, 0.1f);

        // Check lane hovering
        int hoveredLane = -1;
        if (x >= -0.80f && x <= 0.98f) {
            if (y <= 0.62f && y >= 0.20f) hoveredLane = 0;
            else if (y < 0.20f && y >= -0.28f) hoveredLane = 1;
            else if (y < -0.28f && y >= -0.82f) hoveredLane = 2;
        }

        // Draw prominent selected lane highlight (stays visible even when hovering away!)
        if (selectedLanes[0]) {
            addRectangle(vertices, -0.80f,  0.62f, 0.98f,  0.20f, 0.30f, 0.72f, 0.30f); // Bright green tint
            addRectangle(vertices, -0.82f,  0.62f, -0.79f, 0.20f, 1.00f, 0.85f, 0.20f); // Gold active lane marker
        }
        if (selectedLanes[1]) {
            addRectangle(vertices, -0.80f,  0.20f, 0.98f, -0.28f, 0.30f, 0.72f, 0.30f);
            addRectangle(vertices, -0.82f,  0.20f, -0.79f, -0.28f, 1.00f, 0.85f, 0.20f);
        }
        if (selectedLanes[2]) {
            addRectangle(vertices, -0.80f, -0.28f, 0.98f, -0.82f, 0.30f, 0.72f, 0.30f);
            addRectangle(vertices, -0.82f, -0.28f, -0.79f, -0.82f, 1.00f, 0.85f, 0.20f);
        }

        // Draw hovered lane highlight on top
        if (hoveredLane == 0)
            addRectangle(vertices, -0.80f, 0.62f, 0.98f, 0.20f, 0.38f, 0.82f, 0.38f);
        else if (hoveredLane == 1)
            addRectangle(vertices, -0.80f, 0.20f, 0.98f, -0.28f, 0.38f, 0.82f, 0.38f);
        else if (hoveredLane == 2)
            addRectangle(vertices, -0.80f, -0.28f, 0.98f, -0.82f, 0.38f, 0.82f, 0.38f);

        // Check Escape key for pause
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
            projectiles.clear();
            enemyProjectiles.clear();
            collectibles.clear();

            enemies.clear();
            currentWave = 1;
            enemiesSpawned = 0;
            spawnTimer = 0;

            coins = startingCoins;
            selectedPlayerType = -1;
            placementLane = -1;
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


        // Check if any active enemy breached the home base (x <= -0.80f)
        for (size_t j = 0; j < enemies.size(); j++) {
            if (enemies[j].active && enemies[j].x <= -0.80f) {
                isGameOver = true;
                break;
            }
        }

        if (!isPaused && !isGameOver && !hasWon) {
            // Finish spawning and defeat every enemy before starting the next wave.
            if (enemiesSpawned == enemiesPerWave[currentWave - 1] &&
                !hasActiveEnemies(enemies) && currentWave < 3) {
                currentWave = currentWave + 1;
                enemiesSpawned = 0;
                spawnTimer = 0;
            }

            if (enemiesSpawned < enemiesPerWave[currentWave - 1]) {
                if (spawnTimer > 0) {
                    spawnTimer = spawnTimer - 1;
                }
                if (spawnTimer == 0) {
                    EnemyType type = EASY;
                    if (currentWave == 2 && enemiesSpawned % 2 == 1) {
                        type = MEDIUM;
                    } else if (currentWave == 3) {
                        if (enemiesSpawned % 3 == 1) {
                            type = MEDIUM;
                        } else if (enemiesSpawned % 3 == 2) {
                            type = HARD;
                        }
                    }
                    int lane = std::rand() % 3;
                    enemies.push_back(Enemy(lane, type));
                    enemiesSpawned = enemiesSpawned + 1;
                    spawnTimer = framesBetweenSpawns;
                }
            }
        }

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

        // Lane clicks only select. Cards or number keys buy one player.
        if (!isPaused && !isGameOver && !hasWon && mouseClicked) {
            if (hoveredCard != -1) {
                playerToPlace = hoveredCard;
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

        if (placementLane != -1) {
            if (playerToPlace == 0 && coins >= 10) {
                melee_player* p = new melee_player();
                p->setLane(placementLane);
                players.push_back(p);
                coins = coins - 10;
                selectedPlayerType = 0;
            } else if (playerToPlace == 1 && coins >= 20) {
                ranged_player* p = new ranged_player();
                p->setLane(placementLane);
                players.push_back(p);
                coins = coins - 20;
                selectedPlayerType = 1;
            } else if (playerToPlace == 2 && coins >= 30) {
                defense_player* p = new defense_player();
                p->setLane(placementLane);
                players.push_back(p);
                coins = coins - 30;
                selectedPlayerType = 2;
            }
        }
        drawText(vertices, "SELECT LANE THEN CARD OR 1 2 3", -0.72f, -0.90f, 0.005f);

        // Check keyboard for Jump (W / Up) and Duck (S / Down) across all selected lanes
        if (!isPaused && !isGameOver && !hasWon) {
            if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
                for (size_t i = 0; i < players.size(); i++) {
                    if (selectedLanes[players[i]->lane]) {
                        players[i]->jump();
                    } else {
                        players[i]->stand();
                    }
                }
            } else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
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

        // Move and draw each spawned player
        for (size_t i = 0; i < players.size(); i++) {
            if (players[i]->health <= 0) {
                delete players[i];
                players.erase(players.begin() + i);
                i = i - 1;
                continue;
            }
            bool isPlayerBlocked = false;

            // Update player combat and movement if not paused or game over
            if (!isPaused && !isGameOver && !hasWon) {
                // Check if any active enemy on the same lane blocks forward movement
                for (size_t j = 0; j < enemies.size(); j++) {
                    if (enemies[j].active && enemies[j].lane == players[i]->lane) {
                        float dist = enemies[j].x - players[i]->x;
                        if (dist >= 0.0f && dist <= contactDistance) {
                            isPlayerBlocked = true;
                            break;
                        }
                    }
                }

                // Check melee attack collision with enemies
                if (players[i]->playerType == playerClass::MELEE) {
                    melee_player* mp = static_cast<melee_player*>(players[i]);
                    if (mp->attackCooldown > 0) {
                        mp->attackCooldown = mp->attackCooldown - 1;
                    }

                    for (size_t j = 0; j < enemies.size(); j++) {
                        if (enemies[j].active && enemies[j].lane == mp->lane) {
                            float dist = enemies[j].x - mp->x;
                            if (dist >= 0.0f && dist <= contactDistance) {
                                if (mp->attackCooldown <= 0) {
                                    mp->doAttack(); // Trigger strategy attack (punched)
                                    enemies[j].health = enemies[j].health - mp->damage;
                                    mp->attackCooldown = 50; // Punch cooldown (~0.8s)

                                    if (enemies[j].health <= 0) {
                                        enemies[j].active = false;
                                        Collectible c;
                                        c.x = enemies[j].x;
                                        c.y = enemies[j].y;
                                        c.lane = enemies[j].lane;
                                        c.value = 10;
                                        c.active = true;
                                        collectibles.push_back(c);
                                    }
                                }
                                break;
                            }
                        }
                    }
                }

                // Only advance forward if not currently blocked by an enemy
                if (isPlayerBlocked == false) {
                    players[i]->move();
                }
                players[i]->updateJump();
            }

            if (players[i]->playerType == playerClass::MELEE) {
                for (size_t j = 0; j < enemies.size(); j++) {
                    float dist = enemies[j].x - players[i]->x;
                    if (enemies[j].active && enemies[j].lane == players[i]->lane &&
                        dist >= 0.0f && dist <= contactDistance) {
                        float punchY = players[i]->y + players[i]->yOffset;
                        addRectangle(vertices, players[i]->x + 0.04f, punchY + 0.02f,
                                               players[i]->x + 0.09f, punchY - 0.02f,
                                               1.0f, 0.4f, 0.4f);
                        break;
                    }
                }
            }

            float drawY = players[i]->y + players[i]->yOffset;
            float r = 0.9f;
            float g = 0.2f;
            float b = 0.2f; // Red for Melee

            if (players[i]->playerType == playerClass::RANGED) {
                r = 0.2f; g = 0.5f; b = 0.9f; // Blue for Ranged
            } else if (players[i]->playerType == playerClass::DEFENSE) {
                r = 0.85f; g = 0.65f; b = 0.2f; // Gold for Defense
            }

            if (players[i]->damageFlashTimer > 0) {
                r = 1.0f; g = 0.8f; b = 0.8f; // Flash on hit
            }

            addRectangle(vertices, players[i]->x - 0.04f, drawY + 0.06f,
                                   players[i]->x + 0.04f, drawY - 0.06f,
                                   r, g, b);

            // Health bar above player
            float px = players[i]->x;
            float barY = drawY + 0.080f;
            addRectangle(vertices, px - 0.04f, barY + 0.006f, px + 0.04f, barY - 0.006f, 0.35f, 0.1f, 0.1f);
            float hpRatio = (float)players[i]->health / (float)players[i]->maxHealth;
            if (hpRatio < 0.0f) hpRatio = 0.0f;
            if (hpRatio > 1.0f) hpRatio = 1.0f;
            addRectangle(vertices, px - 0.04f, barY + 0.006f, px - 0.04f + 0.08f * hpRatio, barY - 0.006f, 0.2f, 0.85f, 0.2f);

            // Check if protected behind a defense player on the same lane
            bool protectedByDefense = hasDefenseInFront(players[i], players);
            players[i]->isProtected = protectedByDefense;
            if (protectedByDefense) {
                players[i]->playerStatus = playerClass::invulnerable;
                // Straight vertical blue line as a shield in front of protected player
                addRectangle(vertices, players[i]->x + 0.05f, drawY + 0.07f,
                                       players[i]->x + 0.06f, drawY - 0.07f,
                                       0.2f, 0.5f, 1.0f);
            } else {
                players[i]->playerStatus = playerClass::vulnerable;
            }

            // If ranged player, periodically shoot projectiles forward down their lane
            if (!isPaused && !isGameOver && !hasWon && players[i]->playerType == playerClass::RANGED) {
                ranged_player* rp = static_cast<ranged_player*>(players[i]);
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
        }

        // Move and draw projectiles fired by ranged players
        for (size_t i = 0; i < projectiles.size(); i++) {
            if (projectiles[i].active == true) {
                if (!isPaused && !isGameOver && !hasWon) {
                    projectiles[i].x = projectiles[i].x + projectiles[i].speed;

                    // Check collision with enemies
                    for (size_t j = 0; j < enemies.size(); j++) {
                        if (enemies[j].active && enemies[j].lane == projectiles[i].lane) {
                            float dist = std::abs(projectiles[i].x - enemies[j].x);
                            if (dist < 0.05f) {
                                projectiles[i].active = false;
                                enemies[j].health = enemies[j].health - 1;
                                if (enemies[j].health <= 0) {
                                    enemies[j].active = false;
                                    Collectible c;
                                    c.x = enemies[j].x;
                                    c.y = enemies[j].y;
                                    c.lane = enemies[j].lane;
                                    c.value = 10;
                                    c.active = true;
                                    collectibles.push_back(c);
                                }
                                break;
                            }
                        }
                    }

                    if (projectiles[i].x > 0.95f) {
                        projectiles[i].active = false;
                    }
                }

                if (projectiles[i].active == true) {
                    addRectangle(vertices, projectiles[i].x - 0.02f, projectiles[i].y + 0.015f,
                                           projectiles[i].x + 0.02f, projectiles[i].y - 0.015f,
                                           1.0f, 0.85f, 0.1f); // Yellow projectile bullet
                }
            }
        }

        // Draw and pick up dropped collectibles
        for (size_t i = 0; i < collectibles.size(); i++) {
            if (collectibles[i].active == true) {
                // Check if any moving player walks over the dropped coin
                if (!isPaused && !isGameOver && !hasWon) {
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

                // Simple yellow square for dropped coin
                if (collectibles[i].active == true) {
                    addRectangle(vertices, collectibles[i].x - 0.02f, collectibles[i].y + 0.02f,
                                           collectibles[i].x + 0.02f, collectibles[i].y - 0.02f,
                                           1.0f, 0.85f, 0.1f);
                }
            }
        }

        // Move and draw active enemies
        for (size_t j = 0; j < enemies.size(); j++) {
            if (enemies[j].active == true) {
                if (!isPaused && !isGameOver && !hasWon) {
                    enemies[j].attackTimer = enemies[j].attackTimer + 1;
                    int blockingPlayer = -1;
                    float closestDistance = contactDistance;

                    // Check if blocked by any player or defense blocker on the same lane
                    for (size_t p = 0; p < players.size(); p++) {
                        if (players[p]->health > 0 && players[p]->lane == enemies[j].lane) {
                            float dist = enemies[j].x - players[p]->x;
                            if (dist >= 0.0f && dist <= closestDistance) {
                                blockingPlayer = static_cast<int>(p);
                                closestDistance = dist;
                            }
                        }
                    }
                    if (enemies[j].type == HARD && blockingPlayer != -1 && enemies[j].attackTimer % 50 == 0) {
                        players[blockingPlayer]->isProtected = hasDefenseInFront(players[blockingPlayer], players);
                        players[blockingPlayer]->takeDamage(enemies[j].damage);
                    }

                    int animCycle = enemies[j].attackTimer % 300;

                    // Ranged enemies stop to throw. Melee enemies keep advancing.
                    if (enemies[j].type == HARD || animCycle < 150) {
                        if (blockingPlayer == -1) {
                            enemies[j].moveLeft();
                            if (enemies[j].x <= -0.80f) {
                                isGameOver = true;
                            }
                        }
                    } else if (animCycle == 240) {
                        // Frame 5 release: throw spear straight horizontally down the lane!
                        Projectile spear;
                        spear.x = enemies[j].x;
                        spear.y = enemies[j].y;
                        if (enemies[j].type == MEDIUM) {
                            spear.isLow = true;
                            spear.y = enemies[j].y - 0.05f;
                        }
                        spear.lane = enemies[j].lane;
                        spear.speed = 0.0075f;
                        spear.active = true;
                        enemyProjectiles.push_back(spear);
                    }
                }

                int animCycle = enemies[j].attackTimer % 300;
                if (enemies[j].type != HARD && animCycle >= 190 && animCycle < 240) {
                    // Equation from Frame 4
                    drawText(vertices, "D=U*T", enemies[j].x - 0.10f, enemies[j].y + 0.13f, 0.003f, 1.0f, 1.0f, 0.2f);

                    // Straight horizontal aiming line dots extending forward from spear
                    for (float t = 0.0f; t <= 1.0f; t += 0.10f) {
                        float aimX = enemies[j].x - 0.05f - t * 0.25f;
                        float aimY = enemies[j].y;
                        if (enemies[j].type == MEDIUM) {
                            aimY = enemies[j].y - 0.05f;
                        }
                        addRectangle(vertices, aimX - 0.002f, aimY + 0.002f, aimX + 0.002f, aimY - 0.002f,
                                               1.0f, 0.9f, 0.3f);
                    }
                }

                // Enemy Health bar above head (red background, green remaining)
                float healthRatio = (float)enemies[j].health / (float)enemies[j].maxHealth;
                if (healthRatio < 0.0f) healthRatio = 0.0f;
                if (healthRatio > 1.0f) healthRatio = 1.0f;
                addRectangle(vertices, enemies[j].x - 0.04f, enemies[j].y + 0.09f,
                                       enemies[j].x + 0.04f, enemies[j].y + 0.08f,
                                       0.6f, 0.1f, 0.1f);
                addRectangle(vertices, enemies[j].x - 0.04f, enemies[j].y + 0.09f,
                                       enemies[j].x - 0.04f + 0.08f * healthRatio, enemies[j].y + 0.08f,
                                       0.1f, 0.85f, 0.2f);
            }
        }

        // Move and update enemy spears thrown horizontally at players
        for (size_t k = 0; k < enemyProjectiles.size(); k++) {
            if (enemyProjectiles[k].active == true) {
                if (!isPaused && !isGameOver && !hasWon) {
                    enemyProjectiles[k].x = enemyProjectiles[k].x - enemyProjectiles[k].speed;

                    // Check collision with players on this lane
                    int targetPlayer = -1;
                    for (size_t p = 0; p < players.size(); p++) {
                        if (players[p]->health > 0 && players[p]->lane == enemyProjectiles[k].lane) {
                            float dist = std::abs(enemyProjectiles[k].x - players[p]->x);
                            if (dist < 0.05f) {
                                if (players[p]->playerType != playerClass::DEFENSE) {
                                    if (enemyProjectiles[k].isLow && players[p]->yOffset > 0.05f) {
                                        continue;
                                    }
                                    if (!enemyProjectiles[k].isLow && players[p]->yOffset < -0.02f) {
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
                        players[targetPlayer]->isProtected = hasDefenseInFront(players[targetPlayer], players);
                        players[targetPlayer]->takeDamage(1);
                    }

                    // Remove if left screen
                    if (enemyProjectiles[k].x < -0.90f) {
                        enemyProjectiles[k].active = false;
                    }
                }
            }
        }

        if (!isPaused && !isGameOver && !hasWon && currentWave == 3 &&
            enemiesSpawned == enemiesPerWave[2] && !hasActiveEnemies(enemies)) {
            hasWon = true;
        }

        drawText(vertices, "WAVE " + std::to_string(currentWave) + " OF 3",
                 0.36f, 0.76f, 0.0035f);
        if (enemiesSpawned == enemiesPerWave[currentWave - 1]) {
            drawText(vertices, "DEFEAT REMAINING ENEMIES", 0.36f, 0.69f, 0.0035f);
        } else {
            drawText(vertices, "SPAWNED " + std::to_string(enemiesSpawned) + " OF " +
                     std::to_string(enemiesPerWave[currentWave - 1]),
                     0.36f, 0.69f, 0.0035f);
        }

        const size_t sceneVertexCount = vertices.size();

        // Draw Pause overlay
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
            drawText(vertices, "ESC TO RESUME  M TO MENU", -0.36f, -0.44f, 0.0035f);
        }

        // Draw Game Over overlay
        if (isGameOver) {
            addRectangle(vertices, -0.45f, 0.48f, 0.45f, -0.50f, 0.15f, 0.05f, 0.05f);
            drawText(vertices, "GAME OVER", -0.22f, 0.36f, 0.005f, 1.0f, 0.25f, 0.25f);
            drawText(vertices, "HOUSE BREACHED", -0.30f, 0.20f, 0.0035f, 1.0f, 0.85f, 0.85f);
        }

        if (hasWon) {
            addRectangle(vertices, -0.45f, 0.48f, 0.45f, -0.50f, 0.05f, 0.15f, 0.05f);
            drawText(vertices, "YOU WIN", -0.17f, 0.36f, 0.005f, 0.4f, 1.0f, 0.4f);
            drawText(vertices, "ALL THREE WAVES CLEARED", -0.40f, 0.20f, 0.0035f);
        }
        if (isGameOver || hasWon) {
            if (drawMenuButton(vertices, "RESTART", 0.03f, x, y, mouseClicked)) {
                restartRequested = true;
            }
            if (drawMenuButton(vertices, "MAIN MENU", -0.20f, x, y, mouseClicked)) {
                isMainMenu = true;
            }
            drawText(vertices, "R TO RESTART  M TO MENU", -0.36f, -0.44f, 0.0035f);
        }

        // Catch up gameplay before drawing another frame.
        if (accumulatedTime >= updateStep) {
            glfwPollEvents();
            continue;
        }

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_DYNAMIC_DRAW);

        glClearColor(0.15f, 0.35f, 0.12f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        const float backgroundQuad[] = {
            -1,1,0,1,  1,1,1,1,  1,-1,1,0,
            -1,1,0,1,  1,-1,1,0,  -1,-1,0,0
        };
        glUseProgram(playerProgram);
        glUniform1i(glGetUniformLocation(playerProgram, "playerImage"), 0);
        glActiveTexture(GL_TEXTURE0);
        glBindVertexArray(playerVAO);
        glBindBuffer(GL_ARRAY_BUFFER, playerVBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(backgroundQuad), backgroundQuad);
        for (GLuint backgroundTexture : backgroundTextures) {
            if (!backgroundTexture) continue;
            glBindTexture(GL_TEXTURE_2D, backgroundTexture);
            glDrawArrays(GL_TRIANGLES, 0, 6);
        }

        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(sceneVertexCount / 6));

        if (playerTexture) {
            for (size_t i = 0; i < players.size(); i++) {
                if (players[i]->playerType == playerClass::MELEE) {
                    const float left = players[i]->x - 0.04f;
                    const float right = players[i]->x + 0.04f;
                    const float top = players[i]->y + players[i]->yOffset + 0.06f;
                    const float bottom = players[i]->y + players[i]->yOffset - 0.06f;
                    const float quad[] = {
                        left,top,0,1, right,top,0.25f,1, right,bottom,0.25f,0,
                        left,top,0,1, right,bottom,0.25f,0, left,bottom,0,0
                    };
                    glUseProgram(playerProgram);
                    glActiveTexture(GL_TEXTURE0);
                    glBindTexture(GL_TEXTURE_2D, playerTexture);
                    glUniform1i(glGetUniformLocation(playerProgram, "playerImage"), 0);
                    glBindBuffer(GL_ARRAY_BUFFER, playerVBO);
                    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(quad), quad);
                    glBindVertexArray(playerVAO);
                    glDrawArrays(GL_TRIANGLES, 0, 6);
                }
            }
        }

        // Draw animated enemy textured quads
        for (size_t j = 0; j < enemies.size(); j++) {
            if (enemies[j].active) {
                int animCycle = enemies[j].attackTimer % 300;
                int frameIdx = 0;
                if (enemies[j].type == HARD || animCycle < 150) {
                    // Walk loop with spear at hip: Frames 1, 2, 7 (indices 0, 1, 6)
                    // Slowed way down: 50 ticks per step!
                    int walkFrames[] = {0, 1, 6};
                    frameIdx = walkFrames[(animCycle / 50) % 3];
                } else if (animCycle < 190) {
                    // Frame 3: Lift spear up over shoulder! (index 2, lasts 40 ticks)
                    frameIdx = 2;
                } else if (animCycle < 240) {
                    // Frame 4: Pull spear back horizontally & aim arm forward! (index 3, lasts 50 ticks)
                    frameIdx = 3;
                } else if (animCycle < 270) {
                    // Frame 5: Thrust arm forward, spear released! (index 4, lasts 30 ticks)
                    frameIdx = 4;
                } else {
                    // Frame 6: Follow-through arm outstretched! (index 5, lasts 30 ticks)
                    frameIdx = 5;
                }

                if (enemyRangedTextures[frameIdx]) {
                    const float left = enemies[j].x - 0.05f;
                    const float right = enemies[j].x + 0.05f;
                    const float top = enemies[j].y + 0.075f;
                    const float bottom = enemies[j].y - 0.075f;

                    // Flipped horizontally so robot faces left towards home base
                    const float quad[] = {
                        left, top, 1.0f, 1.0f,
                        right, top, 0.0f, 1.0f,
                        right, bottom, 0.0f, 0.0f,

                        left, top, 1.0f, 1.0f,
                        right, bottom, 0.0f, 0.0f,
                        left, bottom, 1.0f, 0.0f
                    };
                    glUseProgram(playerProgram);
                    glActiveTexture(GL_TEXTURE0);
                    glBindTexture(GL_TEXTURE_2D, enemyRangedTextures[frameIdx]);
                    glUniform1i(glGetUniformLocation(playerProgram, "playerImage"), 0);
                    glBindBuffer(GL_ARRAY_BUFFER, playerVBO);
                    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(quad), quad);
                    glBindVertexArray(playerVAO);
                    glDrawArrays(GL_TRIANGLES, 0, 6);
                }
            }
        }

        // Draw flying spears thrown by enemies
        if (spearTexture) {
            for (size_t k = 0; k < enemyProjectiles.size(); k++) {
                if (enemyProjectiles[k].active) {
                    const float left = enemyProjectiles[k].x - 0.045f;
                    const float right = enemyProjectiles[k].x + 0.045f;
                    const float top = enemyProjectiles[k].y + 0.008f;
                    const float bottom = enemyProjectiles[k].y - 0.008f;

                    // Flipped horizontally so spear tip flies left
                    const float quad[] = {
                        left, top, 1.0f, 1.0f,
                        right, top, 0.0f, 1.0f,
                        right, bottom, 0.0f, 0.0f,

                        left, top, 1.0f, 1.0f,
                        right, bottom, 0.0f, 0.0f,
                        left, bottom, 1.0f, 0.0f
                    };
                    glUseProgram(playerProgram);
                    glActiveTexture(GL_TEXTURE0);
                    glBindTexture(GL_TEXTURE_2D, spearTexture);
                    glUniform1i(glGetUniformLocation(playerProgram, "playerImage"), 0);
                    glBindBuffer(GL_ARRAY_BUFFER, playerVBO);
                    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(quad), quad);
                    glBindVertexArray(playerVAO);
                    glDrawArrays(GL_TRIANGLES, 0, 6);
                }
            }
        }

        if (isPaused || isGameOver || hasWon) {
            glUseProgram(shaderProgram);
            glBindVertexArray(VAO);
            glDrawArrays(GL_TRIANGLES, static_cast<GLint>(sceneVertexCount / 6),
                         static_cast<GLsizei>((vertices.size() - sceneVertexCount) / 6));
        }

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
    if (playerTexture) glDeleteTextures(1, &playerTexture);
    glDeleteTextures(4, backgroundTextures);
    glDeleteTextures(7, enemyRangedTextures);
    if (spearTexture) glDeleteTextures(1, &spearTexture);
    glDeleteVertexArrays(1, &playerVAO);
    glDeleteBuffers(1, &playerVBO);

    glfwTerminate();
    return 0;
}
