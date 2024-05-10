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
    Camera(glm::vec3 position) : position(position) {

    }

    glm::mat4 get_projection() const { 
        return glm::perspective(glm::radians(FOV), static_cast<double>(SRC_WIDTH) / static_cast<double>(SRC_HEIGHT), near, far);
    }

    glm::mat4 get_view() const {
        return glm::lookAt(position, position + front, up);
    }

    void keyboard_update(CAMERA_MOVEMENTS camera_movements, float speed) {
        switch (camera_movements) {
            case CAMERA_MOVEMENTS::FORWARD:
                position += speed * front;
                break;
            case CAMERA_MOVEMENTS::BACKWARD:
                position -= speed * glm::normalize(glm::cross(front, up));
                break;
            case CAMERA_MOVEMENTS::RIGHT:
                position -= speed * front;
                break;
            case CAMERA_MOVEMENTS::LEFT:
                position += speed * glm::normalize(glm::cross(front, up));
                break;
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
protected:
    glm::vec3 position = glm::vec3(0.0f), front = glm::vec3(0.0f, 0.0f, 1.0f), up = glm::vec3(0.0f, 1.0f, 0.0f);
    double yaw = 90.0, pitch = 0.0, near = 0.001, far = 1000.0, FOV = 60.0;
};

#endif