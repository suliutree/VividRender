#pragma once
#include <string>
#include <vector>

class IDevice;
class ICommandBuffer;

/// 抽象一个渲染通道（Pass），记录它的依赖资源和执行函数
class RenderPass {
public:
    virtual ~RenderPass() = default;

    /// 通道名称（可用于日志或调试）
    virtual const std::string& getName() const = 0;

    /// 真正执行渲染逻辑
    virtual void execute(IDevice* device, ICommandBuffer* cmd) = 0;
};
