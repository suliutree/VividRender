#include "OpenGLCommandBuffer.h"
#include <iostream>

void OpenGLCommandBuffer::clear() {
    // std::cout << "check clear" << std::endl;
    // 录制 clear 命令
    _commands.emplace_back([]() {
        std::cout << "render therad aaa: " << glGetError() << std::endl;
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        std::cout << "render therad bbb: " << glGetError() << std::endl;
    });
}


void OpenGLCommandBuffer::draw(unsigned int vertexCount) {
    // 录制 draw 命令，捕获顶点数量
    _commands.emplace_back([vertexCount]() {
        std::cout << "render therad ccc: " << glGetError() << std::endl;
        glDrawArrays(GL_TRIANGLES, 0, vertexCount);
        std::cout << "render therad ddd: " << glGetError() << std::endl;
    });
}

void OpenGLCommandBuffer::bindPipeline(GLuint programID) {
    _commands.emplace_back([programID]() {
        std::cout << "render therad 001: " << glGetError() << std::endl;
        glUseProgram(programID);
        std::cout << "render therad 002: " << glGetError() << std::endl;

    });
}

void OpenGLCommandBuffer::unbindPipeline() {
    _commands.emplace_back([]() {
        std::cout << "render therad 003: " << glGetError() << std::endl;

        glUseProgram(0);
        std::cout << "render therad 004: " << glGetError() << std::endl;

    });
}

void OpenGLCommandBuffer::bindVertexArray(GLuint VAO) {
    _commands.emplace_back([VAO]() {
        std::cout << "render therad 005: " << glGetError() << std::endl;
        std::cout << "VAO: " << VAO << std::endl;

        glBindVertexArray(VAO);
        std::cout << "render therad 006: " << glGetError() << std::endl;

    });
}

void OpenGLCommandBuffer::executeAll() {
    // --- 调试打印 3: 检查命令数量 ---
    std::cout << "[Render Thread] Executing " << _commands.size() << " commands." << std::endl;

    std::cout << "render therad 002a: " << glGetError() << std::endl;

    // 遍历并执行所有录制的命令
    for (size_t i = 0; i < _commands.size(); ++i) {
        if (_commands[i]) { // <-- 关键检查！
            _commands[i]();
        } else {
            // 这会把崩溃变成一条错误信息
            std::cerr << "[Render Thread] ERROR: Command at index " << i << " is an empty std::function!" << std::endl;
        }
    }

    std::cout << "render therad 002b: " << glGetError() << std::endl;
}

void OpenGLCommandBuffer::reset() {
    _commands.clear();
}