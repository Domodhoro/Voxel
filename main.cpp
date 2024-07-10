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
#define SRC_WIDTH 1024
#define SRC_HEIGHT 768
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
#include "./src/inputs.hpp"

int main() {
    lua_State *L = lua_init();

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

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, true);
#endif

    GLFWmonitor *monitor = glfwGetPrimaryMonitor();

    if (!monitor) {
        std::cerr << "Falha ao acessar monitor." << std::endl;

        return -1;
    }

    glfwWindowHint(GLFW_RESIZABLE, false);

    GLFWwindow *window = glfwCreateWindow(SRC_WIDTH, SRC_HEIGHT, SRC_TITLE, nullptr, nullptr);

    if (!window) {
        std::cerr << "Falha ao criar a janela de visualização." << std::endl;

        return -1;
    }

    glfwMakeContextCurrent(window);

    auto framebufferSizeCallback = [](GLFWwindow *window, int width, int height) {
        glViewport(0, 0, width, height);
    };
    
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

    const GLFWvidmode *mode = glfwGetVideoMode(monitor);

    if (!mode) {
        std::cerr << "Falha ao acessar modo de vídeo." << std::endl;

        return -1;
    }

    glfwSetWindowPos(window, (mode->width - SRC_WIDTH) / 2, (mode->height - SRC_HEIGHT) / 2);
    glfwSetCursorPos(window, SRC_WIDTH / 2, SRC_HEIGHT / 2);

    if (glewInit() != GLEW_OK) {
        std::cerr << "Falha ao iniciar GLEW." << std::endl;

        return -1;
    }

    std::string chunkVertexShaderSource = luaToString(L, "chunkVertexShaderSource");
    std::string chunkFragmentShaderSource = luaToString(L, "chunkFragmentShaderSource");
    std::string skyboxVertexShaderSource = luaToString(L, "skyboxVertexShaderSource");
    std::string skyboxFragmentShaderSource = luaToString(L, "skyboxFragmentShaderSource");
    std::string frameBoxVertexShaderSource = luaToString(L, "frameBoxVertexShaderSource");
    std::string frameBoxFragmentShaderSource = luaToString(L, "frameBoxFragmentShaderSource");

    ShaderProgram chunkShaderProgram(chunkVertexShaderSource, chunkFragmentShaderSource);
    ShaderProgram skyboxShaderProgram(skyboxVertexShaderSource, skyboxFragmentShaderSource);
    ShaderProgram frameBoxShaderProgram(frameBoxVertexShaderSource, frameBoxFragmentShaderSource);

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

    Camera camera(SRC_WIDTH, SRC_HEIGHT);

    camera.setPosition({-16.0f, 0.0f, 16.0f});
    camera.setFOV(60);

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

            /*
            if (chunkManager.checkCollision(camera)) {
                glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            }
            */

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

    lua_close(L);

    return 0;
}