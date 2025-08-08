#include "Model.h"
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include "OpenGLVertexBuffer.h"
#include "OpenGLIndexBuffer.h"
#include "ResourceManager.h"

#include <memory>
#include <iostream>
#include <algorithm> // std::replace
#include <sys/stat.h> // stat()

Model::Model(const std::string& path, ResourceManager* resMgr)
    : _path(path)
    , _resMgr(resMgr) 
{
    _directory = path.substr(0, path.find_last_of('/'));
}

void Model::initializeGL() {
    Assimp::Importer importer;
    const aiScene* scene =
        importer.ReadFile(_path, aiProcess_Triangulate |
                                 aiProcess_GenSmoothNormals |
                                 aiProcess_FlipUVs);
    if (!scene || !scene->HasMeshes())
        throw std::runtime_error("Assimp load failed: " + _path);

    glm::vec3 minBounds(std::numeric_limits<float>::max());
    glm::vec3 maxBounds(std::numeric_limits<float>::lowest());

    for (unsigned int m = 0; m < scene->mNumMeshes; ++m) {
        const aiMesh* mesh = scene->mMeshes[m];
        for (unsigned int i = 0; i < mesh->mNumVertices; ++i) {
            const aiVector3D& vert = mesh->mVertices[i];
            minBounds.x = std::min(minBounds.x, vert.x);
            minBounds.y = std::min(minBounds.y, vert.y);
            minBounds.z = std::min(minBounds.z, vert.z);

            maxBounds.x = std::max(maxBounds.x, vert.x);
            maxBounds.y = std::max(maxBounds.y, vert.y);
            maxBounds.z = std::max(maxBounds.z, vert.z);
        }
    }
    // Calculate the center of the AABB
    _center = (minBounds + maxBounds) * 0.5f;

    std::cout << "Model loaded: " << _path << std::endl;
    std::cout << " - Calculated Center: (" << _center.x << ", " << _center.y << ", " << _center.z << ")" << std::endl;

    processNode(scene->mRootNode, scene);

    // _meshes.reserve(scene->mNumMeshes);

    // for (unsigned m = 0; m < scene->mNumMeshes; ++m) {
    //     const aiMesh* mesh = scene->mMeshes[m];
    //     std::vector<Vertex> vertices;
    //     std::vector<uint32_t> indices;

    //     vertices.reserve(mesh->mNumVertices);
    //     for (unsigned i = 0; i < mesh->mNumVertices; ++i) {
    //         Vertex v;
    //         v.pos    = { mesh->mVertices[i].x,
    //                      mesh->mVertices[i].y,
    //                      mesh->mVertices[i].z };
    //         v.normal = { mesh->mNormals[i].x,
    //                      mesh->mNormals[i].y,
    //                      mesh->mNormals[i].z };
    //         v.uv     = mesh->HasTextureCoords(0) ?
    //             glm::vec2(mesh->mTextureCoords[0][i].x,
    //                       mesh->mTextureCoords[0][i].y) :
    //             glm::vec2(0.0f);
    //         vertices.emplace_back(v);
    //     }


    //     std::cout << "num vertices: " << mesh->mNumVertices << std::endl;

    //     for (unsigned f = 0; f < mesh->mNumFaces; ++f) {
    //         const aiFace& face = mesh->mFaces[f];
    //         indices.insert(indices.end(),
    //                        face.mIndices,
    //                        face.mIndices + face.mNumIndices);
    //     }

    //     auto vb = std::make_shared<OpenGLVertexBuffer>(
    //                   vertices.data(),
    //                   sizeof(Vertex) * vertices.size());
    //     auto ib = std::make_shared<OpenGLIndexBuffer>(
    //                   indices.data(),
    //                   sizeof(uint32_t) * indices.size());

    //     vb->initializeGL();
    //     ib->initializeGL();

    //     _meshes.emplace_back(SubMesh{
    //         std::static_pointer_cast<IVertexBuffer>(vb),
    //         ib,
    //         static_cast<unsigned int>(indices.size())
    //     });

    //     std::cout << "vertex size: " << vertices.size() << std::endl;
    //     std::cout << "index size: " << indices.size() << std::endl;
    // }
}

void Model::processNode(aiNode* node, const aiScene* scene) {
    
    for (unsigned int i = 0; i < node->mNumMeshes; i++) {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        _meshes.push_back(processMesh(mesh, scene));
    }

    for (unsigned int i = 0; i < node->mNumChildren; i++) {
        processNode(node->mChildren[i], scene);
    }

}

Model::SubMesh Model::processMesh(aiMesh* mesh, const aiScene* scene) {
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;

    // 顶点数据
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
                mesh->mTextureCoords[0][i].y
            );
        } else {
            vertex.uv = glm::vec2(0.0f);
        }

        vertices.push_back(vertex);
    }

    // 索引数据
    for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
        const aiFace& face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++) {
            indices.push_back(face.mIndices[j]);
        }
    }

    // 材质纹理
    std::shared_ptr<Texture2D> albedoTexture = nullptr;
    if (mesh->mMaterialIndex >= 0) {
        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
        albedoTexture = loadMaterialTexture(
            material,
            aiTextureType_DIFFUSE,
            "texture_diffuse",
            scene
        );
    }

    // 创建并初始化 OpenGL 缓冲
    auto vb = std::make_shared<OpenGLVertexBuffer>(
        vertices.data(),
        sizeof(Vertex) * vertices.size()
    );
    auto ib = std::make_shared<OpenGLIndexBuffer>(
        indices.data(),
        sizeof(uint32_t) * indices.size()
    );
    vb->initializeGL();
    ib->initializeGL();

    return SubMesh{
        std::static_pointer_cast<IVertexBuffer>(vb),
        ib,
        static_cast<unsigned int>(indices.size()),
        albedoTexture
    };
}

// std::shared_ptr<Texture2D> Model::loadMaterialTexture(
//     aiMaterial* material,
//     aiTextureType type,
//     const std::string& typeName,
//     const aiScene* scene
// ) {
//     if (material->GetTextureCount(type) == 0) {
//         return nullptr;
//     }

//     aiString str;
//     material->GetTexture(type, 0, &str);
//     std::string texturePath = str.C_Str();

//     // 嵌入式纹理 (*0, *1, …)
//     if (!texturePath.empty() && texturePath[0] == '*') {
//         // 解析索引，直接从 scene 中取出 aiTexture
//         int textureIndex = std::stoi(texturePath.substr(1));
//         aiTexture* embeddedTex = scene->mTextures[textureIndex];

//         // mHeight==0 表示压缩格式，mWidth 是数据长度
//         std::string identifier = _path + ":" + texturePath;
//         return _resMgr->loadTextureFromMemory(
//             identifier,
//             reinterpret_cast<unsigned char*>(embeddedTex->pcData),
//             embeddedTex->mWidth
//         );
//     }
//     else {
//         // 外部文件纹理
//         std::cout << "_directory: " << _directory << ", texturePath: " << texturePath << std::endl;
//         std::string fullPath = _directory + '/' + texturePath;
//         return _resMgr->loadTexture(fullPath);
//     }
// }


static bool fileExists(const std::string& path) {
    struct stat buffer;
    return (stat(path.c_str(), &buffer) == 0);
}

// 把所有 '\' 替换成 '/'
static inline std::string toUnixSlashes(std::string s) {
    std::replace(s.begin(), s.end(), '\\', '/');
    return s;
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

    // 嵌入式纹理
    if (!texturePath.empty() && texturePath[0] == '*') {
        int textureIndex = std::stoi(texturePath.substr(1));
        aiTexture* embeddedTex = scene->mTextures[textureIndex];
        std::string identifier = _path + ":" + texturePath;
        return _resMgr->loadTextureFromMemory(
            identifier,
            reinterpret_cast<unsigned char*>(embeddedTex->pcData),
            embeddedTex->mWidth
        );
    }

    // 只替换路径分隔符，不动 "../"
    std::string baseDir = toUnixSlashes(_directory);
    std::string relPath = toUnixSlashes(texturePath);

    std::string fullPath;
    if (relPath.size() > 1 && relPath[1] == ':') {
        // Windows 绝对路径
        fullPath = relPath;
    } else if (!relPath.empty() && relPath[0] == '/') {
        // Linux 绝对路径
        fullPath = relPath;
    } else {
        // 相对路径
        fullPath = baseDir + "/" + relPath;
    }

    // 如果找不到，回退到文件名查找
    if (!fileExists(fullPath)) {
        size_t slashPos = relPath.find_last_of('/');
        std::string filename = (slashPos == std::string::npos) ? relPath : relPath.substr(slashPos + 1);
        std::string altPath = baseDir + "/" + filename;
        if (fileExists(altPath)) {
            fullPath = altPath;
        }
    }

    std::cout << "_directory: " << baseDir
              << ", texturePath(raw): " << texturePath
              << ", resolved: " << fullPath << std::endl;

    return _resMgr->loadTexture(fullPath);
}