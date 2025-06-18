#define GLFW_INCLUDE_NONE
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

#include "OpenGLDevice.h"
#include "Pipeline.h"
#include "VertexBuffer.h"
#include "OpenGLVertexBuffer.h"

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

    // 三角形：pos(x,y) + color(r,g,b)
    float vertices[] = {
        0.0f,  0.5f,   1.0f, 0.0f, 0.0f,  // 顶点 1：红
        0.5f, -0.5f,   0.0f, 1.0f, 0.0f,  // 顶点 2：绿
        -0.5f, -0.5f,   0.0f, 0.0f, 1.0f   // 顶点 3：蓝
    };
    // 1. 创建管线
    PipelineState pipeline("shaders/simple.vert", "shaders/simple.frag");
    // 2. 创建顶点缓冲
    IVertexBuffer* triangleVB = new OpenGLVertexBuffer(vertices, sizeof(vertices));


    // 主循环
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        device.beginFrame();
        cmd->clear();
        
        pipeline.bind();
        triangleVB->bind();
        cmd->draw(3);    // 绘制 3 个顶点
        pipeline.unbind();

        device.endFrame();
    }

    delete triangleVB;

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
