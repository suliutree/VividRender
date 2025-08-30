#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// 键盘输入现在用于平移相机
enum class Camera_Movement {
    Up,
    Down,
    Left,
    Right
};

class Camera {
public:
    // --- 构造函数变更 ---
    // 现在需要一个焦点和初始距离，而不是一个固定的位置
    Camera(glm::vec3 focalPoint = {0.0f, 0.0f, 0.0f},
           float distance = 1.0f,
           float yaw = -90.0f, float pitch = 20.0f)
        : FocalPoint(focalPoint), Distance(distance),
          WorldUp(0.0f, 1.0f, 0.0f),
          Yaw(yaw), Pitch(pitch)
    {
        // 初始更新一次，计算出相机的位置和朝向
        update();
    }

    // --- 核心接口 ---
    // 获取视图矩阵，现在是 Position -> FocalPoint
    glm::mat4 getView() const {
        return glm::lookAt(Position, FocalPoint, Up);
    }

    // 投影矩阵保持不变
    glm::mat4 getProj(float aspect) const {
        return glm::perspective(glm::radians(Fov), aspect, Near, Far);
    }

    // 每一帧都需要调用，根据输入更新相机状态
    void update() {
        // 1. 根据 Yaw 和 Pitch 计算方向向量
        glm::vec3 front;
        front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        front.y = sin(glm::radians(Pitch));
        front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        Front = glm::normalize(front);

        Right = glm::normalize(glm::cross(Front, WorldUp));
        Up    = glm::normalize(glm::cross(Right, Front));

        // 2. 根据方向和距离，计算相机最终的位置
        // 从焦点沿着视线反方向移动 Distance 距离
        Position = FocalPoint - Front * Distance;
    }

    void frame(const glm::vec3& center, const glm::vec3& halfSize, float aspect) {
        FocalPoint = center;

        // 用 AABB 的半宽/半高来计算需要的距离
        float halfHeight = halfSize.y;
        float halfWidth  = halfSize.x;

        float vfov = glm::radians(Fov);
        float hfov = 2.0f * std::atan(std::tan(vfov * 0.5f) * aspect);

        float distV = halfHeight / std::tan(vfov * 0.5f);
        float distH = halfWidth  / std::tan(hfov * 0.5f);

        Distance = std::max(distV, distH) * 1.05f; // 稍微留点边（5% padding）

        // 保障近平面/远平面合理
        float radius = glm::length(halfSize);
        Near = std::max(0.01f, Distance - radius * 2.0f);
        Far  = Distance + radius * 2.0f;

        update();
    }


    // --- 输入处理函数变更 ---

    // 处理键盘输入，实现平移 (Pan)
    void processKeyboard(Camera_Movement dir, float dt) {
        float velocity = PanSpeed * dt;
        // 注意：我们移动的是焦点，相机位置会在 update() 中随之改变
        if (dir == Camera_Movement::Left)  FocalPoint -= Right * velocity;
        if (dir == Camera_Movement::Right) FocalPoint += Right * velocity;
        if (dir == Camera_Movement::Up)    FocalPoint += Up * velocity;
        if (dir == Camera_Movement::Down)  FocalPoint -= Up * velocity;
    }

    // 处理鼠标移动，实现轨道旋转 (Orbit)
    void processMouse(float xoffset, float yoffset, bool constrainPitch = true) {
        xoffset *= MouseSensitivity;
        yoffset *= MouseSensitivity;

        Yaw   += xoffset;
        Pitch += yoffset;

        if (constrainPitch) {
            if (Pitch > 89.0f) Pitch = 89.0f;
            if (Pitch < -89.0f) Pitch = -89.0f;
        }
        // 注意：这里不再调用 updateVectors，而是在主循环中统一调用 update()
    }

    // 处理鼠标滚轮，实现缩放 (Zoom)
    void processScroll(float yoffset) {
        Distance -= yoffset * ZoomSensitivity;
        if (Distance < 1.0f) Distance = 1.0f;
        if (Distance > 100.0f) Distance = 100.0f;
    }

    // --- 公开的属性 ---
    float PanSpeed        = 5.0f;
    float MouseSensitivity= 0.1f;
    float ZoomSensitivity = 0.2f;
    float Fov             = 60.0f;
    float Near            = 0.1f;
    float Far             = 1000.0f;

    // Debugging
    glm::vec3 Position;
    glm::vec3 FocalPoint;

private:
    // 私有状态
    float Distance;
    float Yaw;
    float Pitch;

    // 向量
    glm::vec3 Front;
    glm::vec3 Up;
    glm::vec3 Right;
    glm::vec3 WorldUp;
};