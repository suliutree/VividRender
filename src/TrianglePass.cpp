#include "TrianglePass.h"
#include "Pipeline.h"
#include "VertexBuffer.h"
#include "CommandBuffer.h"

TrianglePass::TrianglePass(PipelineState* p, IVertexBuffer* v)
    : name("TrianglePass"),
      inputs({"Cleared"}),     // 依赖 ClearPass
      outputs({"Frame"}),      // 标记完成
      pipeline(p),
      vb(v)
{}

const std::string& TrianglePass::getName() const {
    return name;
}

const std::vector<std::string>& TrianglePass::getInputs() const {
    return inputs;
}

const std::vector<std::string>& TrianglePass::getOutputs() const {
    return outputs;
}

void TrianglePass::execute(IDevice* device, ICommandBuffer* cmd) {
    pipeline->bind();
    vb->bind();
    cmd->draw(3);
    pipeline->unbind();
}
