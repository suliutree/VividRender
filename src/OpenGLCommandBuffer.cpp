#include "OpenGLCommandBuffer.h"
#include <iostream>
#include <glm/gtc/type_ptr.hpp> 

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

void OpenGLCommandBuffer::bindTexture(GLenum unit, GLuint textureID) {
    _commands.emplace_back([unit, textureID]() {
        glActiveTexture(unit);
        glBindTexture(GL_TEXTURE_2D, textureID);
    });
}

void OpenGLCommandBuffer::setUniform1i(GLint location, GLint value) {
    _commands.emplace_back([location, value]() {
        glUniform1i(location, value);
    });
}

void OpenGLCommandBuffer::setUniformMat4(GLint location, const float* value) {
    glm::mat4 mat = glm::make_mat4(value);
    _commands.emplace_back([location, mat]() {
        glUniformMatrix4fv(location, 1, GL_FALSE, &mat[0][0]);
    });
}

void OpenGLCommandBuffer::setUniform3f(GLint location, float x, float y, float z) {
    _commands.emplace_back([location, x, y, z]() {
        glUniform3f(location, x, y, z);
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