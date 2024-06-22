#include <iostream>
#include <vector>
#include <algorithm>

#ifdef _WIN32
#define SDL_MAIN_HANDLED
#endif

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#ifdef _WIN32
#include <lua/lua.hpp>
#elif __linux__
extern "C" {
#include "./lua54/lua.h"
#include "./lua54/lualib.h"
#include "./lua54/lauxlib.h"
}
#endif

#define FNL_IMPL
#include "./FastNoiseLite.h"

#define SRC_WIDTH 1200
#define SRC_HEIGHT 600
#define FPS 60
#define CHUNK_WIDTH 16
#define CHUNK_HEIGHT 128
#define CHUNK_LENGTH 16

#include "./shader.hpp"
#include "./vertex_array.hpp"
#include "./texture.hpp"
#include "./camera.hpp"
#include "./chunk.hpp"

lua_State *initLua();

void framebufferSizeCallback(GLFWwindow *window, int width, int height);
void keyboardCallback(GLFWwindow *window, Camera &camera);
void mouseCallback(GLFWwindow *window, Camera &camera);

Shader createShaderProgram(lua_State *L);

int main() {
    lua_State *L = initLua();

    if (!L) {
        return -1;
    }

    if (glfwInit() == GLFW_NOT_INITIALIZED) {
        std::cerr << "Falha ao iniciar GLFW." << std::endl;

        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, false);

    GLFWwindow *window = glfwCreateWindow(SRC_WIDTH, SRC_HEIGHT, "Voxel Engine 1.0", nullptr, nullptr);

    if (!window) {
        std::cerr << "Falha ao criar a janela de visualização." << std::endl;

        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

    const GLFWvidmode *mode = glfwGetVideoMode(glfwGetPrimaryMonitor());

    glfwSetWindowPos(window, (mode->width - SRC_WIDTH) / 2, (mode->height - SRC_HEIGHT) / 2);
    glfwSetCursorPos(window, SRC_WIDTH / 2, SRC_HEIGHT / 2);

    if (glewInit() != GLEW_OK) {
        std::cerr << "Falha ao iniciar GLEW." << std::endl;

        return -1;
    }

    Shader shader = createShaderProgram(L);

    Texture texture("./block.png");

    Camera camera({0.0f, 0.0f, -3.0f});

    Chunk chunk({0.0f, 0.0f, 0.0f});

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);

    double lastFrame = 0.0, currentFrame = 0.0;

    while (!glfwWindowShouldClose(window)) {
        currentFrame = glfwGetTime();

        if ((currentFrame - lastFrame) > (1.0f / FPS)) {
            glClearColor(0.5f, 0.5f, 1.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            keyboardCallback(window, camera);
            mouseCallback(window, camera);

            chunk.draw(shader, texture, camera);

            glfwSwapBuffers(window);
            glfwPollEvents();

            lastFrame = currentFrame;
        }
    }

    return 0;
}

lua_State *initLua() {
    lua_State *L = luaL_newstate();

    if (!L) {
        std::cerr << "Falha ao criar estado lua." << std::endl;

        return nullptr;
    }

    luaL_openlibs(L);

    if (luaL_dofile(L, "./script.lua") != LUA_OK) {
        std::cerr << "Falha ao ler arquivo lua: " << lua_tostring(L, -1) << std::endl;

        return nullptr;
    }

    return L;
}

void framebufferSizeCallback(GLFWwindow *window, int width, int height) {
    glViewport(0, 0, width, height);
}

void keyboardCallback(GLFWwindow *window, Camera &camera) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }

    float cameraSpeed = 0.2f;

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        camera.keyboardUpdate(CAMERA_MOVEMENTS::FORWARD, cameraSpeed);
    }

    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        camera.keyboardUpdate(CAMERA_MOVEMENTS::BACKWARD, cameraSpeed);
    }

    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        camera.keyboardUpdate(CAMERA_MOVEMENTS::RIGHT, cameraSpeed);
    }

    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        camera.keyboardUpdate(CAMERA_MOVEMENTS::LEFT, cameraSpeed);
    }
}

void mouseCallback(GLFWwindow *window, Camera &camera) {
    double x = 0.0, y = 0.0;

    glfwGetCursorPos(window, &x, &y);

    static bool firstMouse = true;
    static double lastX = 0.0, lastY = 0.0;

    if (firstMouse) {
        lastX = x;
        lastY = y;
        firstMouse = false;
    }

    double xOffset = x - lastX, yOffset = lastY -y;

    lastX = x;
    lastY = y;

    double cameraSensitivity = 0.1;

    camera.mouseUpdate(xOffset, yOffset, cameraSensitivity);
}

Shader createShaderProgram(lua_State *L) {
    lua_getglobal(L, "vertexShaderSource");

    const char *vertexShaderSource = lua_tostring(L, -1);

    lua_pop(L, 1);

    lua_getglobal(L, "fragmentShaderSource");

    const char *fragmentShaderSource = lua_tostring(L, -1);

    lua_pop(L, 1);

    return Shader(vertexShaderSource, fragmentShaderSource);
}