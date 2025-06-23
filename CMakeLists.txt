cmake_minimum_required(VERSION 3.10)
project(VividRender LANGUAGES C CXX)

# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# GLFW
find_package(glfw3 3.3 REQUIRED)

# 强制使用 GLVND
if(POLICY CMP0072)
  cmake_policy(SET CMP0072 NEW)
endif()
set(OpenGL_GL_PREFERENCE GLVND)

# Find OpenGL framework on macOS
if(APPLE)
  find_library(COCOA_LIBRARY Cocoa)
  find_library(OpenGL_LIBRARY OpenGL)
  find_library(IOKIT_LIBRARY IOKit)
  find_library(COREVIDEO_LIBRARY CoreVideo)

  set(OPENGL_LIBRARIES
    ${OpenGL_LIBRARY}
    ${COCOA_LIBRARY}
    ${IOKIT_LIBRARY}
    ${COREVIDEO_LIBRARY}
  )
else()
  find_package(OpenGL REQUIRED)
endif()

add_library(glad STATIC
  third_party/glad/src/glad.c
)
target_include_directories(glad PUBLIC
  third_party/glad/include
)

find_package(Threads REQUIRED)

# 添加头文件搜索路径
include_directories(${CMAKE_SOURCE_DIR}/src)

 # 可执行文件
add_executable(VividRender
  src/main.cpp
  src/CommandQueue.h
  src/RenderPass.h
  src/RenderGraph.h src/RenderGraph.cpp
  src/ClearPass.h   src/ClearPass.cpp
  src/TrianglePass.h src/TrianglePass.cpp
  src/VertexBuffer.h src/OpenGLVertexBuffer.h src/OpenGLVertexBuffer.cpp
  src/Shader.h src/Shader.cpp
  src/Pipeline.h src/Pipeline.cpp
  src/Device.h src/CommandBuffer.h
  src/OpenGLDevice.h src/OpenGLDevice.cpp
  src/OpenGLCommandBuffer.h src/OpenGLCommandBuffer.cpp
)

# 构建完毕后，把项目根的 shaders 目录拷贝到可执行文件所在目录
add_custom_command(TARGET VividRender POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/shaders
    $<TARGET_FILE_DIR:VividRender>/shaders
)

# 链接：GLFW、OpenGL、GLAD
target_link_libraries(VividRender
  PRIVATE
    glfw
    glad
    ${OPENGL_LIBRARIES}  # Linking OpenGL on macOS
    dl
    Threads::Threads
)
