#pragma once

#include <string>
#include <unordered_map>
#include <glad/glad.h>
#include "IRenderResource.h"

/// 封装一个简单的 Pipeline State Object（PSO）
class PipelineState : public IRenderResource {
public:
    /// 构造：传入顶点/片段着色器文件路径
    PipelineState(const std::string& vertPath, const std::string& fragPath);
    ~PipelineState();

    void initializeGL() override;

    GLuint getProgramID() const { return programID; }

    GLint getUniformLocation(const std::string& name);

private:
    std::string _vsPath, _fsPath;
    GLuint programID = 0;
    std::unordered_map<std::string, GLint> _uniformLocation;

    /// 辅助：链接 Program 并检查错误
    bool linkAndValidate(GLuint vertShader, GLuint fragShader);
};
