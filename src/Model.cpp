#include "Model.h"
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include "OpenGLVertexBuffer.h"
#include "OpenGLIndexBuffer.h"
#include "ResourceManager.h"

#include <memory>
#include <iostream>
#include <algorithm>
#include <sys/stat.h>
#include <limits>            // ★ numeric_limits

// ---- 小工具：Assimp<->GLM 矩阵/向量互转 ----
static inline glm::mat4 aiToGlm(const aiMatrix4x4& m) {
    // 注意：glm 构造函数按列传参，这样写正好完成行列匹配
    return glm::mat4(
        m.a1, m.b1, m.c1, m.d1,
        m.a2, m.b2, m.c2, m.d2,
        m.a3, m.b3, m.c3, m.d3,
        m.a4, m.b4, m.c4, m.d4
    );
}

static bool fileExists(const std::string& path) {
    struct stat buffer;
    return (stat(path.c_str(), &buffer) == 0);
}

static inline std::string toUnixSlashes(std::string s) {
    std::replace(s.begin(), s.end(), '\\', '/');
    return s;
}

Model::Model(const std::string& path, ResourceManager* resMgr)
    : _path(path)
    , _resMgr(resMgr)
{
    // ★ 兼容两种分隔符
    size_t pos = path.find_last_of("/\\");
    _directory = (pos == std::string::npos) ? "" : path.substr(0, pos);
}

void Model::computeBoundsWithNodes(const aiScene* scene) {
    aiMatrix4x4 I; // identity
    aiVector3D minV(  std::numeric_limits<float>::max());
    aiVector3D maxV( -std::numeric_limits<float>::max());

    std::function<void(aiNode*, const aiMatrix4x4&)> dfs =
    [&](aiNode* node, const aiMatrix4x4& parent) {
        aiMatrix4x4 global = parent * node->mTransformation;

        for (unsigned i = 0; i < node->mNumMeshes; ++i) {
            aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
            for (unsigned v = 0; v < mesh->mNumVertices; ++v) {
                aiVector3D p = global * mesh->mVertices[v];
                minV.x = std::min(minV.x, p.x);
                minV.y = std::min(minV.y, p.y);
                minV.z = std::min(minV.z, p.z);
                maxV.x = std::max(maxV.x, p.x);
                maxV.y = std::max(maxV.y, p.y);
                maxV.z = std::max(maxV.z, p.z);
            }
        }
        for (unsigned c = 0; c < node->mNumChildren; ++c)
            dfs(node->mChildren[c], global);
    };
    dfs(scene->mRootNode, I);

    _center   = glm::vec3( (minV.x+maxV.x)*0.5f, (minV.y+maxV.y)*0.5f, (minV.z+maxV.z)*0.5f );
    _halfSize = glm::vec3( (maxV.x-minV.x)*0.5f, (maxV.y-minV.y)*0.5f, (maxV.z-minV.z)*0.5f );

    std::cout << " - Calculated Center: (" << _center.x << ", " << _center.y << ", " << _center.z << ")\n";
    std::cout << " - Half Size: (" << _halfSize.x << ", " << _halfSize.y << ", " << _halfSize.z << ")\n";
}

void Model::initializeGL() {
    Assimp::Importer importer;
    const aiScene* scene =
        importer.ReadFile(_path,
            aiProcess_Triangulate |
            aiProcess_GenSmoothNormals |
            aiProcess_FlipUVs
            // 如需切线：| aiProcess_CalcTangentSpace
            // 如需把节点矩阵烘焙进顶点：| aiProcess_PreTransformVertices
        );

    if (!scene || !scene->HasMeshes())
        throw std::runtime_error("Assimp load failed: " + _path);

    computeBoundsWithNodes(scene);

    // ★ 从根开始把全局矩阵往下传
    aiMatrix4x4 I; // identity
    processNode(scene->mRootNode, scene, I);
}

void Model::processNode(aiNode* node, const aiScene* scene, const aiMatrix4x4& parent) {
    aiMatrix4x4 global = parent * node->mTransformation;

    for (unsigned int i = 0; i < node->mNumMeshes; i++) {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        _meshes.push_back(processMesh(mesh, scene, global));
    }
    for (unsigned int i = 0; i < node->mNumChildren; i++) {
        processNode(node->mChildren[i], scene, global);
    }
}

Model::SubMesh Model::processMesh(aiMesh* mesh, const aiScene* scene, const aiMatrix4x4& globalXf) {
    std::vector<Vertex>    vertices;
    std::vector<uint32_t>  indices;

    vertices.reserve(mesh->mNumVertices);
    indices.reserve(mesh->mNumFaces * 3);

    // 顶点
    for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
        Vertex vertex;

        vertex.pos = {
            mesh->mVertices[i].x,
            mesh->mVertices[i].y,
            mesh->mVertices[i].z
        };

        if (mesh->HasNormals()) {
            vertex.normal = {
                mesh->mNormals[i].x,
                mesh->mNormals[i].y,
                mesh->mNormals[i].z
            };
        } else {
            vertex.normal = glm::vec3(0.0f);
        }

        if (mesh->HasTextureCoords(0)) {
            vertex.uv = glm::vec2(
                mesh->mTextureCoords[0][i].x,
                mesh->mTextureCoords[0][i].y   // aiProcess_FlipUVs 已处理 V 翻转
            );
        } else {
            vertex.uv = glm::vec2(0.0f);
        }

        vertices.push_back(vertex);
    }

    // 索引（Triangulate 后应为 3）
    for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
        const aiFace& face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++) {
            indices.push_back(face.mIndices[j]);
        }
    }

    // 材质纹理：优先 BASE_COLOR，再回退 DIFFUSE
    std::shared_ptr<Texture2D> albedoTexture = nullptr;
    if (mesh->mMaterialIndex >= 0) {
        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

        albedoTexture = loadMaterialTexture(material, aiTextureType_BASE_COLOR, "baseColor", scene);
        if (!albedoTexture) {
            albedoTexture = loadMaterialTexture(material, aiTextureType_DIFFUSE, "texture_diffuse", scene);
        }
    }

    // 创建并初始化 OpenGL 缓冲
    auto vb = std::make_shared<OpenGLVertexBuffer>(vertices.data(),
                                                   sizeof(Vertex) * vertices.size());
    auto ib = std::make_shared<OpenGLIndexBuffer>(indices.data(),
                                                  sizeof(uint32_t) * indices.size());
    vb->initializeGL();
    ib->initializeGL();

    SubMesh sm;
    sm.vb = std::static_pointer_cast<IVertexBuffer>(vb);
    sm.ib = ib;
    sm.indexCount = static_cast<unsigned int>(indices.size());
    sm.albedoTexture = albedoTexture;
    sm.localTransform = aiToGlm(globalXf);      // ★ 保存该子网格的全局矩阵
    return sm;
}

std::shared_ptr<Texture2D> Model::loadMaterialTexture(
    aiMaterial* material,
    aiTextureType type,
    const std::string& typeName,
    const aiScene* scene
) {
    if (material->GetTextureCount(type) == 0) return nullptr;

    aiString str;
    material->GetTexture(type, 0, &str);
    std::string texturePath = str.C_Str();

    // ── 1) 嵌入式纹理 ─────────────────────────────
    if (!texturePath.empty() && texturePath[0] == '*') {
        int textureIndex = std::stoi(texturePath.substr(1));
        aiTexture* embeddedTex = scene->mTextures[textureIndex];
        std::string identifier = _path + ":" + texturePath;

        if (embeddedTex->mHeight == 0) {
            // ★ 压缩图（PNG/JPG...）：mWidth 是字节数
            const unsigned char* bytes = reinterpret_cast<const unsigned char*>(embeddedTex->pcData);
            size_t sizeBytes = static_cast<size_t>(embeddedTex->mWidth);
            return _resMgr->loadTextureFromMemory(identifier, bytes, sizeBytes);
        } else {
            // ★ 未压缩 BGRA8：需要转 RGBA 再创建纹理
            const int w = static_cast<int>(embeddedTex->mWidth);
            const int h = static_cast<int>(embeddedTex->mHeight);
            const aiTexel* texels = embeddedTex->pcData;

            std::vector<unsigned char> rgba;
            rgba.resize(static_cast<size_t>(w) * static_cast<size_t>(h) * 4);
            for (int i = 0; i < w * h; ++i) {
                // aiTexel: bgra（每个分量 0..255）
                rgba[i * 4 + 0] = texels[i].r;
                rgba[i * 4 + 1] = texels[i].g;
                rgba[i * 4 + 2] = texels[i].b;
                rgba[i * 4 + 3] = texels[i].a;
            }

            // 需要 ResourceManager 提供从 RGBA 像素创建纹理的方法
            // 示例：loadTextureFromRGBA(identifier, w, h, rgba.data())
            return _resMgr->loadTextureFromRGBA(identifier, w, h, rgba.data());
        }
    }

    // ── 2) 外部文件纹理（相对路径/绝对路径） ────────
    std::string baseDir = toUnixSlashes(_directory);
    std::string relPath = toUnixSlashes(texturePath);

    std::string fullPath;
    if (relPath.size() > 1 && relPath[1] == ':') {
        // Windows 绝对路径（如 C:/...）
        fullPath = relPath;
    } else if (!relPath.empty() && relPath[0] == '/') {
        // *nix 绝对路径
        fullPath = relPath;
    } else {
        // 相对路径（保留 ../ 语义）
        fullPath = baseDir.empty() ? relPath : (baseDir + "/" + relPath);
    }

    // 找不到则回退到“同目录仅文件名”
    if (!fileExists(fullPath)) {
        size_t slashPos = relPath.find_last_of('/');
        std::string filename = (slashPos == std::string::npos) ? relPath : relPath.substr(slashPos + 1);
        std::string altPath = baseDir.empty() ? filename : (baseDir + "/" + filename);
        if (fileExists(altPath)) {
            fullPath = altPath;
        }
    }

    std::cout << "_directory: " << baseDir
              << ", texturePath(raw): " << texturePath
              << ", resolved: " << fullPath << std::endl;

    return _resMgr->loadTexture(fullPath);
}
