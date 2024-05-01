#include <iostream>
#include <vector>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

extern "C" {
#include "./lua54/lua.h"
#include "./lua54/lualib.h"
#include "./lua54/lauxlib.h"
}

#define FNL_IMPL
#include "./FastNoiseLite.h"

#define SRC_WIDTH 1200
#define SRC_HEIGHT 600
#define FPS 60

#include "./camera.hpp"
#include "./shader.hpp"
#include "./image.hpp"
#include "./block.hpp"

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void keyboard_callback(GLFWwindow *window, Camera &camera);
void mouse_callback(GLFWwindow *window, Camera &camera);

int main() {
    lua_State *L = luaL_newstate();

    if (!L) {
        std::cerr << "Falha ao criar estado lua." << std::endl;

        return 1;
    }

    luaL_openlibs(L);

    if (luaL_dofile(L, "./script.lua") != LUA_OK) {
        std::cerr << "Falha ao ler arquivo lua: " << lua_tostring(L, -1) << std::endl;

        return 1;
    }

	if (glfwInit() == GLFW_NOT_INITIALIZED) {
        std::cerr << "Falha ao iniciar GLFW." << std::endl;

        return 1;
    }

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, false);

    GLFWwindow *window = glfwCreateWindow(SRC_WIDTH, SRC_HEIGHT, "Voxel Engine 1.0", nullptr, nullptr);

    if (!window) {
        std::cerr << "Falha ao criar a janela de visualização." << std::endl;

        return 1;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    const GLFWvidmode *mode = glfwGetVideoMode(glfwGetPrimaryMonitor());

    glfwSetWindowPos(window, (mode->width - SRC_WIDTH) / 2, (mode->height - SRC_HEIGHT) / 2);
    glfwSetCursorPos(window, SRC_WIDTH / 2, SRC_HEIGHT / 2);

    if (glewInit() != GLEW_OK) {
        std::cout << "Falha ao iniciar GLEW." << std::endl;

        return 1;
    }

    Image image("./block.png");

    lua_getglobal(L, "vertex_shader_source");

    const char *vertex_shader_source = lua_tostring(L, -1);

    lua_pop(L, 1);

    lua_getglobal(L, "fragment_shader_source");

    const char *fragment_shader_source = lua_tostring(L, -1);

    lua_pop(L, 1);

    Shader shader(vertex_shader_source, fragment_shader_source);

    Block block;

    Camera camera({0.0f, 0.0f, -3.0f});

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);

    double last_frame = 0.0, current_frame = 0.0;

    while (!glfwWindowShouldClose(window)) {
        current_frame = glfwGetTime();

        if ((current_frame - last_frame) > (1.0f / FPS)) {
            glClearColor(0.5f, 0.5f, 1.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            keyboard_callback(window, camera);
            mouse_callback(window, camera);

            block.draw(shader, image, camera);

            glfwSwapBuffers(window);
            glfwPollEvents();

            last_frame = current_frame;
        }
    }

    glfwTerminate();

    lua_close(L);

	return 0;
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    glViewport(0, 0, width, height);
}

void keyboard_callback(GLFWwindow *window, Camera &camera) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }

    float camera_speed = 0.1f;

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        camera.keyboard_update(CAMERA_MOVEMENTS::FORWARD, camera_speed);
    }

    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        camera.keyboard_update(CAMERA_MOVEMENTS::BACKWARD, camera_speed);
    }

    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        camera.keyboard_update(CAMERA_MOVEMENTS::RIGHT, camera_speed);
    }

    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        camera.keyboard_update(CAMERA_MOVEMENTS::LEFT, camera_speed);
    }
}

void mouse_callback(GLFWwindow *window, Camera &camera) {
    double x = 0.0, y = 0.0;

    glfwGetCursorPos(window, &x, &y);

    static bool first_mouse = true;
    static double last_x = 0.0, last_y = 0.0;

    if (first_mouse) {
        last_x = x;
        last_y = y;
        first_mouse = false;
    }

    double off_set_x = x - last_x, off_set_y = last_y -y;

    last_x = x;
    last_y = y;

    double camera_sensitivity = 0.1;

    camera.mouse_update(off_set_x, off_set_y, camera_sensitivity);
}