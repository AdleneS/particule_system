#ifndef VOX_H
#define VOX_H
#define CL_HPP_ENABLE_EXCEPTIONS
#define CL_HPP_TARGET_OPENCL_VERSION 120
#define CL_HPP_MINIMUM_OPENCL_VERSION 120
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <unordered_map>
#include <string>
#include <time.h>
#include <algorithm>
#include <thread>
#include <mutex>
#include <map>
#include <random>
#include <sstream>
#include <unistd.h>

#include "../gl3w/include/GL/gl3w.h"
#include "../gl3w/src/gl3w.c"

#include "shader.hpp"
#include "camera.hpp"

#include "../glfw/include/GLFW/glfw3.h"
#include <CL/opencl.hpp>
#include "../glm/glm.hpp"
#include "../glm/ext/matrix_clip_space.hpp"
#include "../glm/gtc/type_ptr.hpp"

#define GLSL(src) #src

struct Particle
{
    glm::vec2 Position, Velocity;
    glm::vec4 Color;
    float Life;

    Particle()
        : Position(0.0f), Velocity(0.0f), Color(1.0f), Life(0.0f) {}
};

void mouse_callback(GLFWwindow *window, double xpos, double ypos);
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);

float SCR_WIDTH = 1920;
float SCR_HEIGHT = 1080;
Camera camera(glm::vec3(0.0f, 128.0f, 0.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;
bool light = false;
float deltaTime = 0.0f;
float lastFrame = 0.0f;
bool stop = false;

#endif