#pragma once
#include "VertexBuffer.h"
#include "IRenderResource.h"
#include <glad/glad.h>
#include <vector>

class OpenGLVertexBuffer : public IVertexBuffer, public IRenderResource {
public:
    /// data: 顶点数据指针，size: 字节大小
    OpenGLVertexBuffer(const void* data, unsigned int size);
    ~OpenGLVertexBuffer() override;

    // void bind() const override;

    void initializeGL() override;

    GLuint getVAO() const { return _VAO; }

private:
    std::vector<uint8_t> _cpuData;
    unsigned int _size = 0;
    GLuint _VAO = 0, _VBO = 0;
};
