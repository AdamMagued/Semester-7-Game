#include "Drawing.h"
#include <cstdio>
#include <iostream>
#define STB_IMAGE_IMPLEMENTATION
#include "../../assets/textures/stb_image.h"

void addRectangle(std::vector<float> &vertices, float left, float top, float right, float bottom, float red,
                  float green, float blue) {
    const float rectangle[] = {left,  top,    0.0f, red, green, blue, right, top,    0.0f, red, green, blue,
                               right, bottom, 0.0f, red, green, blue, left,  top,    0.0f, red, green, blue,
                               right, bottom, 0.0f, red, green, blue, left,  bottom, 0.0f, red, green, blue};
    vertices.insert(vertices.end(), rectangle, rectangle + 36);
}

void addTriangle(std::vector<float> &vertices, float x1, float y1, float x2, float y2, float x3, float y3,
                 float red, float green, float blue) {
    const float triangle[] = {x1,  y1,    0.0f, red, green, blue, x2,  y2,    0.0f,
                              red, green, blue, x3,  y3,    0.0f, red, green, blue};
    vertices.insert(vertices.end(), triangle, triangle + 18);
}

void addLaneOutline(std::vector<float> &vertices, float top, float bottom, bool selected) {
    const float red = selected ? 1.0f : 0.18f;
    const float green = selected ? 0.78f : 0.88f;
    const float blue = selected ? 0.12f : 1.0f;
    const float thickness = selected ? 0.014f : 0.008f;
    addRectangle(vertices, -0.80f, top, 0.98f, top - thickness, red, green, blue);
    addRectangle(vertices, -0.80f, bottom + thickness, 0.98f, bottom, red, green, blue);
    addRectangle(vertices, -0.80f, top, -0.80f + thickness, bottom, red, green, blue);
    addRectangle(vertices, 0.98f - thickness, top, 0.98f, bottom, red, green, blue);
}

static const char *getGlyph(char c) {
    switch (c) {
    case 'A':
        return ".###.#...##...#######...##...##...#";
    case 'B':
        return "####.#...##...#####.#...##...#####.";
    case 'C':
        return ".#####....#....#....#....#.....####";
    case 'D':
        return "####.#...##...##...##...##...#####.";
    case 'E':
        return "######....#....####.#....#....#####";
    case 'F':
        return "######....#....####.#....#....#....";
    case 'G':
        return ".###.#...##....#.####...##...#.###.";
    case 'H':
        return "#...##...##...#######...##...##...#";
    case 'I':
        return "#####..#....#....#....#....#..#####";
    case 'J':
        return "..###...#....#....#....#.#..#..##..";
    case 'K':
        return "#...##..#.#.#..##...#.#..#..#.#...#";
    case 'L':
        return "#....#....#....#....#....#....#####";
    case 'M':
        return "#...###.###.#.##.#.##...##...##...#";
    case 'N':
        return "#...###..###..##.#.##..###..###...#";
    case 'O':
        return ".###.#...##...##...##...##...#.###.";
    case 'P':
        return "####.#...##...#####.#....#....#....";
    case 'Q':
        return ".###.#...##...##...##.#.##..#..##.#";
    case 'R':
        return "####.#...##...#####.#.#..#..#.#...#";
    case 'S':
        return ".#####....#.....###.....#....#####.";
    case 'T':
        return "#####..#....#....#....#....#....#..";
    case 'U':
        return "#...##...##...##...##...##...#.###.";
    case 'V':
        return "#...##...##...##...##...#.#.#...#..";
    case 'W':
        return "#...##...##...##.#.##.#.###.###...#";
    case 'X':
        return "#...##...#.#.#...#...#.#.#...##...#";
    case 'Y':
        return "#...##...#.#.#...#....#....#....#..";
    case 'Z':
        return "#####....#...#...#...#...#....#####";
    case '0':
        return ".###.#...##..###.#.###..##...#.###.";
    case '1':
        return "..#...##....#....#....#....#...###.";
    case '2':
        return ".###.#...#....#...#...#...#...#####";
    case '3':
        return "####.....#....#.###.....#....#####.";
    case '4':
        return "...#...##..#.#.#..#.#####...#....#.";
    case '5':
        return "######....#....####.....#....#####.";
    case '6':
        return ".###.#....#....####.#...##...#.###.";
    case '7':
        return "#####....#...#...#...#....#....#...";
    case '8':
        return ".###.#...##...#.###.#...##...#.###.";
    case '9':
        return ".###.#...##...#.####....#....#.###.";
    case '.':
        return "...........................##...##.";
    case ',':
        return "......................##...##...#..";
    case ':':
        return "......##...##........##...##.......";
    case '!':
        return "..#....#....#....#....#.........#..";
    case '?':
        return ".###.#...#....#...#...#.........#..";
    case '\'':
        return "..#....#...#.......................";
    case '-':
        return "...............#####...............";
    default:
        return "...................................";
    }
}

void drawText(std::vector<float> &vertices, const std::string &text, float startX, float startY,
              float pixelSize, float r, float g, float b) {
    float curX = startX;
    float pixelHeight = pixelSize * 5.0f / 7.0f;
    float pixelWidth = pixelHeight * 9.0f / 16.0f;
    for (size_t i = 0; i < text.size(); i++) {
        char c = text[i];
        if (c == ' ') {
            curX = curX + pixelWidth * 3.0f;
            continue;
        }
        const char *glyph = getGlyph(c);
        for (int row = 0; row < 7; row++) {
            for (int col = 0; col < 5; col++) {
                if (glyph[row * 5 + col] == '#') {
                    float px = curX + col * pixelWidth;
                    float py = startY - row * pixelHeight;
                    addRectangle(vertices, px, py, px + pixelWidth, py - pixelHeight, r, g, b);
                }
            }
        }
        curX = curX + pixelWidth * 6.0f;
    }
}

bool drawMenuButton(std::vector<float> &vertices, const std::string &text, float top, float mouseX,
                    float mouseY, bool clicked) {
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

void framebufferSizeCallback(GLFWwindow *, int width, int height) { glViewport(0, 0, width, height); }

const char *vertexShaderSrc = R"(
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;

out vec3 vertexColor;

void main() {
    gl_Position = vec4(aPos, 1.0);
    vertexColor = aColor;
}
)";

const char *fragmentShaderSrc = R"(
#version 330 core
in vec3 vertexColor;
out vec4 FragColor;

void main() {
    FragColor = vec4(vertexColor, 1.0);
}
)";

const char *playerVertexShaderSrc = R"(
#version 330 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aUV;
out vec2 uv;
void main() { gl_Position = vec4(aPos, 0.0, 1.0); uv = aUV; }
)";

const char *playerFragmentShaderSrc = R"(
#version 330 core
in vec2 uv;
out vec4 FragColor;
uniform sampler2D playerImage;
uniform float damageAmount;
uniform float brightness;
void main() {
    vec4 color = texture(playerImage, uv);
    color.rgb = mix(color.rgb, vec3(1.0, 0.0, 0.0), damageAmount);
    color.rgb *= brightness;
    FragColor = color;
}
)";

GLuint compileShader(GLenum type, const char *src) {
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
    if (!success) {
        glDeleteShader(shader);
        return 0;
    }
    return shader;
}

GLuint loadTexture(const char *path, int *imageWidth, int *imageHeight) {
    int width, height, channels;
    unsigned char *image = stbi_load(path, &width, &height, &channels, 4);
    if (!image) {
        std::cerr << "Couldn't load " << path << ": " << stbi_failure_reason() << '\n';
        return 0;
    }

    if (imageWidth)
        *imageWidth = width;
    if (imageHeight)
        *imageHeight = height;
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
    stbi_image_free(image);
    return texture;
}

void drawWrappedText(std::vector<float> &vertices, const std::string &text, float left, float top,
                     float width, float size, float lineSpacing) {
    std::string line;
    std::string word;
    float usedWidth = 0;
    for (size_t i = 0; i <= text.size(); ++i) {
        if (i < text.size() && text[i] != ' ') {
            word += text[i];
            continue;
        }
        float wordWidth = word.size() * size * 5.0f / 7.0f * 6 * 9.0f / 16.0f;
        float gap = line.empty() ? 0 : size * 5.0f / 7.0f * 3 * 9.0f / 16.0f;
        if (!line.empty() && usedWidth + gap + wordWidth > width) {
            drawText(vertices, line, left, top, size);
            top -= lineSpacing;
            line.clear();
            usedWidth = 0;
            gap = 0;
        }
        if (!line.empty())
            line += ' ';
        line += word;
        usedWidth += gap + wordWidth;
        word.clear();
    }
    if (!line.empty())
        drawText(vertices, line, left, top, size);
}

GLuint createProgram(const char *vertexSource, const char *fragmentSource) {
    GLuint vertex = compileShader(GL_VERTEX_SHADER, vertexSource);
    GLuint fragment = compileShader(GL_FRAGMENT_SHADER, fragmentSource);
    if (!vertex || !fragment) {
        glDeleteShader(vertex);
        glDeleteShader(fragment);
        return 0;
    }
    GLuint program = glCreateProgram();
    glAttachShader(program, vertex);
    glAttachShader(program, fragment);
    glLinkProgram(program);
    glDeleteShader(vertex);
    glDeleteShader(fragment);
    int success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        char log[512];
        glGetProgramInfoLog(program, 512, nullptr, log);
        std::cerr << "Shader link error: " << log << '\n';
        glDeleteProgram(program);
        return 0;
    }
    return program;
}

void drawGeometry(const std::vector<float> &vertices, GLuint program, GLuint vao, GLuint vbo,
                  size_t firstFloat) {
    glUseProgram(program);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_DYNAMIC_DRAW);
    glDrawArrays(GL_TRIANGLES, static_cast<GLint>(firstFloat / 6),
                 static_cast<GLsizei>((vertices.size() - firstFloat) / 6));
}
