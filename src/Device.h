#pragma once
#include "CommandBuffer.h"
#include "IRenderResource.h"

class IDevice {
public:
    virtual ~IDevice() = default;

    /// 主线程调用：开始一帧的录制。
    /// 内部会等待渲染线程完成对可用资源的占用。
    /// 返回一个可供录制的命令缓冲区。
    virtual ICommandBuffer* beginFrame() = 0;

    /// 主线程调用：提交已录制完成的命令缓冲区。
    virtual void endFrame(ICommandBuffer* cmd) = 0;
    
    /// 请求关闭渲染器
    virtual void shutdown() = 0;

    /// 注册一个需要在渲染线程中创建 GL/GLSL/metal/VK 资源的对象
    virtual void registerResource(IRenderResource* res) = 0;
};
