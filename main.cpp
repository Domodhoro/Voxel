#include <iostream>
#include <fstream>
#include <string>
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

#define SRC_WIDTH 800
#define SRC_HEIGHT 600
#define FPS 60
#define CHUNK_SIZE 16

enum class CAMERA_MOVEMENTS {
    FORWARD, BACKWARD, RIGHT, LEFT
};

struct Vertex {
	glm::vec3 coordinates;
	glm::vec2 texture_coordinates;
};

class Camera {
public:
    Camera(glm::vec3 position) : position(position) {

    }

    glm::mat4 get_projection() const { 
        return glm::perspective(glm::radians(FOV), (double)SRC_WIDTH / (double)SRC_HEIGHT, near, far);
    }

    glm::mat4 get_view() const { 
        return glm::lookAt(position, position + front, up);
    }

    void keyboard_update(CAMERA_MOVEMENTS camera_movements, float speed) {
        if (camera_movements == CAMERA_MOVEMENTS::FORWARD) {
            position += speed * front;
        }

        if (camera_movements == CAMERA_MOVEMENTS::BACKWARD) {
            position -= speed * glm::normalize(glm::cross(front, up));
        }

        if (camera_movements == CAMERA_MOVEMENTS::RIGHT) {
            position -= speed * front;
        }

        if (camera_movements == CAMERA_MOVEMENTS::LEFT) {
            position += speed * glm::normalize(glm::cross(front, up));
        }
    }

    void mouse_update(double &off_set_x, double &off_set_y, double sensitivity) {
        yaw += off_set_x * sensitivity;
        pitch += off_set_y * sensitivity;

        const double angle_max = 80.0;

        if (pitch >= angle_max) {
            pitch = angle_max;
        }

        if (pitch <= -angle_max) {
            pitch = -angle_max;
        }

        glm::vec3 direction = {
            cos(glm::radians(yaw)) * cos(glm::radians(pitch)),
            sin(glm::radians(pitch)),
            sin(glm::radians(yaw)) * cos(glm::radians(pitch))
        };

        front = glm::normalize(direction);
    }
private:
    glm::vec3 position = glm::vec3(0.0f), front = glm::vec3(0.0f, 0.0f, 1.0f), up = glm::vec3(0.0f, 1.0f, 0.0f);
    double yaw = -90.0, pitch = 0.0, near = 0.001, far = 1000.0, FOV = 60.0;
};

class Shader {
public:
    Shader(const char *vertex_shader_source, const char *fragment_shader_source) {
        int success = 0;
        char info_log[512];

        unsigned int vertex = glCreateShader(GL_VERTEX_SHADER), fragment = glCreateShader(GL_FRAGMENT_SHADER);

        glShaderSource(vertex, 1, &vertex_shader_source, nullptr);
        glCompileShader(vertex);
        glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);

        if (!success) {
            glGetShaderInfoLog(vertex, 512, nullptr, info_log);

            std::cout << info_log << std::endl;
        }

        glShaderSource(fragment, 1, &fragment_shader_source, nullptr);
        glCompileShader(fragment);
        glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
        
        if (!success) {
            glGetShaderInfoLog(fragment, 512, nullptr, info_log);

            std::cout << info_log << std::endl;
        }

        shader = glCreateProgram();

        glAttachShader(shader, vertex);
        glAttachShader(shader, fragment);
        glLinkProgram(shader);
        glGetProgramiv(shader, GL_LINK_STATUS, &success);

        if (!success) {
            glGetProgramInfoLog(shader, 512, nullptr, info_log);

            std::cout << info_log << std::endl;
        }

        glDeleteShader(vertex);
        glDeleteShader(fragment);
    }

    void set_mat4(const char *name, const glm::mat4 matrix) const { 
        glUniformMatrix4fv(glGetUniformLocation(shader, name), 1, false, glm::value_ptr(matrix));
    }

    void use() const {
        glUseProgram(shader);
    }
private:
    unsigned int shader = 0u;
};

class Image {
public:
    Image(const char* image_path) {
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        SDL_Surface *surface = IMG_Load(image_path);

        if (surface->pixels) {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface->w, surface->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, surface->pixels);
            glGenerateMipmap(GL_TEXTURE_2D);

            SDL_FreeSurface(surface);
        }
        else {
            std::cout << "Falha ao carregar imagem: " << image_path << std::endl;
        }
    }

    void use() const {
        glBindTexture(GL_TEXTURE_2D, texture);
    }
private:
    unsigned int texture = 0u;
};

class Mesh {
public:
    Mesh(std::vector<Vertex> &vertices, std::vector<unsigned int> &indices) {
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices.at(0), GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices.at(0), GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, false, 5 * sizeof(float), (void*)(0));
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 2, GL_FLOAT, false, 5 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);

        count = indices.size();
    }

    void draw(Shader &shader, Image &image, Camera &camera, glm::vec3 position) const {
        image.use();
        shader.use();

        glBindVertexArray(VAO);

        glm::mat4 model = glm::mat4(1.0f);

        model = glm::translate(model, position);

        shader.set_mat4("Projection", camera.get_projection());
        shader.set_mat4("View", camera.get_view());
        shader.set_mat4("Model", model);
        
        glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, (void*)(0));
    }
private:
    unsigned int VAO = 0u, VBO = 0u, EBO = 0u, count = 0u;
};

class Chunk {
public:
    Chunk() {
        fnl_state noise = fnlCreateState();
        noise.noise_type = FNL_NOISE_OPENSIMPLEX2;
        noise.seed = 842;

        for (int i = 0; i != CHUNK_SIZE; i++) {
            for (int j = 0; j != CHUNK_SIZE; j++) {
                for (int k = 0; k != CHUNK_SIZE; k++) {
                    if (j <= 16 * fnlGetNoise2D(&noise, i, k)) {
                        blocks.push_back({{i, j, k}, true});
                    }
                }
            }
        }
    }

    void draw(Shader &shader, Image &image, Camera &camera, Mesh &mesh) const {
        for (auto & block : blocks) {
            if (block.active) {
                glm::vec3 position = block.coordinates;

                mesh.draw(shader, image, camera, position);
            }
        }
    }
private:
    struct Block {
        glm::vec3 coordinates;
        bool active;
    };

    std::vector<Block> blocks;
};

std::string read_file(const std::string &file_path) {
    std::ifstream file(file_path);
    std::string data;

    if (file.is_open()) {
        std::string line;

        while (std::getline(file, line)) {
            data += line + "\n";
        }

        file.close();
    } else {
        std::cout << "Falha ao abrir o arquivo: " << file_path << std::endl;
    }

    return data;
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

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

int main() {
    lua_State *L = luaL_newstate();

    luaL_openlibs(L);

    if (luaL_dofile(L, "./script.lua") != LUA_OK) {        
        std::cout << "Falha ao ler arquivo lua: " << lua_tostring(L, -1) << std::endl;

        return 1;
    }

	if (glfwInit() == GLFW_NOT_INITIALIZED) {
        std::cout << "Falha ao iniciar GLFW." << std::endl;

        return 1;
    }

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, false);

    GLFWwindow *window = glfwCreateWindow(SRC_WIDTH, SRC_HEIGHT, "Voxel Engine 1.0", nullptr, nullptr);

    if (!window) {
        std::cout << "Falha ao criar a janela de visualização." << std::endl;

        return 1;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    const GLFWvidmode *mode = glfwGetVideoMode(glfwGetPrimaryMonitor());

    glfwSetWindowPos(window, (mode->width - SRC_WIDTH) / 2, (mode->height - SRC_HEIGHT) / 2);
    glfwSetCursorPos(window, SRC_WIDTH / 2, SRC_HEIGHT / 2);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    if (glewInit() != GLEW_OK) {
        std::cout << "Falha ao iniciar GLEW." << std::endl;

        return 1;
    }

    std::vector<Vertex> vertices = {
        {{-0.5f, 0.5f,-0.5f}, {0.0f, 1.0f}},
        {{-0.5f,-0.5f,-0.5f}, {0.0f, 0.0f}},
        {{ 0.5f,-0.5f,-0.5f}, {1.0f, 0.0f}},
        {{ 0.5f, 0.5f,-0.5f}, {1.0f, 1.0f}},
        {{-0.5f, 0.5f, 0.5f}, {0.0f, 1.0f}},
        {{-0.5f,-0.5f, 0.5f}, {0.0f, 0.0f}},
        {{ 0.5f,-0.5f, 0.5f}, {1.0f, 0.0f}},
        {{ 0.5f, 0.5f, 0.5f}, {1.0f, 1.0f}},
        {{ 0.5f, 0.5f,-0.5f}, {0.0f, 1.0f}},
        {{ 0.5f,-0.5f,-0.5f}, {0.0f, 0.0f}},
        {{ 0.5f,-0.5f, 0.5f}, {1.0f, 0.0f}},
        {{ 0.5f, 0.5f, 0.5f}, {1.0f, 1.0f}},
        {{-0.5f, 0.5f,-0.5f}, {0.0f, 1.0f}},
        {{-0.5f,-0.5f,-0.5f}, {0.0f, 0.0f}},
        {{-0.5f,-0.5f, 0.5f}, {1.0f, 0.0f}},
        {{-0.5f, 0.5f, 0.5f}, {1.0f, 1.0f}},
        {{-0.5f, 0.5f, 0.5f}, {0.0f, 1.0f}},
        {{-0.5f, 0.5f,-0.5f}, {0.0f, 0.0f}},
        {{ 0.5f, 0.5f,-0.5f}, {1.0f, 0.0f}},
        {{ 0.5f, 0.5f, 0.5f}, {1.0f, 1.0f}},
        {{-0.5f,-0.5f, 0.5f}, {0.0f, 1.0f}},
        {{-0.5f,-0.5f,-0.5f}, {0.0f, 0.0f}},
        {{ 0.5f,-0.5f,-0.5f}, {1.0f, 0.0f}},
        {{ 0.5f,-0.5f, 0.5f}, {1.0f, 1.0f}}
    };

    std::vector<unsigned int> indices = {
         0u,  1u,  3u,  3u,  1u,  2u,
         5u,  4u,  7u,  5u,  7u,  6u,
         8u,  9u, 11u, 11u,  9u, 10u,
        13u, 12u, 15u, 13u, 15u, 14u,
        16u, 17u, 19u, 19u, 17u, 18u,
        21u, 20u, 23u, 21u, 23u, 22u
    };

    std::string vertex_shader_source = read_file("./vertex.glsl");
    std::string fragment_shader_source = read_file("./fragment.glsl");

    Mesh mesh(vertices, indices);
    Image image("./block.png");
    Shader shader(vertex_shader_source.c_str(), fragment_shader_source.c_str());
    Camera camera({0.0f, 0.0f, 3.0f});
    Chunk chunk;

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

            chunk.draw(shader, image, camera, mesh);

            glfwSwapBuffers(window);
            glfwPollEvents();

            last_frame = current_frame;
        }
    }

    glfwTerminate();

    lua_close(L);

	return 0;
}