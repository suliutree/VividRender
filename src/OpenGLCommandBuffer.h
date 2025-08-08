#pragma once
#include <glad/glad.h>
#include "CommandBuffer.h"
#include <memory>
#include <vector>
#include <functional>

class OpenGLIndexBuffer;

class OpenGLCommandBuffer : public ICommandBuffer {
public:
    OpenGLCommandBuffer() = default;
    virtual ~OpenGLCommandBuffer() = default;

    void clear() override;
    void draw(unsigned int vertexCount) override;

    void drawIndexed(std::shared_ptr<OpenGLIndexBuffer> ib, unsigned int indexCount) override;

    void bindPipeline(GLuint programID) override;
    void unbindPipeline() override;
    void bindVertexArray(GLuint VAO) override;

    void bindTexture(GLenum unit, GLuint textureID) override;
    void setUniform1i(GLint location, GLint value) override;

    void setUniformMat4(GLint location, const float* value) override;
    void setUniform3f(GLint location, float x, float y, float z) override;

    // --- 供渲染线程调用的新接口 ---
    /// 执行所有已录制的命令
    void executeAll();

    /// 清空录制的命令列表，为下一帧做准备
    void reset();

    void setFrameIndex(int idx);
    int getFrameIndex() const;

private:
    int _frameIndex = -1;
    std::vector<std::function<void()>> _commands;
};
