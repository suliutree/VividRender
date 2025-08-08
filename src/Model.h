#pragma once
#include <vector>
#include <memory>
#include <glm/glm.hpp>
#include <assimp/scene.h>
#include "IRenderResource.h"
#include "VertexBuffer.h"
#include "Texture2D.h"

struct Vertex {
    glm::vec3 pos;
    glm::vec3 normal;
    glm::vec2 uv;
};

class OpenGLIndexBuffer;
class ResourceManager;

/// 一整个 .obj（可能包含多个 Mesh）
class Model : public IRenderResource {
public:
    explicit Model(const std::string& path, ResourceManager* resMgr);
    ~Model() override = default;

    void initializeGL() override;

    struct SubMesh {
        std::shared_ptr<IVertexBuffer> vb;
        std::shared_ptr<OpenGLIndexBuffer> ib;   // 可选
        unsigned indexCount;
        std::shared_ptr<Texture2D> albedoTexture;
    };
    const std::vector<SubMesh>& getSubMeshes() const { return _meshes; }

    glm::vec3 getCenter() const { return _center; }

private:
    void processNode(aiNode* node, const aiScene* scene);
    SubMesh processMesh(aiMesh* mesh, const aiScene* scene);
    std::shared_ptr<Texture2D> loadMaterialTexture(
        aiMaterial* material,
        aiTextureType type,
        const std::string& typeName,
        const aiScene* scene
    );

    std::string _path;
    std::string _directory; // 模型所在目录
    // std::vector<Vertex>   _verticesCPU;   // 临时缓冲
    // std::vector<uint32_t> _indicesCPU;
    ResourceManager* _resMgr;
    std::vector<SubMesh>  _meshes;        // GPU 资源

    glm::vec3 _center;
};
