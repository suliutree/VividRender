#pragma once

// 使用强类型的枚举来标识资源，避免裸字符串带来的错误
enum class RenderResource {
    // 代表被清空的渲染目标
    ClearedRenderTarget,
    // 代表最终渲染完成的帧
    FinalFrame,
    // 未来可以扩展，例如：
    // GBufferAlbedo,
    // GBufferNormal,
    // ShadowMap,
    // LitScene
};