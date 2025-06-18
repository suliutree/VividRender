#include "OpenGLCommandBuffer.h"

void OpenGLCommandBuffer::clear() {
    // 同时清除颜色和深度
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}


void OpenGLCommandBuffer::draw(unsigned int vertexCount) {
    // 假设已经绑定了 VAO/VBO，以及已绑定的 ShaderProgram
    glDrawArrays(GL_TRIANGLES, 0, vertexCount);
}