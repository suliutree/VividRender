#define GLFW_INCLUDE_NONE
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

#include "OpenGLDevice.h"

int main() {
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW\n";
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
   #ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
   #endif

    GLFWwindow* window = glfwCreateWindow(800, 600, "VividRender", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window\n";
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD\n";
        return -1;
    }

    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);

    // 使用抽象设备
    OpenGLDevice device(window);
    ICommandBuffer* cmd = device.getCommandBuffer();

    // PipelineState pipeline("shaders/simple.vert", "shaders/simple.frag");

    // 主循环
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        device.beginFrame();
        cmd->clear();
        // —— 这里可以插入更多渲染命令 —— //
        device.endFrame();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
