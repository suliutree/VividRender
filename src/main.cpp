#define GLFW_INCLUDE_NONE
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

#include "OpenGLDevice.h"
#include "Pipeline.h"
#include "VertexBuffer.h"
#include "OpenGLVertexBuffer.h"
#include "RenderGraph.h"
#include "ClearPass.h"
#include "TrianglePass.h"

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

    // 1. 创建主渲染窗口
    GLFWwindow* renderWindow = glfwCreateWindow(800, 600, "VividRender - Shared Context", nullptr, nullptr);
    if (renderWindow == nullptr) {
        std::cerr << "Failed to create render window" << std::endl;
        glfwTerminate();
        return -1;
    }

    // 2. 创建一个隐藏的窗口，用于承载主线程的共享 Context
    //    关键：最后一个参数告诉 GLFW，这个新 Context 要和 renderWindow 的 Context 共享资源
    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE); // 设置为不可见
    GLFWwindow* loaderWindow = glfwCreateWindow(1, 1, "Loader", nullptr, renderWindow);
    if (loaderWindow == nullptr) {
        std::cerr << "Failed to create loader window" << std::endl;
        glfwTerminate();
        return -1;
    }

    // 3. 在主线程上激活 loaderWindow 的上下文，并用它来初始化 GLAD 和创建资源
    glfwMakeContextCurrent(loaderWindow);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD on main thread" << std::endl;
        return -1;
    }
    std::cout << "GLAD Initialized on Main/Loader Thread." << std::endl;

    // 三角形：pos(x,y) + color(r,g,b)
    float vertices[] = {
         0.0f,  0.5f,   1.0f, 0.0f, 0.0f,
         0.5f, -0.5f,   0.0f, 1.0f, 0.0f,
        -0.5f, -0.5f,   0.0f, 0.0f, 1.0f
    };

    PipelineState pipeline("shaders/simple.vert", "shaders/simple.frag");

    auto* glTriangleVB = new OpenGLVertexBuffer(vertices, sizeof(vertices));
    IVertexBuffer* triangleVB = glTriangleVB;

    std::cout << "main thread: " << glGetError() << std::endl;

    // 4. 创建设备，它会将 renderWindow 的 Context 转移到渲染线程
    //    注意：我们将 renderWindow 传递给它
    OpenGLDevice device(renderWindow);
    device.registerResource(glTriangleVB);
    device.registerResource(&pipeline);

    // // 5. 主线程释放其上下文的控制权，以避免任何意外的 GL 调用
    // //    这是一个好习惯，确保主线程只在需要时（比如动态加载资源）才去绑定上下文
    // glfwMakeContextCurrent(nullptr);

    // --- Render Graph 设置 ---
    // 在实际应用中，每帧可能会重新构建 RenderGraph
    auto clearPass = std::make_shared<ClearPass>();
    auto trianglePass = std::make_shared<TrianglePass>(&pipeline, triangleVB);


    // --- 主循环 (Main Thread) ---
    while (!glfwWindowShouldClose(renderWindow)) {
        glfwPollEvents();
        
        // RenderGraph 每次执行都会清空，所以每帧重新添加
        RenderGraph graph;
        // ClearPass:
        // - 输入: 无
        // - 输出: ClearedRenderTarget
        graph.addPass(clearPass, {}, {RenderResource::ClearedRenderTarget});

        // TrianglePass:
        // - 输入: ClearedRenderTarget
        // - 输出: FinalFrame
        graph.addPass(trianglePass, {RenderResource::ClearedRenderTarget}, {RenderResource::FinalFrame});

        // 1. 开始录制
        ICommandBuffer* cmd = device.beginFrame();

        // 2. 在主线程上录制所有渲染命令
        graph.execute(&device, cmd);

        // 3. 提交命令缓冲区给渲染线程执行
        device.endFrame(cmd);
    }

    device.shutdown(); // 发送信号，等待渲染线程结束
    delete triangleVB;
    glfwDestroyWindow(loaderWindow); // 销毁隐藏窗口
    glfwDestroyWindow(renderWindow); // 销毁主窗口
    glfwTerminate();
    return 0;
}
