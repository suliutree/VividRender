#include "OpenGLDevice.h"
#include <iostream>

OpenGLDevice::OpenGLDevice(GLFWwindow* window) : _window(window) {
    // // 主线程释放对 OpenGL Context 的控制权
    // glfwMakeContextCurrent(nullptr);

    // 启动渲染线程，并将 window 的上下文传递给它
    _renderThread = std::thread(&OpenGLDevice::renderThreadMain, this);
}

OpenGLDevice::~OpenGLDevice() {
    // 如果 shutdown 没有被显式调用，确保在这里处理
    if (_isRunning.load()) {
        shutdown();
    }
}

void OpenGLDevice::shutdown() {
    if (!_isRunning.load()) return;
    
    _isRunning.store(false);
    _commandQueue.stop(); // 唤醒并停止渲染线程的等待
    if (_renderThread.joinable()) {
        _renderThread.join();
    }
}

ICommandBuffer* OpenGLDevice::beginFrame() {
    // 只在初始化时为 false，在渲染线程首次插入 fence 后设为 true，之后保持 true，用来区分“第一次”和“之后每次”都要等待 fence 的逻辑
    if (hasSubmitted[_currentFrame]) {
        GLenum waitResult;
        do {
            // 这是主线程（Client）在等待 GPU。它会阻塞主线程的执行，直到 _fences[_currentFrame] 这个 Fence 被 GPU 触发。
            waitResult = glClientWaitSync(_fences[_currentFrame], GL_SYNC_FLUSH_COMMANDS_BIT, 1000000000); // 等待1秒
        } while (waitResult == GL_TIMEOUT_EXPIRED || waitResult == GL_WAIT_FAILED);
        glDeleteSync(_fences[_currentFrame]); // 一旦等待成功，意味着这个 Fence 已经完成了它的历史使命，我们就可以安全地删除它，释放资源。
        _fences[_currentFrame] = nullptr;
    }

    // 对应的旧数据已经被 GPU 完全消费，可以安全地被覆写
    // 拿到本次要用的命令缓冲区
    auto* cmd = &_commandBuffers[_currentFrame];

    cmd->reset();

    // 记录“这个命令缓冲区对应的是哪一路槽位”
    // —— 就是把主线程当前的 _currentFrame 写给它
    static_cast<OpenGLCommandBuffer*>(cmd)->setFrameIndex(_currentFrame);
    
    return cmd;
}

void OpenGLDevice::endFrame(ICommandBuffer* cmd) {
    // 1. 将录制好的命令缓冲区推入队列，供渲染线程消费
    _commandQueue.push(cmd);
    
    // 2. 移动到下一帧的资源
    _currentFrame = (_currentFrame + 1) % NUM_FRAMES_IN_FLIGHT;
}

void OpenGLDevice::renderThreadMain() {
    // 1. 渲染线程获取 OpenGL Context
    glfwMakeContextCurrent(_window);
    
    // 初始化 GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD in render thread" << std::endl;
        return;
    }

    int width, height;
    glfwGetFramebufferSize(_window, &width, &height);
    glViewport(0, 0, width, height);
    
    // 全局 GL State 设置
    glClearColor(0.05f, 0.05f, 0.2f, 1.0f);
    glEnable(GL_DEPTH_TEST);


    // 在渲染线程中，遍历所有注册资源，调用 initializeGL()
    for (auto* res : _resources) {
        res->initializeGL();
    }
    
    while (_isRunning.load()) {
        ICommandBuffer* cmd;
        if (_commandQueue.wait_and_pop(cmd)) {

            // --- 调试打印 1: 检查收到的命令 ---
            if (cmd) {
                // std::cout << "[Render Thread] Popped a command buffer." << std::endl;
            } else {
                std::cerr << "[Render Thread] Popped a NULL command buffer!" << std::endl;
                continue; // 如果是空的就不要继续执行
            }

            // 2. 从队列中获取到一个命令缓冲区，并执行其中的所有命令
            static_cast<OpenGLCommandBuffer*>(cmd)->executeAll();

            // 3. 交换缓冲区
            glfwSwapBuffers(_window);

            int idx = static_cast<OpenGLCommandBuffer*>(cmd)->getFrameIndex();

            // 4. 插入一个 Fence，用于主线程下一轮的同步
            // 这个 Fence 会在所有已提交的 GL 命令执行完毕后被触发
            _fences[idx] = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0); // 当执行完在此之前提交的所有命令后，请将这个 Fence 标记为‘已触发’（Signaled）状态

            hasSubmitted[idx] = true;
        }
    }
    
    // 线程退出前清理
    glfwMakeContextCurrent(nullptr);
}