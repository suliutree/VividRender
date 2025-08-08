#pragma once

#include <string>
#include <unordered_map>
#include <memory>
#include <mutex>

class IDevice;
class PipelineState;
class IVertexBuffer;
class Texture2D;
class Model;

class ResourceManager {
public:
    explicit ResourceManager(IDevice* device);
    ~ResourceManager();

    std::shared_ptr<PipelineState> loadPipeline(
        const std::string& vertPath,
        const std::string& fragPath
    );

    std::shared_ptr<IVertexBuffer> loadVertexBuffer(
        const void* data,
        unsigned int size
    );

    std::shared_ptr<Texture2D> loadTexture(const std::string& imagePath);
    std::shared_ptr<Texture2D> loadTextureFromMemory(
        const std::string& identifier,
        const unsigned char* data,
        int len
    );

    std::shared_ptr<Model> loadModel(const std::string& path);

private:
    IDevice* _device;  // 用于注册资源到渲染线程
    std::mutex _mutex; // 保护下面的多个 cache

    // Key 为 vertPath + "|" + fragPath
    std::unordered_map<std::string, std::weak_ptr<PipelineState>> _pipelineCache;

    std::unordered_map<size_t, std::weak_ptr<IVertexBuffer>> _vbCache;

    // Key 为图片路径
    std::unordered_map<std::string, std::weak_ptr<Texture2D>> _textureCache; 

    std::unordered_map<std::string, std::weak_ptr<Model>> _modelCache;
};