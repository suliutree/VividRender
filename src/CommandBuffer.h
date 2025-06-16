#pragma once

class ICommandBuffer {
public:
    virtual ~ICommandBuffer() = default;
    /// 清除当前帧的渲染目标（Color/Depth 等）
    virtual void clear() = 0;
};
