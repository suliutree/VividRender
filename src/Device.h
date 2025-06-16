#pragma once
#include "CommandBuffer.h"

class IDevice {
public:
    virtual ~IDevice() = default;
    /// 每帧开始时调用：可以做清屏之前的状态设置
    virtual void beginFrame() = 0;
    /// 获取当前帧使用的命令缓冲区接口
    virtual ICommandBuffer* getCommandBuffer() = 0;
    /// 每帧结束时调用：提交命令、交换缓冲
    virtual void endFrame() = 0;
};
