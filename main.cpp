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

#define SRC_TITLE "Voxel Engine 1.0"
#define FPS 80
#define CHUNK_WIDTH 8
#define CHUNK_HEIGHT 8
#define CHUNK_LENGTH 8
#define RENDER_LIMIT_DISTANCE 64
#define CAMERA_WIDTH 1.0
#define CAMERA_HEIGHT 2.0
#define CAMERA_LENGTH 1.0

#include "./src/color.hpp"
#include "./src/lua.hpp"
#include "./src/shaderProgram.hpp"
#include "./src/geometry.hpp"
#include "./src/vertexArray.hpp"
#include "./src/texture.hpp"
#include "./src/AABB.hpp"
#include "./src/camera.hpp"
#include "./src/frameBox.hpp"
#include "./src/skybox.hpp"
#include "./src/chunk.hpp"
#include "./src/chunkManager.hpp"

void framebufferSizeCallback(GLFWwindow *window, int width, int height) {
    glViewport(0, 0, width, height);
}

void keyboardCallback(GLFWwindow *window, Camera &camera);
void mouseCallback(GLFWwindow *window, Camera &camera);

int main() {
    Lua lua;

    if (!lua.init()) {
        return -1;
    }

    if (!lua.load()) {
        return -1;
    }

    if (glfwInit() == GLFW_NOT_INITIALIZED) {
        std::cerr << "Falha ao iniciar GLFW." << std::endl;

        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, true);
#endif

    GLFWmonitor *monitor = glfwGetPrimaryMonitor();

    if (!monitor) {
        std::cerr << "Falha ao acessar monitor." << std::endl;

        return -1;
    }

    const GLFWvidmode *mode = glfwGetVideoMode(monitor);

    if (!mode) {
        std::cerr << "Falha ao acessar modo de vídeo." << std::endl;

        return -1;
    }

    glfwWindowHint(GLFW_RESIZABLE, false);

    int window_width = 1024, window_height = 768;

    if (static_cast<int>(lua.toNumber("config", "fullscreen")) == 1) {
        window_width = mode->width;
        window_height = mode->height;
    }

    GLFWwindow *window = glfwCreateWindow(window_width, window_height, SRC_TITLE, nullptr, nullptr);

    if (!window) {
        std::cerr << "Falha ao criar a janela de visualização." << std::endl;

        return -1;
    }

    glfwMakeContextCurrent(window);    
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
    glfwSetWindowPos(window, (mode->width - window_width) / 2, (mode->height - window_height) / 2);
    glfwSetCursorPos(window, window_width / 2, window_height / 2);

    if (glewInit() != GLEW_OK) {
        std::cerr << "Falha ao iniciar GLEW." << std::endl;

        return -1;
    }

    ShaderProgram chunkShaderProgram(lua.toString("shaderSource", "chunkVertex"), lua.toString("shaderSource", "chunkFragment"));
    ShaderProgram skyboxShaderProgram(lua.toString("shaderSource", "skyboxVertex"), lua.toString("shaderSource", "skyboxFragment"));
    ShaderProgram frameBoxShaderProgram(lua.toString("shaderSource", "frameBoxVertex"), lua.toString("shaderSource", "frameBoxFragment"));

    std::vector<std::string> skyboxTexture {
        "img/skybox/right.png",
        "img/skybox/left.png",
        "img/skybox/up.png",
        "img/skybox/down.png",
        "img/skybox/front.png",
        "img/skybox/back.png"
    };

    Texture textures;

    textures.load("./img/blocks.png", "BLOCKS");
    textures.load(skyboxTexture, "SKYBOX");

    Camera camera(window_width, window_height);

    camera.setPosition({-16.0f, 0.0f, 16.0f});
    camera.setFOV(lua.toNumber("config", "FOV"));

    Skybox skybox;

    ChunkManager chunkManager;

    chunkManager.addChunk({ 16.0f,  0.0f,  0.0f});
    chunkManager.addChunk({-16.0f,  0.0f,  0.0f});
    chunkManager.addChunk({  0.0f, 16.0f,  0.0f});
    chunkManager.addChunk({  0.0f,-16.0f,  0.0f});
    chunkManager.addChunk({  0.0f,  0.0f, 16.0f});
    chunkManager.addChunk({  0.0f,  0.0f,-16.0f});

    FrameBox frameBox;

    frameBox.setPosition({-2.0f, 0.0f, 0.0f});

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glFrontFace(GL_CCW);

    double lastFrame = 0.0, currentFrame = 0.0;

    while (!glfwWindowShouldClose(window)) {
        currentFrame = glfwGetTime();

        if ((currentFrame - lastFrame) > (1.0f / FPS)) {
            glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            keyboardCallback(window, camera);
            mouseCallback(window, camera);

            glDepthMask(false);

            skybox.render(skyboxShaderProgram, textures, camera);

            glDepthMask(true);
            glCullFace(GL_FRONT);
            
            chunkManager.render(chunkShaderProgram, textures, camera);
            frameBox.render(frameBoxShaderProgram, camera, GREEN, glm::vec3(1.0f));

            glCullFace(GL_BACK);
            glfwSwapBuffers(window);
            glfwPollEvents();

            lastFrame = currentFrame;
        }
    }

    glfwTerminate();

    return 0;
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

    camera.mouseUpdate(xOffset, yOffset, 0.1);
}