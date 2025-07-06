#include "ResourceManager.h"
#include "Device.h"
#include "Pipeline.h"
#include "OpenGLVertexBuffer.h"
#include "Texture2D.h"  // 你的纹理类

#include <functional>
// #include <xxhash.h>     // 如果需要高效哈希，可用 XXHash 等

ResourceManager::ResourceManager(IDevice* device)
    : _device(device)
{}

ResourceManager::~ResourceManager() = default;

std::shared_ptr<PipelineState> ResourceManager::loadPipeline(
    const std::string& vertPath,
    const std::string& fragPath
) {
    // 生成 key
    std::string key = vertPath + "|" + fragPath;
    std::lock_guard<std::mutex> lk(_mutex);

    // 查缓存
    auto it = _pipelineCache.find(key);
    if (it != _pipelineCache.end()) {
        if (auto existing = it->second.lock()) {
            return existing;
        }
    }

    // 缓存失效或不存在 —— 新建
    auto pipeline = std::make_shared<PipelineState>(vertPath, fragPath);
    // 注册到渲染线程：在正确的 GL Context 中调用 initializeGL()
    _device->registerResource(pipeline.get());

    _pipelineCache[key] = pipeline;
    return pipeline;
}

std::shared_ptr<IVertexBuffer> ResourceManager::loadVertexBuffer(
    const void* data,
    unsigned int size
) {
    // 简化示例：用 size 作为 key；实际可对 data 内容做哈希
    size_t key = std::hash<unsigned int>{}(size);
    std::lock_guard<std::mutex> lk(_mutex);

    auto it = _vbCache.find(key);
    if (it != _vbCache.end()) {
        if (auto existing = it->second.lock())
            return existing;
    }

    auto impl = std::make_shared<OpenGLVertexBuffer>(data, size);
    // 把具体对象注册到渲染线程（GL Context）
    _device->registerResource(impl.get());

    // upcast 到基类接口
    std::shared_ptr<IVertexBuffer> vb = impl;
    _vbCache[key] = vb;
    return vb;
}

std::shared_ptr<Texture2D> ResourceManager::loadTexture(const std::string& imagePath) {
    std::lock_guard<std::mutex> lk(_mutex);

    auto it = _textureCache.find(imagePath);
    if (it != _textureCache.end()) {
        if (auto existing = it->second.lock())
            return existing;
    }

    auto tex = std::make_shared<Texture2D>(imagePath);
    _device->registerResource(tex.get());
    _textureCache[imagePath] = tex;
    return tex;
}
