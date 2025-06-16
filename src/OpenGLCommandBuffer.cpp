#include "OpenGLCommandBuffer.h"

void OpenGLCommandBuffer::clear() {
    // 同时清除颜色和深度
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}
