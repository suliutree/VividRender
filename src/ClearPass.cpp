#include "ClearPass.h"
#include "CommandBuffer.h"

ClearPass::ClearPass()
    : name("ClearPass"),
      inputs({}),              // 无输入
      outputs({"Cleared"})     // 输出标记为 "Cleared"
{}

const std::string& ClearPass::getName() const {
    return name;
}

const std::vector<std::string>& ClearPass::getInputs() const {
    return inputs;
}

const std::vector<std::string>& ClearPass::getOutputs() const {
    return outputs;
}

void ClearPass::execute(IDevice* device, ICommandBuffer* cmd) {
    // 只做一次 clear，不交换缓冲
    cmd->clear();
}
