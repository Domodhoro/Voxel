#ifndef CAMERA_HPP
#define CAMERA_HPP

enum class CAMERA_MOVEMENTS {
    FORWARD, 
    BACKWARD, 
    RIGHT, 
    LEFT
};

class Camera {
public:
    Camera() : position(0.0f) {
        
    }

    Camera(glm::vec3 position) : position(position) {

    }

    void toUp(float speed) {
        position.y += speed;
    }

    void toDown(float speed) {
        position.y -= speed;
    }

    void setPosition(glm::vec3 position) {
        this->position = position;
    }

    glm::vec3 getPosition() const {
        return position;
    }

    glm::mat4 getProjection() const { 
        return glm::perspective(glm::radians(FOV), static_cast<double>(SRC_WIDTH) / static_cast<double>(SRC_HEIGHT), near, far);
    }

    glm::mat4 getView() const {
        return glm::lookAt(position, position + front, up);
    }

    void keyboardUpdate(CAMERA_MOVEMENTS cameraMovements, float cameraSpeed) {
        switch (cameraMovements) {
            case CAMERA_MOVEMENTS::FORWARD:
                position += cameraSpeed * front;
                break;
            case CAMERA_MOVEMENTS::BACKWARD:
                position -= cameraSpeed * glm::normalize(glm::cross(front, up));
                break;
            case CAMERA_MOVEMENTS::RIGHT:
                position -= cameraSpeed * front;
                break;
            case CAMERA_MOVEMENTS::LEFT:
                position += cameraSpeed * glm::normalize(glm::cross(front, up));
                break;
        }
    }

    void mouseUpdate(double &xOffset, double &yOffset, double sensitivity) {
        yaw += xOffset * sensitivity;
        pitch += yOffset * sensitivity;

        const double angleMax = 80.0;

        if (pitch >= angleMax) {
            pitch = angleMax;
        }

        if (pitch <= -angleMax) {
            pitch = -angleMax;
        }

        direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        direction.y = sin(glm::radians(pitch));
        direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));

        front = glm::normalize(direction);
    }
protected:
    glm::vec3 position = glm::vec3(0.0f), front = glm::vec3(0.0f, 0.0f, -1.0f), up = glm::vec3(0.0f, 1.0f, 0.0f), direction = glm::vec3(0.0f);

    double yaw = -90.0, pitch = 0.0, near = 0.001, far = 1000.0, FOV = 60.0;
};

#endif