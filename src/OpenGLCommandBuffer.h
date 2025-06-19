#pragma once
#include <glad/glad.h>
#include "CommandBuffer.h"
#include <vector>
#include <functional>

class OpenGLCommandBuffer : public ICommandBuffer {
public:
    OpenGLCommandBuffer() = default;
    virtual ~OpenGLCommandBuffer() = default;

    void clear() override;
    void draw(unsigned int vertexCount) override;

    void bindPipeline(GLuint programID) override;
    void unbindPipeline() override;
    void bindVertexArray(GLuint VAO) override;

    // --- 供渲染线程调用的新接口 ---
    /// 执行所有已录制的命令
    void executeAll();

    /// 清空录制的命令列表，为下一帧做准备
    void reset();

private:
    std::vector<std::function<void()>> _commands;
};
