#include "RenderGraph.h"
#include "CommandBuffer.h"
#include "Device.h"
#include <unordered_set>

void RenderGraph::addPass(std::shared_ptr<RenderPass> pass) {
    passes.push_back(std::move(pass));
}

void RenderGraph::execute(IDevice* device) {
    std::unordered_set<std::string> available;  // 已就绪的资源集合
    ICommandBuffer* cmd = device->getCommandBuffer();

    // 简单的“随到随跑”拓扑排序
    size_t idx = 0;
    while (idx < passes.size()) {
        auto& pass = passes[idx];
        bool ready = true;
        for (auto& in : pass->getInputs()) {
            if (available.find(in) == available.end()) {
                ready = false;
                break;
            }
        }
        if (ready) {
            // 执行通道
            pass->execute(device, cmd);
            // 标记输出资源就绪
            for (auto& out : pass->getOutputs()) {
                available.insert(out);
            }
            // 从队列移除已执行通道
            passes.erase(passes.begin() + idx);
        } else {
            ++idx;
        }
    }
}
