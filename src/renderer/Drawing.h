#pragma once
#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <string>
#include <vector>
void addRectangle(std::vector<float> &, float, float, float, float, float, float, float);
void addTriangle(std::vector<float> &, float, float, float, float, float, float, float, float, float);
void addLaneOutline(std::vector<float> &, float, float, bool);
void drawText(std::vector<float> &, const std::string &, float, float, float, float = 1, float = 1,
              float = 1);
bool drawMenuButton(std::vector<float> &, const std::string &, float, float, float, bool);
void framebufferSizeCallback(GLFWwindow *, int, int);
GLuint compileShader(GLenum, const char *);
GLuint loadTexture(const char *, int *imageWidth = nullptr, int *imageHeight = nullptr);
extern const char *vertexShaderSrc;
extern const char *fragmentShaderSrc;
extern const char *playerVertexShaderSrc;
extern const char *playerFragmentShaderSrc;

void drawWrappedText(std::vector<float> &, const std::string &, float, float, float, float, float);

GLuint createProgram(const char *, const char *);
void drawGeometry(const std::vector<float> &, GLuint, GLuint, GLuint, size_t firstFloat = 0);
