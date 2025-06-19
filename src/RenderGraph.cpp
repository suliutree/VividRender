#include "RenderGraph.h"
#include "CommandBuffer.h"
#include "Device.h"
#include <unordered_set>
#include <iostream>

void RenderGraph::addPass(std::shared_ptr<RenderPass> pass) {
    passes.push_back(std::move(pass));
}

void RenderGraph::execute(IDevice* device, ICommandBuffer* cmd) {
    // 创建一个临时的 passes 列表副本或索引列表来进行拓扑排序和执行，
    // 而不是直接修改 this->passes。
    std::vector<std::shared_ptr<RenderPass>> remainingPasses = passes;
    std::unordered_set<std::string> available;

    size_t executedCount = 0;
    while (!remainingPasses.empty() && executedCount < passes.size()) {
        bool passExecutedInLoop = false;
        for (auto it = remainingPasses.begin(); it != remainingPasses.end(); ) {
            auto& pass = *it;
            bool ready = true;
            for (auto& in : pass->getInputs()) {
                if (available.find(in) == available.end()) {
                    ready = false;
                    break;
                }
            }

            if (ready) {
                pass->execute(device, cmd);
                for (auto& out : pass->getOutputs()) {
                    available.insert(out);
                }
                it = remainingPasses.erase(it); // 从临时列表中移除
                passExecutedInLoop = true;
            } else {
                ++it;
            }
        }
        
        // 如果一整轮循环没有任何 pass 被执行，说明有循环依赖，跳出以防死循环
        if (!passExecutedInLoop) {
            std::cerr << "RenderGraph Error: Circular dependency or missing inputs!" << std::endl;
            break;
        }
        executedCount++;
    }
}
