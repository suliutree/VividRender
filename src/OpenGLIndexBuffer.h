#pragma once
#include "IRenderResource.h"
#include <glad/glad.h>
#include <vector>
#include <cstdint>

/// 仅负责管理一个 EBO（index buffer）
class OpenGLIndexBuffer : public IRenderResource {
public:
    /// @param data  CPU 侧索引数据（字节序列）
    /// @param size  字节大小（indexCount * sizeof(GLuint)）
    OpenGLIndexBuffer(const void* data, unsigned int size);
    ~OpenGLIndexBuffer() override;

    /// 渲染线程调用：真正创建 EBO 并上传数据
    void initializeGL() override;

    /// 绑定到当前 VAO 的 GL_ELEMENT_ARRAY_BUFFER
    inline void bind() const { glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _EBO); }

private:
    std::vector<std::uint8_t> _cpuData;   // 暂存索引数据
    unsigned int _size = 0;               // 字节数
    GLuint _EBO = 0;                      // GPU 侧句柄
};
