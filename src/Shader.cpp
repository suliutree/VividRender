#include "Shader.h"
#include <fstream>
#include <sstream>
#include <iostream>

GLuint Shader::compileFromFile(const std::string& path, GLenum type) {
    // 1. 读取文件
    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "Failed to open shader file: " << path << "\n";
        return 0;
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string srcStr = buffer.str();
    const char* src = srcStr.c_str();

    // 2. 创建并编译
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);

    // 3. 检查编译状态
    GLint status;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
    if (status != GL_TRUE) {
        GLint logLen;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLen);
        std::string log(logLen, ' ');
        glGetShaderInfoLog(shader, logLen, nullptr, &log[0]);
        std::cerr << "Shader compile error (" << path << "):\n" << log << "\n";
        glDeleteShader(shader);
        return 0;
    }
    return shader;
}
