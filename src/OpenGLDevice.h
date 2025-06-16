#pragma once
#include "Device.h"
#include "OpenGLCommandBuffer.h"
#include <GLFW/glfw3.h>

class OpenGLDevice : public IDevice {
public:
    OpenGLDevice(GLFWwindow* window)
        : _window(window)
    {
        // 上下文已经在 main 中创建并初始化 glad
        // 可以在这里做全局 GLState 设置，比如启用深度测试、面剔除等
        glEnable(GL_DEPTH_TEST);
    }

    void beginFrame() override {
        // 每帧开始时，可以在这里设置清屏颜色，这里示例使用固定值
        glClearColor(0.05f, 0.05f, 0.2f, 1.0f);
    }

    ICommandBuffer* getCommandBuffer() override {
        return &_cmdBuffer;
    }

    void endFrame() override {
        // 交换缓冲、处理系统消息等
        glfwSwapBuffers(_window);
    }

private:
    GLFWwindow* _window = nullptr;
    OpenGLCommandBuffer _cmdBuffer;
};
