#pragma once

#include "../gl3w/include/GL/gl3w.h"
#include "../glm/glm.hpp"
#include <cstdio>

const float pos[] = {
    0.5, 0.0, 0.0};

class Buffer
{
private:
    /* data */
public:
    GLuint VAO;
    GLuint VBO;
    glm::vec3 Position;
    glm::mat4x4 mat;
    Buffer();
    ~Buffer();
    void loadVBO();
    void deleteVBO();
};