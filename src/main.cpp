#define GLAD_GL_IMPLEMENTATION
#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#define STB_IMAGE_IMPLEMENTATION
#include "../assets/textures/stb_image.h"
#include "game/entities/players/melee/melee_player.h"
#include "game/entities/players/defense/defense_player.h"
#include "game/entities/players/ranged/ranged_player.h"
#include "game/entities/enemies/Enemy.h"

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

    GLFWwindow* window = glfwCreateWindow(800, 600, "Triangle", nullptr, nullptr);
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
    std::vector<Collectible> collectibles;
    std::vector<Enemy> enemies;
    enemies.push_back(Enemy(0, EASY));
    enemies.push_back(Enemy(1, MEDIUM));
    enemies.push_back(Enemy(2, HARD));

    int selectedPlayerType = 0;  // 0 = Melee (Key 1), 1 = Ranged (Key 2), 2 = Defense (Key 3)
    int coins = 100;             // Starting coins
    bool wasMouseDown = false;   // To detect a fresh mouse click
    bool selectedLanes[3] = {true, false, false}; // Active selected lanes (can toggle multiple)
    bool isPaused = false;
    bool wasEscDown = false;
    bool isGameOver = false;

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

    // --- Render loop ---
    while (!glfwWindowShouldClose(window)) {
        // Remove last frame's highlight before checking the mouse again.
        vertices.resize(boardSize);

        double mouseX, mouseY;
        int windowWidth, windowHeight;
        glfwGetCursorPos(window, &mouseX, &mouseY);
        glfwGetWindowSize(window, &windowWidth, &windowHeight);

        float x = 2.0f * static_cast<float>(mouseX) / windowWidth - 1.0f;
        float y = 1.0f - 2.0f * static_cast<float>(mouseY) / windowHeight;

        // --- Draw UI Cards in top-left ---
        // Card 1: Melee (Cost 10) - Red Box
        if (selectedPlayerType == 0) {
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
        if (selectedPlayerType == 1) {
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
        if (selectedPlayerType == 2) {
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
        drawText(vertices, coinStr, 0.40f, 0.86f, 0.0035f, 1.0f, 0.9f, 0.1f);

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
                if (!isGameOver) {
                    isPaused = !isPaused;
                }
            }
        } else {
            wasEscDown = false;
        }

        // Check R key to restart game
        if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
            if (isPaused || isGameOver) {
                for (size_t p = 0; p < players.size(); p++) {
                    delete players[p];
                }
                players.clear();
                projectiles.clear();
                collectibles.clear();

                enemies.clear();
                enemies.push_back(Enemy(0, EASY));
                enemies.push_back(Enemy(1, MEDIUM));
                enemies.push_back(Enemy(2, HARD));

                coins = 100;
                selectedPlayerType = 0;
                selectedLanes[0] = true;
                selectedLanes[1] = false;
                selectedLanes[2] = false;
                isPaused = false;
                isGameOver = false;
            }
        }

        // Check if any active enemy breached the home base (x <= -0.80f)
        for (size_t j = 0; j < enemies.size(); j++) {
            if (enemies[j].active && enemies[j].x <= -0.80f) {
                isGameOver = true;
                break;
            }
        }

        // Select player type using keyboard 1, 2, 3
        if (!isPaused && !isGameOver) {
            if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) {
                selectedPlayerType = 0;
            } else if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) {
                selectedPlayerType = 1;
            } else if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS) {
                selectedPlayerType = 2;
            }
        }

        // Click mouse: select card OR select lane & spawn player
        int mouseState = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
        if (!isPaused && !isGameOver && mouseState == GLFW_PRESS && wasMouseDown == false) {
            wasMouseDown = true; // Registered click

            // Click on top-left UI cards to select
            if (y >= 0.66f && y <= 0.93f) {
                if (x >= -0.75f && x <= -0.55f && coins >= 10) {
                    selectedPlayerType = 0;
                } else if (x >= -0.50f && x <= -0.30f && coins >= 20) {
                    selectedPlayerType = 1;
                } else if (x >= -0.25f && x <= -0.05f && coins >= 30) {
                    selectedPlayerType = 2;
                }
            }
            // Click on a lane: toggle lane selection & spawn player
            else if (hoveredLane != -1) {
                selectedLanes[hoveredLane] = !selectedLanes[hoveredLane];

                if (selectedPlayerType == 0 && coins >= 10) {
                    selectedLanes[hoveredLane] = true;
                    melee_player* p = new melee_player();
                    p->setLane(hoveredLane);
                    players.push_back(p);
                    coins = coins - 10;
                } else if (selectedPlayerType == 1 && coins >= 20) {
                    selectedLanes[hoveredLane] = true;
                    ranged_player* p = new ranged_player();
                    p->setLane(hoveredLane);
                    players.push_back(p);
                    coins = coins - 20;
                } else if (selectedPlayerType == 2 && coins >= 30) {
                    selectedLanes[hoveredLane] = true;
                    defense_player* p = new defense_player();
                    p->setLane(hoveredLane);
                    // Place Defense at clicked X position on the lane!
                    float placeX = x;
                    if (placeX < -0.75f) placeX = -0.75f;
                    if (placeX > 0.70f) placeX = 0.70f;
                    p->x = placeX;
                    players.push_back(p);
                    coins = coins - 30;
                }
            }
        }
        if (mouseState == GLFW_RELEASE) {
            wasMouseDown = false; // Reset on release
        }

        // Check keyboard for Jump (W / Up) and Duck (S / Down) across all selected lanes
        if (!isPaused && !isGameOver) {
            if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
                for (size_t i = 0; i < players.size(); i++) {
                    if (selectedLanes[players[i]->lane]) {
                        players[i]->jump();
                    }
                }
            } else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
                for (size_t i = 0; i < players.size(); i++) {
                    if (selectedLanes[players[i]->lane]) {
                        players[i]->duck();
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
            bool inMeleeCombat = false;

            // Update player combat and movement if not paused or game over
            if (!isPaused && !isGameOver) {
                // Check melee attack collision with enemies
                if (players[i]->playerType == playerClass::MELEE) {
                    melee_player* mp = static_cast<melee_player*>(players[i]);
                    if (mp->attackCooldown > 0) {
                        mp->attackCooldown = mp->attackCooldown - 1;
                    }

                    for (size_t j = 0; j < enemies.size(); j++) {
                        if (enemies[j].active && enemies[j].lane == mp->lane) {
                            float dist = enemies[j].x - mp->x;
                            if (dist > 0.0f && dist < 0.12f) {
                                inMeleeCombat = true; // Enemy in melee combat range!
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
                                // Visual fist punch towards enemy
                                float punchY = mp->y + mp->yOffset;
                                addRectangle(vertices, mp->x + 0.04f, punchY + 0.02f,
                                                       mp->x + 0.09f, punchY - 0.02f,
                                                       1.0f, 0.4f, 0.4f);
                                break;
                            }
                        }
                    }
                }

                // Only advance forward if not currently in melee combat
                if (inMeleeCombat == false) {
                    players[i]->move();
                }
                players[i]->updateJump();
            }

            // Remove dead players
            if (players[i]->health <= 0) {
                delete players[i];
                players.erase(players.begin() + i);
                i = i - 1;
                continue;
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
            float barY = drawY + 0.085f;
            addRectangle(vertices, px - 0.04f, barY + 0.006f, px + 0.04f, barY - 0.006f, 0.35f, 0.1f, 0.1f);
            float hpRatio = (float)players[i]->health / (float)players[i]->maxHealth;
            if (hpRatio < 0.0f) hpRatio = 0.0f;
            if (hpRatio > 1.0f) hpRatio = 1.0f;
            addRectangle(vertices, px - 0.04f, barY + 0.006f, px - 0.04f + 0.08f * hpRatio, barY - 0.006f, 0.2f, 0.85f, 0.2f);

            // Check if protected behind a defense player on the same lane
            bool protectedByDefense = false;
            if (players[i]->playerType != playerClass::DEFENSE) {
                for (size_t d = 0; d < players.size(); d++) {
                    if (players[d]->playerType == playerClass::DEFENSE && players[d]->lane == players[i]->lane) {
                        if (players[i]->x < players[d]->x) {
                            protectedByDefense = true;
                            break;
                        }
                    }
                }
            }
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
            if (!isPaused && !isGameOver && players[i]->playerType == playerClass::RANGED) {
                ranged_player* rp = static_cast<ranged_player*>(players[i]);
                if (rp->shouldShoot()) {
                    Projectile proj;
                    proj.x = rp->x + 0.05f;
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
                if (!isPaused && !isGameOver) {
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
                if (!isPaused && !isGameOver) {
                    for (size_t p = 0; p < players.size(); p++) {
                        if (players[p]->lane == collectibles[i].lane) {
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
                if (!isPaused && !isGameOver) {
                    enemies[j].moveLeft();
                }

                // Draw enemy (color becomes deeper red as health drops)
                float healthRatio = (float)enemies[j].health / 5.0f;
                float er = 0.55f + (1.0f - healthRatio) * 0.40f;
                float eg = 0.15f * healthRatio;
                float eb = 0.55f * healthRatio;

                addRectangle(vertices, enemies[j].x - 0.04f, enemies[j].y + 0.06f,
                                       enemies[j].x + 0.04f, enemies[j].y - 0.06f,
                                       er, eg, eb);
            }
        }

        // Draw Pause overlay
        if (isPaused) {
            addRectangle(vertices, -0.45f, 0.25f, 0.45f, -0.25f, 0.08f, 0.08f, 0.12f);
            drawText(vertices, "PAUSED", -0.16f, 0.15f, 0.005f, 1.0f, 1.0f, 1.0f);
            drawText(vertices, "ESC TO RESUME", -0.28f, 0.00f, 0.0035f, 0.85f, 0.85f, 0.85f);
            drawText(vertices, "R TO RESTART", -0.26f, -0.12f, 0.0035f, 0.85f, 0.85f, 0.85f);
        }

        // Draw Game Over overlay
        if (isGameOver) {
            addRectangle(vertices, -0.45f, 0.25f, 0.45f, -0.25f, 0.15f, 0.05f, 0.05f);
            drawText(vertices, "GAME OVER", -0.22f, 0.15f, 0.005f, 1.0f, 0.25f, 0.25f);
            drawText(vertices, "HOUSE BREACHED", -0.30f, 0.00f, 0.0035f, 1.0f, 0.85f, 0.85f);
            drawText(vertices, "PRESS R TO RESTART", -0.38f, -0.12f, 0.0035f, 1.0f, 1.0f, 1.0f);
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
        glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(vertices.size() / 6));

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
    glDeleteVertexArrays(1, &playerVAO);
    glDeleteBuffers(1, &playerVBO);

    glfwTerminate();
    return 0;
}
