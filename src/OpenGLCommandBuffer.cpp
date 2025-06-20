#include "OpenGLCommandBuffer.h"
#include <iostream>

void OpenGLCommandBuffer::clear() {
    // std::cout << "check clear" << std::endl;
    // 录制 clear 命令
    _commands.emplace_back([]() {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    });
}


void OpenGLCommandBuffer::draw(unsigned int vertexCount) {
    // 录制 draw 命令，捕获顶点数量
    _commands.emplace_back([vertexCount]() {
        glDrawArrays(GL_TRIANGLES, 0, vertexCount);
    });
}

void OpenGLCommandBuffer::bindPipeline(GLuint programID) {
    _commands.emplace_back([programID]() {
        glUseProgram(programID);
    });
}

void OpenGLCommandBuffer::unbindPipeline() {
    _commands.emplace_back([]() {
        glUseProgram(0);
    });
}

void OpenGLCommandBuffer::bindVertexArray(GLuint VAO) {
    _commands.emplace_back([VAO]() {
        glBindVertexArray(VAO);
    });
}

void OpenGLCommandBuffer::executeAll() {

    // 遍历并执行所有录制的命令
    for (size_t i = 0; i < _commands.size(); ++i) {
        if (_commands[i]) { // <-- 关键检查！
            _commands[i]();
        } else {
            // 这会把崩溃变成一条错误信息
            std::cerr << "[Render Thread] ERROR: Command at index " << i << " is an empty std::function!" << std::endl;
        }
    }
}

void OpenGLCommandBuffer::reset() {
    _commands.clear();
}