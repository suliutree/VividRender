cmake_minimum_required(VERSION 3.10)
project(VividRender LANGUAGES C CXX)

# —— 基础设置 —— 
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# GLFW
find_package(glfw3 3.3 REQUIRED)

# 强制使用 GLVND
if(POLICY CMP0072)
  cmake_policy(SET CMP0072 NEW)
endif()
set(OpenGL_GL_PREFERENCE GLVND)

# macOS 下的 OpenGL 框架
if(APPLE)
  find_library(COCOA_LIBRARY       Cocoa)
  find_library(OpenGL_LIBRARY      OpenGL)
  find_library(IOKIT_LIBRARY       IOKit)
  find_library(COREVIDEO_LIBRARY   CoreVideo)
  set(OPENGL_LIBRARIES
    ${OpenGL_LIBRARY}
    ${COCOA_LIBRARY}
    ${IOKIT_LIBRARY}
    ${COREVIDEO_LIBRARY}
  )
else()
  find_package(OpenGL REQUIRED)
  set(OPENGL_LIBRARIES ${OPENGL_LIBRARIES} ${OPENGL_gl_LIBRARY})
endif()

find_package(Threads REQUIRED)

# —— glad 静态库 —— 
add_library(glad STATIC
  third_party/glad/src/glad.c
)
target_include_directories(glad
  PUBLIC
    ${CMAKE_SOURCE_DIR}/third_party/glad/include
)

# —— 主可执行文件 —— 
add_executable(VividRender
  src/main.cpp

  # 渲染框架各模块
  src/RenderGraph.cpp
  src/ClearPass.cpp
  src/TrianglePass.cpp
  src/TexturedTrianglePass.cpp
  src/OpenGLVertexBuffer.cpp
  src/Shader.cpp
  src/Texture2D.cpp
  src/Pipeline.cpp
  src/OpenGLCommandBuffer.cpp
  src/OpenGLDevice.cpp
  src/ResourceManager.cpp
)

# —— 头文件搜索 —— 
# 只为 VividRender 添加两个私有 include 路径：
# 1. 项目源码头文件
# 2. stb_image.h 所在目录
target_include_directories(VividRender
  PRIVATE
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/third_party/stb
    ${CMAKE_SOURCE_DIR}/third_party/
    ${CMAKE_SOURCE_DIR}/third_party/glm
)

# —— 资源拷贝 —— 
# 编译后把 shaders 目录一起拷到可执行文件旁
add_custom_command(TARGET VividRender POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/shaders
    $<TARGET_FILE_DIR:VividRender>/shaders
)

# —— 链接依赖 —— 
target_link_libraries(VividRender
  PRIVATE
    glfw
    glad
    ${OPENGL_LIBRARIES}
    dl
    Threads::Threads
)
