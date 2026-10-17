#define GLAD_GL_IMPLEMENTATION
#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include "game/entities/players/melee/melee_player.h"
#include "game/entities/players/defense/defense_player.h"
#include "game/entities/players/ranged/ranged_player.h"

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

GLuint compileShader(GLenum type, const char* src) {
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

    // Player and UI Card setup
    std::vector<playerClass*> players;
    int selectedPlayerType = 0;  // 0 = Melee (Key 1), 1 = Ranged (Key 2), 2 = Defense (Key 3)
    int coins = 100;             // Starting coins
    bool wasMouseDown = false;   // To detect a fresh mouse click

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
        // Card 1: Melee (Cost 10)
        if (selectedPlayerType == 0) {
            addRectangle(vertices, -0.76f, 0.93f, -0.54f, 0.69f, 1.0f, 1.0f, 1.0f); // White selection border
        }
        if (coins >= 10) {
            addRectangle(vertices, -0.75f, 0.92f, -0.55f, 0.70f, 0.85f, 0.25f, 0.25f); // Red card (unlocked)
        } else {
            addRectangle(vertices, -0.75f, 0.92f, -0.55f, 0.70f, 0.40f, 0.40f, 0.40f); // Grayscale (locked)
        }

        // Card 2: Ranged (Cost 20)
        if (selectedPlayerType == 1) {
            addRectangle(vertices, -0.51f, 0.93f, -0.29f, 0.69f, 1.0f, 1.0f, 1.0f); // White selection border
        }
        if (coins >= 20) {
            addRectangle(vertices, -0.50f, 0.92f, -0.30f, 0.70f, 0.25f, 0.45f, 0.85f); // Blue card (unlocked)
        } else {
            addRectangle(vertices, -0.50f, 0.92f, -0.30f, 0.70f, 0.40f, 0.40f, 0.40f); // Grayscale (locked)
        }

        // Card 3: Defense (Cost 30)
        if (selectedPlayerType == 2) {
            addRectangle(vertices, -0.26f, 0.93f, -0.04f, 0.69f, 1.0f, 1.0f, 1.0f); // White selection border
        }
        if (coins >= 30) {
            addRectangle(vertices, -0.25f, 0.92f, -0.05f, 0.70f, 0.85f, 0.65f, 0.20f); // Gold card (unlocked)
        } else {
            addRectangle(vertices, -0.25f, 0.92f, -0.05f, 0.70f, 0.40f, 0.40f, 0.40f); // Grayscale (locked)
        }

        // Check lane hovering
        int hoveredLane = -1;
        if (x >= -0.80f && x <= 0.98f) {
            if (y <= 0.62f && y >= 0.20f) hoveredLane = 0;
            else if (y < 0.20f && y >= -0.28f) hoveredLane = 1;
            else if (y < -0.28f && y >= -0.82f) hoveredLane = 2;
        }

        if (hoveredLane == 0)
            addRectangle(vertices, -0.80f, 0.62f, 0.98f, 0.20f, 0.32f, 0.78f, 0.30f);
        else if (hoveredLane == 1)
            addRectangle(vertices, -0.80f, 0.20f, 0.98f, -0.28f, 0.32f, 0.78f, 0.30f);
        else if (hoveredLane == 2)
            addRectangle(vertices, -0.80f, -0.28f, 0.98f, -0.82f, 0.32f, 0.78f, 0.30f);

        // Select player type using keyboard 1, 2, 3
        if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) {
            selectedPlayerType = 0;
        } else if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) {
            selectedPlayerType = 1;
        } else if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS) {
            selectedPlayerType = 2;
        }

        // Click mouse: select card OR spawn player on lane
        int mouseState = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
        if (mouseState == GLFW_PRESS && wasMouseDown == false) {
            wasMouseDown = true; // Registered click

            // Click on top-left UI cards to select
            if (y >= 0.70f && y <= 0.92f) {
                if (x >= -0.75f && x <= -0.55f && coins >= 10) {
                    selectedPlayerType = 0;
                } else if (x >= -0.50f && x <= -0.30f && coins >= 20) {
                    selectedPlayerType = 1;
                } else if (x >= -0.25f && x <= -0.05f && coins >= 30) {
                    selectedPlayerType = 2;
                }
            }
            // Click on a lane to spawn the selected player
            else if (hoveredLane != -1) {
                if (selectedPlayerType == 0 && coins >= 10) {
                    melee_player* p = new melee_player();
                    p->setLane(hoveredLane);
                    players.push_back(p);
                    coins = coins - 10;
                } else if (selectedPlayerType == 1 && coins >= 20) {
                    ranged_player* p = new ranged_player();
                    p->setLane(hoveredLane);
                    players.push_back(p);
                    coins = coins - 20;
                } else if (selectedPlayerType == 2 && coins >= 30) {
                    defense_player* p = new defense_player();
                    p->setLane(hoveredLane);
                    players.push_back(p);
                    coins = coins - 30;
                }
            }
        }
        if (mouseState == GLFW_RELEASE) {
            wasMouseDown = false; // Reset on release
        }

        // Check keyboard for Jump (W) and Duck (S)
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
            for (size_t i = 0; i < players.size(); i++) {
                players[i]->jump();
            }
        } else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
            for (size_t i = 0; i < players.size(); i++) {
                players[i]->duck();
            }
        } else {
            for (size_t i = 0; i < players.size(); i++) {
                players[i]->stand();
            }
        }

        // Move and draw each spawned player
        for (size_t i = 0; i < players.size(); i++) {
            players[i]->move();
            players[i]->updateJump();

            float drawY = players[i]->y + players[i]->yOffset;
            float r = 0.9f;
            float g = 0.2f;
            float b = 0.2f; // Red for Melee

            if (players[i]->playerType == playerClass::RANGED) {
                r = 0.2f; g = 0.5f; b = 0.9f; // Blue for Ranged
            } else if (players[i]->playerType == playerClass::DEFENSE) {
                r = 0.85f; g = 0.65f; b = 0.2f; // Gold for Defense
            }

            addRectangle(vertices, players[i]->x - 0.04f, drawY + 0.06f,
                                   players[i]->x + 0.04f, drawY - 0.06f,
                                   r, g, b);
        }

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_DYNAMIC_DRAW);

        glClearColor(0.15f, 0.35f, 0.12f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(vertices.size() / 6));

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    for (size_t i = 0; i < players.size(); i++) {
        delete players[i];
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);

    glfwTerminate();
    return 0;
}
