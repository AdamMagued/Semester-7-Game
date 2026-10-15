#pragma once

#include "Shader.h"

class Renderer {
private:
    Shader shader;
    GLuint VAO;
    GLuint VBO;

public:
    Renderer();
    ~Renderer();

    void drawTriangle(float x, float y, float width, float height, glm::vec4 color);
};
