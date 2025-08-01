#include "OpenGLIndexBuffer.h"
#include <cstring>

OpenGLIndexBuffer::OpenGLIndexBuffer(const void* data, unsigned int size)
    : _size(size)
{
    // 把数据先存到 CPU 缓冲，等待渲染线程拿到正确的 OpenGL Context 后再上传
    _cpuData.resize(size);
    std::memcpy(_cpuData.data(), data, size);
}

void OpenGLIndexBuffer::initializeGL() {
    // 在拥有有效 GL Context 的线程执行
    glGenBuffers(1, &_EBO);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, _size, _cpuData.data(), GL_STATIC_DRAW);

    // 上传完即可释放 CPU 侧拷贝（如需重载可保留）
    _cpuData.clear();
    _cpuData.shrink_to_fit();

    // 不解绑也行（VAO 记录 EBO 绑定状态），为保持一致这里解绑
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

OpenGLIndexBuffer::~OpenGLIndexBuffer() {
    // 必须在仍持有有效 GL Context 的线程调用（通常是渲染线程退出前）
    if (_EBO)
        glDeleteBuffers(1, &_EBO);
}
