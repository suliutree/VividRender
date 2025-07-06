#pragma once

#include "Device.h"
#include "OpenGLCommandBuffer.h"
#include "CommandQueue.h"
#include "IRenderResource.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <thread>
#include <vector>
#include <atomic>
#include <iostream>

// 使用双缓冲来允许 CPU 和 GPU 并行工作
constexpr int NUM_FRAMES_IN_FLIGHT = 2;

class OpenGLDevice : public IDevice {
public:
    OpenGLDevice(GLFWwindow* window);
    ~OpenGLDevice() override;

    // 注册一个需要在渲染线程中创建 GL 资源的对象
    void registerResource(IRenderResource* res) override;

    void processPendingResources();

    ICommandBuffer* beginFrame() override;
    void endFrame(ICommandBuffer* cmd) override;
    void shutdown() override;

private:
    void renderThreadMain();

    std::mutex _resourceMutex;
    // 已经初始化过、可直接用于渲染的资源列表
    std::vector<IRenderResource*> _resources;
    // 刚注册进来的、还没初始化的资源列表
    std::vector<IRenderResource*> _pendingResources;

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

    bool hasSubmitted[NUM_FRAMES_IN_FLIGHT] = { false };
};