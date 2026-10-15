#define GLAD_GL_IMPLEMENTATION
#include "renderer/Renderer.h"
#include "game/entities/enemies/Enemy.h"
#include <GLFW/glfw3.h>
#include <iostream>

int main() {
    if (!glfwInit()) {
        std::cerr << "Failed to init GLFW\n";
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    GLFWwindow* window = glfwCreateWindow(800, 450, "Game", nullptr, nullptr);
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

    // The framebuffer can be bigger than the window on Retina screens
    int fbWidth, fbHeight;
    glfwGetFramebufferSize(window, &fbWidth, &fbHeight);
    glViewport(0, 0, fbWidth, fbHeight);

    {
        Renderer renderer;
        Enemy easyEnemy(1, EASY);
        Enemy mediumEnemy(2, MEDIUM);
        Enemy hardEnemy(3, HARD);
        double lastTime = glfwGetTime();

        while (!glfwWindowShouldClose(window)) {
            double now = glfwGetTime();
            float dt = (float)(now - lastTime);
            lastTime = now;

            glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);

            easyEnemy.update(dt);
            mediumEnemy.update(dt);
            hardEnemy.update(dt);

            easyEnemy.draw(renderer);
            mediumEnemy.draw(renderer);
            hardEnemy.draw(renderer);

            glfwSwapBuffers(window);
            glfwPollEvents();
        }
    }

    glfwTerminate();
    return 0;
}
