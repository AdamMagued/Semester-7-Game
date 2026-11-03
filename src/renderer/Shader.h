#pragma once

#include <glad/gl.h>
#include <glm/glm.hpp>

class Shader {
private:
    GLuint program;

public:
    Shader(const char* vertexSrc, const char* fragmentSrc);
    ~Shader();

    void use() const;
    void setMat4(const char* name, const glm::mat4& value) const;
    void setVec4(const char* name, const glm::vec4& value) const;
};
