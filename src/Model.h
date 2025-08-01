#pragma once
#include <vector>
#include <memory>
#include <glm/glm.hpp>
#include "IRenderResource.h"
#include "VertexBuffer.h"

struct Vertex {
    glm::vec3 pos;
    glm::vec3 normal;
    glm::vec2 uv;
};

class OpenGLIndexBuffer;   // 前向

/// 一整个 .obj（可能包含多个 Mesh）
class Model : public IRenderResource {
public:
    explicit Model(const std::string& path);
    ~Model() override = default;

    void initializeGL() override;      // 创建 GPU 资源

    struct SubMesh {
        std::shared_ptr<IVertexBuffer> vb;
        std::shared_ptr<OpenGLIndexBuffer> ib;   // 可选
        unsigned indexCount;
    };
    const std::vector<SubMesh>& getSubMeshes() const { return _meshes; }

private:
    std::string _path;
    std::vector<Vertex>   _verticesCPU;   // 临时缓冲
    std::vector<uint32_t> _indicesCPU;
    std::vector<SubMesh>  _meshes;        // GPU 资源
};
