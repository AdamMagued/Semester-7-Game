#include "Renderer.h"
#include <glm/gtc/matrix_transform.hpp>

static const char* vertexSource = R"(
#version 330 core
layout (location = 0) in vec3 aPos;
uniform mat4 model;
uniform mat4 projection;
void main() {
    gl_Position = projection * model * vec4(aPos, 1.0);
}
)";

static const char* fragmentSource = R"(
#version 330 core
out vec4 FragColor;
uniform vec4 color;
void main() {
    FragColor = color;
}
)";

Renderer::Renderer() : shader(vertexSource, fragmentSource) {
    // Camera: the world is 16 wide and 9 tall, with y = 0 at the top
    glm::mat4 projection = glm::ortho(0.0f, 16.0f, 9.0f, 0.0f);
    shader.use();
    shader.setMat4("projection", projection);

    // A triangle with its point facing left, because enemies walk left
    float vertices[] = {
        -0.5f,  0.0f, 0.0f,
         0.5f,  0.5f, 0.0f,
         0.5f, -0.5f, 0.0f
    };

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

Renderer::~Renderer() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
}

void Renderer::drawTriangle(float x, float y, float width, float height, glm::vec4 color) {
    // Move to (x, y), then scale to the wanted size
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(x, y, 0.0f));
    model = glm::scale(model, glm::vec3(width, height, 1.0f));

    shader.use();
    shader.setMat4("model", model);
    shader.setVec4("color", color);

    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 3);
}
