#ifndef VOX_H
#define VOX_H
#define CL_HPP_ENABLE_EXCEPTIONS
#define CL_TARGET_OPENCL_VERSION 120
#ifdef _WIN32
#define WINDOWS_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#endif
#ifdef __APPLE__
#include <OpenCL/cl.h>
#else
#include <CL/cl.h>
#include <CL/cl_gl.h>
#include <oclUtils.h>
#endif
#if defined(__APPLE__) || defined(MACOSX)
#define GL_SHARING_EXTENSION "cl_APPLE_gl_sharing"
#else
#define GL_SHARING_EXTENSION "cl_khr_gl_sharing"
#endif
#define VECTOR_SIZE 1024
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
#include <buffer.hpp>

#include "../glfw/include/GLFW/glfw3.h"
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
void Cleanup(int iExitCode);
void (*pCleanup)(int) = &Cleanup;
cl_int getPlatformID(cl_platform_id *clSelectedPlatformID);
void runKernel();
char *oclLoadProgSource(const char *cFilename, const char *cPreamble, size_t *szFinalLength);

unsigned int SCR_WIDTH = 1920;
unsigned int SCR_HEIGHT = 1080;
Camera camera(glm::vec3(0.0f, 0.0f, 0.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;
bool light = false;
float deltaTime = 0.0f;
float lastFrame = 0.0f;
bool stop = false;

GLuint vbo;
GLuint vao;
const unsigned int mesh_width = 1000;
const unsigned int mesh_height = 1000;
// OpenCL vars
cl_platform_id cpPlatform;
cl_context cxGPUContext;
cl_device_id *cdDevices;
cl_uint uiDevCount;
cl_command_queue cqCommandQueue;
cl_kernel ckKernel;
cl_mem vbo_cl;
cl_program cpProgram;
cl_int ciErrNum;
char *cPathAndName = NULL; // var for full paths to data, src, etc.
char *cSourceCL = NULL;    // Buffer to hold source for compilation
size_t szGlobalWorkSize[] = {mesh_width, mesh_height};
const char *cExecutableName = NULL;

float anim = 0.0;

#endif