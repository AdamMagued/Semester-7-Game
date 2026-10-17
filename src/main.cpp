#define GLAD_GL_IMPLEMENTATION
#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#define STB_IMAGE_IMPLEMENTATION
#include "../assets/textures/stb_Image.h"
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

    // Player setup
    melee_player player1;
    bool isSpawned = false;      // Player has not been placed yet
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

        // Click mouse to spawn player on hovered lane!
        int mouseState = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
        if (mouseState == GLFW_PRESS && wasMouseDown == false) {
            wasMouseDown = true; // Registered click

            if (hoveredLane != -1) {
                player1.setLane(hoveredLane);
                isSpawned = true;
            }
        }
        if (mouseState == GLFW_RELEASE) {
            wasMouseDown = false; // Reset on release
        }

        // Check keyboard for Jump (W) and Duck (S)
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
            player1.jump();
        } else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
            player1.duck();
        } else {
            player1.stand();
        }

        // Only move and draw the player IF they have been spawned!
        if (isSpawned == true) {
            player1.move();
            player1.updateJump(); // Handle jump countdown (Dino game style)

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

        if (isSpawned && playerTexture) {
            const float left = player1.x - 0.04f, right = player1.x + 0.04f;
            const float top = player1.y + player1.yOffset + 0.06f;
            const float bottom = player1.y + player1.yOffset - 0.06f;
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

        glfwSwapBuffers(window);
        glfwPollEvents();
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
