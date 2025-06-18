#pragma once

/// 抽象顶点缓冲，只需 bind 即可
class IVertexBuffer {
public:
    virtual ~IVertexBuffer() = default;
    virtual void bind() const = 0;
};