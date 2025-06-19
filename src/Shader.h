#pragma once

#include <string>
#include <glad/glad.h>

/// 负责从文件加载并编译单个着色器
class Shader {
public:
    /// 从给定路径读取源代码并编译，type 为 GL_VERTEX_SHADER 或 GL_FRAGMENT_SHADER 等
    static GLuint compileFromFile(const std::string& path, GLenum type);
};
