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
add_library(vivid_core
  # 渲染框架各模块
  src/RenderGraph.cpp
  src/ClearPass.cpp
  src/TrianglePass.cpp
  src/TexturedTrianglePass.cpp
  src/ModelPass.cpp
  src/OpenGLVertexBuffer.cpp
  src/OpenGLIndexBuffer.cpp
  src/Shader.cpp
  src/Texture2D.cpp
  src/Model.cpp
  src/Pipeline.cpp
  src/OpenGLCommandBuffer.cpp
  src/OpenGLDevice.cpp
  src/ResourceManager.cpp
)

target_include_directories(vivid_core
  PUBLIC
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/third_party/
    ${CMAKE_SOURCE_DIR}/third_party/stb
    ${CMAKE_SOURCE_DIR}/third_party/glm
)

# —— 直接编译第三方 Assimp ——
# 所有选项设为 CACHE BOOL "" FORCE，避免父 CMake 多次 include 时重复定义
set(ASSIMP_BUILD_TESTS            OFF CACHE BOOL "" FORCE)  # 禁用测试
set(ASSIMP_BUILD_ASSIMP_TOOLS     OFF CACHE BOOL "" FORCE)  # 不编译 assimp 命令行
set(ASSIMP_BUILD_SAMPLES          OFF CACHE BOOL "" FORCE)
set(ASSIMP_INSTALL                OFF CACHE BOOL "" FORCE)  # 不生成 install 规则
set(ASSIMP_NO_EXPORT              ON  CACHE BOOL "" FORCE)  # 不再生成 export 目标
set(ASSIMP_BUILD_ALL_IMPORTERS_BY_DEFAULT OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_OBJ_IMPORTER     ON  CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_GLTF_IMPORTER    OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_FBX_IMPORTER     ON  CACHE BOOL "" FORCE)

# 让静态库默认带 -fPIC，才能与共享 OpenGL lib 等共存（macOS / Linux 必要）
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

add_subdirectory(third_party/assimp)   # 生成 static lib: assimp


# —— 链接依赖 —— 
target_link_libraries(vivid_core
  PUBLIC
    glfw
    glad
    ${OPENGL_LIBRARIES}
    dl
    Threads::Threads
    assimp
)


function(copy_runtime_assets TARGET_NAME)
    # 编译后把 shaders 目录一起拷到可执行文件旁
    add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_SOURCE_DIR}/shaders
            $<TARGET_FILE_DIR:${TARGET_NAME}>/shaders
        COMMAND ${CMAKE_COMMAND} -E copy_directory
                ${CMAKE_SOURCE_DIR}/textures
                $<TARGET_FILE_DIR:${TARGET_NAME}>/textures
        COMMAND ${CMAKE_COMMAND} -E copy_directory
                ${CMAKE_SOURCE_DIR}/assets
                $<TARGET_FILE_DIR:${TARGET_NAME}>/assets
    )
endfunction()

# 假设每个 sample 子目录都有自己的 CMakeLists
add_subdirectory(samples)
