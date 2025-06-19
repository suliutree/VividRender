#pragma once
#include "RenderPass.h"
#include <memory>
#include <vector>

/// 简易的渲染图：按资源依赖排序执行各个 Pass
class RenderGraph {
public:
    /// 注册一个通道
    void addPass(std::shared_ptr<RenderPass> pass);

    /// 接受一个命令缓冲区，并在其上录制所有 Pass 的命令
    void execute(IDevice* device, ICommandBuffer* cmd);

private:
    std::vector<std::shared_ptr<RenderPass>> passes;
};
