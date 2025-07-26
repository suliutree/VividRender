#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

enum class Camera_Movement {
    Forward,
    Backward,
    Left,
    Right
};

class Camera {
public:
    Camera(glm::vec3 position = {0.0f, 0.0f, 3.0f},
           float yaw = -90.0f, float pitch = 0.0f,
           float fov = 60.0f,
           float nearClip = 0.1f, float farClip = 100.0f)
        : Position(position), WorldUp(0.0f, 1.0f, 0.0f),
          Yaw(yaw), Pitch(pitch),
          Fov(fov), Near(nearClip), Far(farClip) {
        updateVectors();
    }

    glm::mat4 getView() const {
        return glm::lookAt(Position, Position + Front, Up);
    }

    glm::mat4 getProj(float aspect) const {
        return glm::perspective(glm::radians(Fov), aspect, Near, Far);
    }

    void processKeyboard(Camera_Movement dir, float dt) {
        float velocity = MoveSpeed * dt;
        if (dir == Camera_Movement::Forward)  Position += Front * velocity;
        if (dir == Camera_Movement::Backward) Position -= Front * velocity;
        if (dir == Camera_Movement::Left)     Position -= Right * velocity;
        if (dir == Camera_Movement::Right)    Position += Right * velocity;
    }

    void processMouse(float xoffset, float yoffset, bool constrainPitch = true) {
        xoffset *= MouseSensitivity;
        yoffset *= MouseSensitivity;

        Yaw   += xoffset;
        Pitch += yoffset;

        if (constrainPitch) {
            if (Pitch > 89.0f) Pitch = 89.0f;
            if (Pitch < -89.0f) Pitch = -89.0f;
        }
        updateVectors();
    }

    void processScroll(float yoffset) {   // 可挂到鼠标滚轮
        Fov -= yoffset;
        if (Fov < 1.0f)  Fov = 1.0f;
        if (Fov > 90.0f) Fov = 90.0f;
    }

    float getFov() const { return Fov; }

    // 你也可以把这些暴露出去做 debug
    glm::vec3 Position;
    float MoveSpeed       = 5.0f;
    float MouseSensitivity= 0.1f;

private:
    void updateVectors() {
        glm::vec3 front;
        front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        front.y = sin(glm::radians(Pitch));
        front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        Front = glm::normalize(front);

        Right = glm::normalize(glm::cross(Front, WorldUp));
        Up    = glm::normalize(glm::cross(Right, Front));
    }

private:
    glm::vec3 Front  = {0.0f, 0.0f, -1.0f};
    glm::vec3 Up     = {0.0f, 1.0f,  0.0f};
    glm::vec3 Right  = {1.0f, 0.0f,  0.0f};
    glm::vec3 WorldUp;

    float Yaw;
    float Pitch;
    float Fov;
    float Near, Far;
};
