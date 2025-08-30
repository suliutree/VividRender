#pragma once
#include <glad/glad.h>
#include <memory>

class OpenGLIndexBuffer;

class ICommandBuffer {
public:
    virtual ~ICommandBuffer() = default;
    /// 清除当前帧的渲染目标（Color/Depth 等）
    virtual void clear() = 0;
    /// 绘制，参数为顶点数量
    virtual void draw(unsigned int vertexCount) = 0;

    virtual void drawIndexed(std::shared_ptr<OpenGLIndexBuffer> ib, unsigned int indexCount) = 0;

    virtual void bindPipeline(GLuint programID) = 0;
    virtual void unbindPipeline() = 0;
    virtual void bindVertexArray(GLuint VAO) = 0;

    virtual void bindTexture(GLenum unit, GLuint textureID) = 0;
    virtual void setUniform1i(GLint location, GLint value) = 0;

    virtual void setUniformMat4(GLint location, const float* value) = 0;
    virtual void setUniformMat3(GLint location, const float* value) = 0;
    virtual void setUniform3f(GLint location, float x, float y, float z) = 0;

};
