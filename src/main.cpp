#define GLFW_INCLUDE_NONE
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

#include "OpenGLDevice.h"
#include "ResourceManager.h"
#include "RenderGraph.h"
#include "ClearPass.h"
#include "TrianglePass.h"
#include "TexturedTrianglePass.h"

int main() {
    // --- GLFW 初始化略 ---
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

    GLFWwindow* renderWindow = glfwCreateWindow(800, 600, "VividRender", nullptr, nullptr);
    if (!renderWindow) {
        std::cerr << "Failed to create render window\n";
        glfwTerminate();
        return -1;
    }

    // 隐藏窗口 + 共享 Context 初始化 GLAD
    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
    GLFWwindow* loaderWindow = glfwCreateWindow(1,1,"Loader",nullptr,renderWindow);
    glfwMakeContextCurrent(loaderWindow);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD on loader thread\n";
        return -1;
    }
    std::cout << "GLAD Initialized on Loader Thread.\n";

    // 顶点数据：pos + color + tex 
    float vertices[] = {
         0.0f,  0.5f,  1.0f,0.0f,0.0f,  0.5f,1.0f,
         0.5f, -0.5f,  0.0f,1.0f,0.0f,  1.0f,0.0f,
        -0.5f, -0.5f,  0.0f,0.0f,1.0f,  0.0f,0.0f
    };

    // --- 创建 Device & ResourceManager ---
    // glfwMakeContextCurrent(nullptr);              // 主线程先解绑
    OpenGLDevice device(renderWindow);            // 渲染线程开始跑
    ResourceManager resMgr(&device);              // 统一的资源管理

    // --- 通过 ResourceManager 加载并缓存管线／顶点缓冲 ---
    auto pipeline    = resMgr.loadPipeline("../shaders/simple.vert", "../shaders/simple.frag");
    auto triangleVB  = resMgr.loadVertexBuffer(vertices, sizeof(vertices));
    auto texture     = resMgr.loadTexture("../textures/line.png");

    std::cout << "GL Error after resource load: " << glGetError() << std::endl;

    // --- 构建 RenderGraph 中的 Pass ---
    auto clearPass    = std::make_shared<ClearPass>();
    // auto trianglePass = std::make_shared<TrianglePass>(pipeline.get(), triangleVB.get());
    auto texturedTrianglePass = std::make_shared<TexturedTrianglePass>(pipeline, triangleVB, texture);

    // --- 主循环 ---
    while (!glfwWindowShouldClose(renderWindow)) {
        glfwPollEvents();

        RenderGraph graph;
        graph.addPass(clearPass, {}, { RenderResource::ClearedRenderTarget });
        graph.addPass(texturedTrianglePass, { RenderResource::ClearedRenderTarget }, { RenderResource::FinalFrame });

        // 录制 & 提交
        ICommandBuffer* cmd = device.beginFrame();
        graph.execute(&device, cmd);
        device.endFrame(cmd);
    }

    device.shutdown();            // 停掉渲染线程
    glfwDestroyWindow(loaderWindow);
    glfwDestroyWindow(renderWindow);
    glfwTerminate();
    return 0;
}
