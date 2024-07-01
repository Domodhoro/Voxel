#include <iostream>
#include <string>
#include <vector>
#include <array>
#include <map>
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
#include <lua54/lua.hpp>
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
#define CHUNK_HEIGHT 16
#define CHUNK_LENGTH 16

#include "./src/shaderProgram.hpp"
#include "./src/blockFaces.hpp"
#include "./src/vertexArray.hpp"
#include "./src/texture.hpp"
#include "./src/camera.hpp"
#include "./src/AABB.hpp"
#include "./src/chunk.hpp"
#include "./src/skybox.hpp"

void framebufferSizeCallback(GLFWwindow *window, int width, int height);
void keyboardCallback(GLFWwindow *window, Camera &camera);
void mouseCallback(GLFWwindow *window, Camera &camera);

int main() {
    lua_State *L = luaL_newstate();

    if (!L) {
        std::cerr << "Falha ao criar estado lua." << std::endl;

        return -1;
    }

    luaL_openlibs(L);

    if (luaL_dofile(L, "./shaders/shaders.lua") != LUA_OK) {
        std::cerr << "Falha ao ler arquivo lua: " << lua_tostring(L, -1) << std::endl;

        return -1;
    }

    lua_getglobal(L, "chunkVertexShaderSource");

    const char *chunkVertexShaderSource = lua_tostring(L, -1);

    lua_pop(L, 1);

    lua_getglobal(L, "chunkFragmentShaderSource");

    const char *chunkFragmentShaderSource = lua_tostring(L, -1);

    lua_pop(L, 1);

    lua_getglobal(L, "skyboxVertexShaderSource");

    const char *skyboxVertexShaderSource = lua_tostring(L, -1);

    lua_pop(L, 1);

    lua_getglobal(L, "skyboxFragmentShaderSource");

    const char *skyboxFragmentShaderSource = lua_tostring(L, -1);

    lua_pop(L, 1);

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

#if _WIN32
    const GLFWvidmode *mode = glfwGetVideoMode(glfwGetPrimaryMonitor());

    glfwSetWindowPos(window, (mode->width - SRC_WIDTH) / 2, (mode->height - SRC_HEIGHT) / 2);
    glfwSetCursorPos(window, SRC_WIDTH / 2, SRC_HEIGHT / 2);
#endif

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    if (glewInit() != GLEW_OK) {
        std::cerr << "Falha ao iniciar GLEW." << std::endl;

        return -1;
    }

    ShaderProgram chunkShaderProgram(chunkVertexShaderSource, chunkFragmentShaderSource);
    ShaderProgram skyboxShaderProgram(skyboxVertexShaderSource, skyboxFragmentShaderSource);

    Texture textures;

    textures.load("./img/blocks.png", "BLOCKS");

    std::vector<std::string> skyboxTexture {
        "img/skybox/right.png",
        "img/skybox/left.png",
        "img/skybox/up.png",
        "img/skybox/down.png",
        "img/skybox/front.png",
        "img/skybox/back.png"
    };

    textures.load(skyboxTexture, "SKYBOX");

    Camera camera({0.0f, 0.0f, 20.0f});
    Chunk chunk({0.0f, 0.0f, 0.0f});
    Skybox skybox;

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glFrontFace(GL_CCW);

    double lastFrame = 0.0, currentFrame = 0.0;

    while (!glfwWindowShouldClose(window)) {
        currentFrame = glfwGetTime();

        if ((currentFrame - lastFrame) > (1.0f / FPS)) {
            glClearColor(0.5f, 0.5f, 1.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            keyboardCallback(window, camera);
            mouseCallback(window, camera);

            if (chunk.checkCollision(camera)) {
                
            }

            skybox.draw(skyboxShaderProgram, textures, camera);

            glCullFace(GL_FRONT);
            
            chunk.draw(chunkShaderProgram, textures, camera);

            glCullFace(GL_BACK);

            glfwSwapBuffers(window);
            glfwPollEvents();

            lastFrame = currentFrame;
        }
    }

    glfwTerminate();

    lua_close(L);

    return 0;
}

void framebufferSizeCallback(GLFWwindow *window, int width, int height) {
    glViewport(0, 0, width, height);
}

void keyboardCallback(GLFWwindow *window, Camera &camera) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }

    float cameraSpeed = 0.1f;

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

    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
        if (glfwGetKey(window, GLFW_KEY_SPACE)) {
            camera.toUp(2.0f * cameraSpeed);
        }

        if (glfwGetKey(window, GLFW_KEY_G)) {
            camera.toDown(2.0f * cameraSpeed);
        }
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