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
find_package(OpenGL REQUIRED)

# --- 新增：FetchContent 拉取 GLAD ---
include(FetchContent)
FetchContent_Declare(
  glad
  GIT_REPOSITORY https://github.com/Dav1dde/glad.git
  GIT_TAG        v0.1.36
)
FetchContent_MakeAvailable(glad)

# 添加头文件搜索路径
include_directories(${CMAKE_SOURCE_DIR}/src)

 # 可执行文件
add_executable(VividRender
  src/main.cpp
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
    OpenGL::GL
    glad
    dl
)
target_include_directories(VividRender
  PRIVATE
    ${glad_SOURCE_DIR}/include
)
