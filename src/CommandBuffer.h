#pragma once
#include <glad/glad.h>

class ICommandBuffer {
public:
    virtual ~ICommandBuffer() = default;
    /// 清除当前帧的渲染目标（Color/Depth 等）
    virtual void clear() = 0;
    /// 绘制，参数为顶点数量
    virtual void draw(unsigned int vertexCount) = 0;

    virtual void bindPipeline(GLuint programID) = 0;
    virtual void unbindPipeline() = 0;
    virtual void bindVertexArray(GLuint VAO) = 0;
};
