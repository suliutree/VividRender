#include "ResourceManager.h"
#include "Device.h"
#include "Pipeline.h"
#include "OpenGLVertexBuffer.h"
#include "Texture2D.h"
#include "Model.h"

#include <iostream>

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

std::shared_ptr<Model> ResourceManager::loadModel(const std::string& path)
{
    std::lock_guard<std::mutex> lk(_mutex);

    // 1. 查询缓存
    auto it = _modelCache.find(path);
    if (it != _modelCache.end()) {
        if (auto existing = it->second.lock()) {
            return existing;                    // 命中直接返回
        }
    }

    // 2. 未命中 → 创建新 Model（只解析文件，不做任何 GL 调用）
    auto model = std::make_shared<Model>(path);

    // 3. 交由渲染线程在正确的 Context 内完成 initializeGL()
    _device->registerResource(model.get());

    // 4. 写入缓存 & 返回
    _modelCache[path] = model;
    return model;
}
