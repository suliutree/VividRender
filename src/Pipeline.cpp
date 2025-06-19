#include "Pipeline.h"
#include "Shader.h"
#include <iostream>

PipelineState::PipelineState(const std::string& vertPath, const std::string& fragPath)
    : _vsPath(vertPath), _fsPath(fragPath)
{

}

PipelineState::~PipelineState() {
    if (programID) {
        glDeleteProgram(programID);
    }
}

void PipelineState::initializeGL() {
    // 1. 编译顶点/片段着色器
    GLuint vert = Shader::compileFromFile(_vsPath, GL_VERTEX_SHADER);
    GLuint frag = Shader::compileFromFile(_fsPath, GL_FRAGMENT_SHADER);
    if (vert == 0 || frag == 0) {
        std::cerr << "Failed to compile shaders\n";
        return;
    }

    // 2. 创建 Program
    programID = glCreateProgram();
    glAttachShader(programID, vert);
    glAttachShader(programID, frag);

    // 3. 链接与校验
    if (!linkAndValidate(vert, frag)) {
        glDeleteProgram(programID);
        programID = 0;
    }

    // 4. 着色器对象不再需要
    glDetachShader(programID, vert);
    glDetachShader(programID, frag);
    glDeleteShader(vert);
    glDeleteShader(frag);
}

bool PipelineState::linkAndValidate(GLuint vertShader, GLuint fragShader) {
    glLinkProgram(programID);
    GLint linkStatus;
    glGetProgramiv(programID, GL_LINK_STATUS, &linkStatus);
    if (linkStatus != GL_TRUE) {
        GLint logLen;
        glGetProgramiv(programID, GL_INFO_LOG_LENGTH, &logLen);
        std::string log(logLen, ' ');
        glGetProgramInfoLog(programID, logLen, nullptr, &log[0]);
        std::cerr << "Program link error:\n" << log << "\n";
        return false;
    }

    // 可选：验证
    glValidateProgram(programID);
    GLint valStatus;
    glGetProgramiv(programID, GL_VALIDATE_STATUS, &valStatus);
    if (valStatus != GL_TRUE) {
        GLint logLen;
        glGetProgramiv(programID, GL_INFO_LOG_LENGTH, &logLen);
        std::string log(logLen, ' ');
        glGetProgramInfoLog(programID, logLen, nullptr, &log[0]);
        std::cerr << "Program validation error:\n" << log << "\n";
        return false;
    }
    return true;
}

// void PipelineState::bind() const {
//     glUseProgram(programID);
// }

// void PipelineState::unbind() const {
//     glUseProgram(0);
// }
