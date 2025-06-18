#pragma once
#include "VertexBuffer.h"
#include <glad/glad.h>

class OpenGLVertexBuffer : public IVertexBuffer {
public:
    /// data: 顶点数据指针，size: 字节大小
    OpenGLVertexBuffer(const void* data, unsigned int size);
    ~OpenGLVertexBuffer();

    void bind() const override;

private:
    unsigned int VAO, VBO;
};
