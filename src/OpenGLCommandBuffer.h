#pragma once
#include <glad/glad.h>
#include "CommandBuffer.h"

class OpenGLCommandBuffer : public ICommandBuffer {
public:
    OpenGLCommandBuffer() = default;
    virtual ~OpenGLCommandBuffer() = default;

    void clear() override;
};
