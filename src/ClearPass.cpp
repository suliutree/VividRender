#include "ClearPass.h"
#include "CommandBuffer.h"

ClearPass::ClearPass()
    : name("ClearPass")
{}

const std::string& ClearPass::getName() const {
    return name;
}

void ClearPass::execute(IDevice* device, ICommandBuffer* cmd) {
    // 只做一次 clear，不交换缓冲
    cmd->clear();
}
