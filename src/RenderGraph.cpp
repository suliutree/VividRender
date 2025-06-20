#include "RenderGraph.h"
#include "CommandBuffer.h"
#include "Device.h"
#include <unordered_set>
#include <iostream>

void RenderGraph::addPass(
    std::shared_ptr<RenderPass> pass,
    const std::vector<RenderResource>& inputs,
    const std::vector<RenderResource>& outputs)
{
    // 将 Pass 和它的依赖关系一起打包成一个节点存入图中
    nodes.emplace_back(PassNode{std::move(pass), inputs, outputs});
}

void RenderGraph::execute(IDevice* device, ICommandBuffer* cmd) {
    std::vector<PassNode> remainingNodes = nodes;
    // 使用新的强类型资源枚举
    std::unordered_set<RenderResource> availableResources;

    // 拓扑排序和执行的整体逻辑不变
    size_t executedCount = 0;
    // 防止因循环依赖导致的无限循环
    size_t maxLoops = nodes.size(); 

    while (!remainingNodes.empty() && executedCount < maxLoops) {
        bool passExecutedInLoop = false;
        for (auto it = remainingNodes.begin(); it != remainingNodes.end(); ) {
            auto& node = *it;
            bool ready = true;
            // 检查当前 Pass 的所有输入资源是否都已就绪
            for (const auto& in_resource : node.inputs) {
                if (availableResources.find(in_resource) == availableResources.end()) {
                    ready = false;
                    break;
                }
            }

            if (ready) {
                // 执行 Pass
                node.pass->execute(device, cmd);
                // 将此 Pass 产生的所有输出资源标记为可用
                for (const auto& out_resource : node.outputs) {
                    availableResources.insert(out_resource);
                }
                // 从待执行列表中移除，并继续循环
                it = remainingNodes.erase(it);
                passExecutedInLoop = true;
            } else {
                ++it; // 继续检查下一个
            }
        }
        
        if (!passExecutedInLoop && !remainingNodes.empty()) {
            std::cerr << "RenderGraph Error: Circular dependency or missing inputs!" << std::endl;
            // 可以进一步打印出哪些资源缺失，以方便调试
            for(const auto& node : remainingNodes) {
                 for (const auto& in_resource : node.inputs) {
                    if (availableResources.find(in_resource) == availableResources.end()) {
                        // 此处可以添加从枚举到字符串的转换，方便打印
                        std::cerr << "  - Pass '" << node.pass->getName() << "' is waiting for a resource." << std::endl;
                        break;
                    }
                }
            }
            break;
        }
        executedCount++;
    }
}