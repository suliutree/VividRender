#include "TrianglePass.h"
#include "Pipeline.h"
#include "VertexBuffer.h"
#include "CommandBuffer.h"
#include "OpenGLVertexBuffer.h"

TrianglePass::TrianglePass(PipelineState* p, IVertexBuffer* v)
    : name("TrianglePass"),
      pipeline(p),
      vb(v)
{}

const std::string& TrianglePass::getName() const {
    return name;
}

void TrianglePass::execute(IDevice* device, ICommandBuffer* cmd) {
    // 1. 绑定 Pipeline
    cmd->bindPipeline(pipeline->getProgramID());
    // 2. 绑定顶点阵列
    auto* glVB = static_cast<OpenGLVertexBuffer*>(vb);
    cmd->bindVertexArray(glVB->getVAO());
    // 3. 绘制三角形
    cmd->draw(3);
    // 4. 卸载 Program
    cmd->unbindPipeline();
}
