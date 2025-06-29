#include "OpenGLVertexBuffer.h"
#include <cstring>

OpenGLVertexBuffer::OpenGLVertexBuffer(const void* data, unsigned int size) {
    // 把数据拷贝到 CPU 内存，等渲染线程用
    _size = size;
    _cpuData.resize(size);
    memcpy(_cpuData.data(), data, size);
}

void OpenGLVertexBuffer::initializeGL(){
    glGenVertexArrays(1, &_VAO);
    glGenBuffers(1, &_VBO);
    glBindVertexArray(_VAO);

    glBindBuffer(GL_ARRAY_BUFFER, _VBO);
    glBufferData(GL_ARRAY_BUFFER, _size, _cpuData.data(), GL_STATIC_DRAW);

    constexpr GLsizei stride = 5 * sizeof(float);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, stride, (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)(2 * sizeof(float)));

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindVertexArray(0);

    // 上传完后，可选地把 CPU 侧数据释放
    _cpuData.clear();
}

OpenGLVertexBuffer::~OpenGLVertexBuffer() {
    // 注意：这里的 delete 只能在已有 Context 的线程调用，渲染线程退出前再销毁
    if (_VBO) glDeleteBuffers(1, &_VBO);
    if (_VAO) glDeleteVertexArrays(1, &_VAO);
}
