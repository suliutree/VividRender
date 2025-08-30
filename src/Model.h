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

class Model : public IRenderResource {
public:
    explicit Model(const std::string& path, ResourceManager* resMgr);
    ~Model() override = default;

    void initializeGL() override;

    struct SubMesh {
        std::shared_ptr<IVertexBuffer> vb;
        std::shared_ptr<OpenGLIndexBuffer> ib; 
        unsigned indexCount;
        std::shared_ptr<Texture2D> albedoTexture;
        glm::mat4 localTransform {1.0f};
    };
    const std::vector<SubMesh>& getSubMeshes() const { return _meshes; }

    glm::vec3 getCenter() const { return _center; }
    glm::vec3 getHalfSize() const { return _halfSize; }

private:
    void computeBoundsWithNodes(const aiScene* scene);
    void processNode(aiNode* node, const aiScene* scene, const aiMatrix4x4& parent);
    SubMesh processMesh(aiMesh* mesh, const aiScene* scene, const aiMatrix4x4& globalXf);
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
    glm::vec3 _halfSize;
};
