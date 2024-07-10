#ifndef INPUTS_HPP
#define INPUTS_HPP

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

#endif