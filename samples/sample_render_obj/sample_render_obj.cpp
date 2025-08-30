#define GLFW_INCLUDE_NONE
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

#include "OpenGLDevice.h"
#include "ResourceManager.h"
#include "RenderGraph.h"
#include "ClearPass.h"
#include "ModelPass.h"
#include "Camera.h"
#include "Model.h"
#include <glm/glm.hpp>

const int WindowWidth = 800;
const int WindowHeight = 600;

struct InputState {
    bool firstMouse = true;
    double lastX = WindowWidth * 0.5;  // 窗口中心
    double lastY = WindowHeight * 0.5;
    Camera* camera = nullptr;
};

static void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    InputState* st = (InputState*)glfwGetWindowUserPointer(window);
    if (st->firstMouse) {
        st->lastX = xpos;
        st->lastY = ypos;
        st->firstMouse = false;
    }
    float xoffset = (float)(xpos - st->lastX);
    float yoffset = (float)(st->lastY - ypos); // y 反过来
    st->lastX = xpos;
    st->lastY = ypos;

    st->camera->processMouse(xoffset, yoffset);
}

static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    InputState* st = (InputState*)glfwGetWindowUserPointer(window);
    st->camera->processScroll((float)yoffset);
}

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

    GLFWwindow* renderWindow = glfwCreateWindow(WindowWidth, WindowHeight, "VividRender", nullptr, nullptr);
    if (!renderWindow) {
        std::cerr << "Failed to create render window\n";
        glfwTerminate();
        return -1;
    }

    glfwSetInputMode(renderWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // 隐藏窗口 + 共享 Context 初始化 GLAD
    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
    GLFWwindow* loaderWindow = glfwCreateWindow(1,1,"Loader",nullptr,renderWindow);
    glfwMakeContextCurrent(loaderWindow);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD on loader thread\n";
        return -1;
    }
    std::cout << "GLAD Initialized on Loader Thread.\n";

    // --- 创建 Device & ResourceManager ---
    // glfwMakeContextCurrent(nullptr);              // 主线程先解绑
    OpenGLDevice device(renderWindow);            // 渲染线程开始跑
    ResourceManager resMgr(&device);              // 统一的资源管理

    auto phong      = resMgr.loadPipeline("../../../shaders/phong.vert", "../../../shaders/phong.frag");
    auto model      = resMgr.loadModel("../../../third_party/assimp/test/models/OBJ/spider.obj");
    auto tex        = resMgr.loadTexture("../../../textures/line.png");

    std::cout << "GL Error after resource load: " << glGetError() << std::endl;

    int w = WindowWidth, h = WindowHeight;
    glfwGetFramebufferSize(renderWindow, &w, &h);
    float aspect = (float)w / (float)h;

    glm::vec3 modelCenter = model->getCenter();
    glm::vec3 halfSize = model->getHalfSize();
    Camera camera(modelCenter, 1.f);
    camera.frame(modelCenter, halfSize, aspect); 

    InputState input;
    input.camera = &camera;
    glfwSetWindowUserPointer(renderWindow, &input);
    glfwSetCursorPosCallback(renderWindow, mouse_callback);
    // *** 新增：需要为滚轮设置回调 ***
    glfwSetScrollCallback(renderWindow, scroll_callback); // 需要添加 scroll_callback 函数


    // --- 构建 RenderGraph 中的 Pass ---
    auto clearPass  = std::make_shared<ClearPass>();
    auto modelPass  = std::make_shared<ModelPass>(phong, model, &camera, &aspect);

    double lastTime = glfwGetTime();

    // --- 主循环 ---
    while (!glfwWindowShouldClose(renderWindow)) {
        glfwPollEvents();

         if (glfwGetKey(renderWindow, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            glfwSetWindowShouldClose(renderWindow, GLFW_TRUE);
        }

        double now = glfwGetTime();
        float deltaTime = float(now - lastTime);
        lastTime = now;

        // --- 键盘控制摄像机 (现在是平移) ---
        // *** 主要修改点 2: 键盘映射 ***
        // 注意枚举值的变化
        if (glfwGetKey(renderWindow, GLFW_KEY_W) == GLFW_PRESS) camera.processKeyboard(Camera_Movement::Up,    deltaTime);
        if (glfwGetKey(renderWindow, GLFW_KEY_S) == GLFW_PRESS) camera.processKeyboard(Camera_Movement::Down,  deltaTime);
        if (glfwGetKey(renderWindow, GLFW_KEY_A) == GLFW_PRESS) camera.processKeyboard(Camera_Movement::Left,  deltaTime);
        if (glfwGetKey(renderWindow, GLFW_KEY_D) == GLFW_PRESS) camera.processKeyboard(Camera_Movement::Right, deltaTime);

        // *** 主要修改点 3: 调用 camera.update() ***
        // 在处理完所有输入后，调用 update() 来计算相机最终的位置和朝向
        camera.update();

        // 处理窗口大小变化
        int nw, nh;
        glfwGetFramebufferSize(renderWindow, &nw, &nh);
        if (nw != w || nh != h) {
            w = nw; h = nh;
            aspect = (float)w / (float)h;
        }

        RenderGraph g;
        g.addPass(clearPass, {}, {RenderResource::ClearedRenderTarget});
        g.addPass(modelPass,  {RenderResource::ClearedRenderTarget}, {RenderResource::FinalFrame});
        ICommandBuffer* cmd = device.beginFrame();
        g.execute(&device, cmd);
        device.endFrame(cmd);
    }

    device.shutdown();            // 停掉渲染线程
    glfwDestroyWindow(loaderWindow);
    glfwDestroyWindow(renderWindow);
    glfwTerminate();
    return 0;
}
