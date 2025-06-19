#pragma once

#include "Device.h"
#include "OpenGLCommandBuffer.h"
#include "CommandQueue.h"
#include "IGLResource.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <thread>
#include <vector>
#include <atomic>

// 使用双缓冲来允许 CPU 和 GPU 并行工作
constexpr int NUM_FRAMES_IN_FLIGHT = 2;

class OpenGLDevice : public IDevice {
public:
    OpenGLDevice(GLFWwindow* window);
    ~OpenGLDevice() override;

    // 注册一个需要在渲染线程中创建 GL 资源的对象
    void registerResource(IGLResource* res) {
        _resources.push_back(res);
    }

    ICommandBuffer* beginFrame() override;
    void endFrame(ICommandBuffer* cmd) override;
    void shutdown() override;

private:
    void renderThreadMain();

    std::vector<IGLResource*> _resources;

    GLFWwindow* _window = nullptr;
    std::thread _renderThread;
    CommandQueue<ICommandBuffer*> _commandQueue;

    // 命令缓冲区池
    OpenGLCommandBuffer _commandBuffers[NUM_FRAMES_IN_FLIGHT];
    
    // Fence 池，用于 CPU-GPU 同步
    GLsync _fences[NUM_FRAMES_IN_FLIGHT] = { nullptr };
    
    // 当前正在录制的帧索引 (主线程使用)
    int _currentFrame = 0;

    std::atomic<bool> _isRunning{true};
};