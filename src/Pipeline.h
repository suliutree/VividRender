#pragma once

#include <string>
#include <glad/glad.h>
#include "IGLResource.h"

/// 封装一个简单的 Pipeline State Object（PSO）
class PipelineState : public IGLResource {
public:
    /// 构造：传入顶点/片段着色器文件路径
    PipelineState(const std::string& vertPath, const std::string& fragPath);
    ~PipelineState();

    // /// 绑定（使用）该 Program
    // void bind() const;
    // /// 解绑（回到固定管线或 0）
    // void unbind() const;

    void initializeGL() override;

    GLuint getProgramID() const { return programID; }

private:
    std::string _vsPath, _fsPath;

    GLuint programID = 0;
    /// 辅助：链接 Program 并检查错误
    bool linkAndValidate(GLuint vertShader, GLuint fragShader);
};
