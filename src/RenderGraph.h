#pragma once
#include "RenderGraphDefs.h" // 引入新的资源定义
#include "RenderPass.h"
#include <memory>
#include <vector>

// 为了让 RenderResource 可用于 std::unordered_set，需要提供一个哈希函数
namespace std {
    template <>
    struct hash<RenderResource> {
        size_t operator()(const RenderResource& r) const {
            return static_cast<size_t>(r);
        }
    };
}

/// 简易的渲染图：按资源依赖排序执行各个 Pass
class RenderGraph {
public:
    // 新的 addPass 接口，明确地声明了此 Pass 实例的依赖关系
    void addPass(
        std::shared_ptr<RenderPass> pass,
        const std::vector<RenderResource>& inputs,
        const std::vector<RenderResource>& outputs
    );

    /// 接受一个命令缓冲区，并在其上录制所有 Pass 的命令
    void execute(IDevice* device, ICommandBuffer* cmd);

private:
    // 内部结构，用于存储 Pass 及其依赖关系
    struct PassNode {
        std::shared_ptr<RenderPass> pass;
        std::vector<RenderResource> inputs;
        std::vector<RenderResource> outputs;
    };

    std::vector<PassNode> nodes;
};
