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

    GLint uniformCount = 0;
    glGetProgramiv(programID, GL_ACTIVE_UNIFORMS, &uniformCount);

    char nameBuf[256];
    for (GLint i = 0; i < uniformCount; ++i) {
        GLsizei length = 0;
        GLint size = 0;
        GLenum type = 0;
        glGetActiveUniform(programID, i, sizeof(nameBuf), &length, &size, &type, nameBuf);
        std::string name(nameBuf, length);
        GLint loc = glGetUniformLocation(programID, name.c_str());
        _uniformLocation[name] = loc;
    }

    // 4. 着色器对象不再需要
    glDetachShader(programID, vert);
    glDetachShader(programID, frag);
    glDeleteShader(vert);
    glDeleteShader(frag);
}

GLint PipelineState::getUniformLocation(const std::string& name)
{
    auto it = _uniformLocation.find(name);
    if (it != _uniformLocation.end()) {
        return it->second;
    }

    // 这里使用 gl 函数是否合理
    GLint loc = glGetUniformLocation(programID, name.c_str());
    _uniformLocation[name] = loc;
    return loc;
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

    // // 可选：验证，这里验证必须有一个 VAO 绑定，否则状态不完整，目前先注释掉
    // // 在 OpenGL Core Profile（3.3 及以上核心模式）中，任何与顶点属性相关的绘制操作都必须在绑定一个 VAO（Vertex Array Object）的前提下进行，
    // // 即使你不打算使用属性或只绘制全屏四边形，也必须有一个 VAO 绑定，否则状态是不完整的。
    // glValidateProgram(programID);
    // GLint valStatus;
    // glGetProgramiv(programID, GL_VALIDATE_STATUS, &valStatus);
    // if (valStatus != GL_TRUE) {
    //     GLint logLen;
    //     glGetProgramiv(programID, GL_INFO_LOG_LENGTH, &logLen);
    //     std::string log(logLen, ' ');
    //     glGetProgramInfoLog(programID, logLen, nullptr, &log[0]);
    //     std::cerr << "Program validation error: " << log << "\n";
    //     return false;
    // }
    return true;
}
