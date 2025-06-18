#pragma once
#include "RenderPass.h"
#include <memory>
#include <vector>

/// 简易的渲染图：按资源依赖排序执行各个 Pass
class RenderGraph {
public:
    /// 注册一个通道
    void addPass(std::shared_ptr<RenderPass> pass);

    /// 依据依赖执行所有通道，并清空已执行列表
    void execute(IDevice* device);

private:
    std::vector<std::shared_ptr<RenderPass>> passes;
};
