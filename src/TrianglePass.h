#pragma once
#include "RenderPass.h"
class PipelineState;
class IVertexBuffer;

/// 需要在构造时传入已创建好的管线和顶点缓冲
class TrianglePass : public RenderPass {
public:
    TrianglePass(PipelineState* pipeline, IVertexBuffer* vb);

    const std::string& getName() const override;
    const std::vector<std::string>& getInputs() const override;
    const std::vector<std::string>& getOutputs() const override;
    void execute(IDevice* device, ICommandBuffer* cmd) override;

private:
    std::string name;
    std::vector<std::string> inputs;
    std::vector<std::string> outputs;

    PipelineState* pipeline;
    IVertexBuffer* vb;
};
