#include "Model.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "OpenGLVertexBuffer.h"
#include "OpenGLIndexBuffer.h"
#include "ResourceManager.h"

#include <memory>
#include <iostream>

Model::Model(const std::string& path) : _path(path) {}

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
    std::cout << " - Number of meshes: " << scene->mNumMeshes << std::endl;


    std::cout << "number meshes: " << scene->mNumMeshes << std::endl;

    _meshes.reserve(scene->mNumMeshes);

    for (unsigned m = 0; m < scene->mNumMeshes; ++m) {
        const aiMesh* mesh = scene->mMeshes[m];
        std::vector<Vertex> vertices;
        std::vector<uint32_t> indices;

        vertices.reserve(mesh->mNumVertices);
        for (unsigned i = 0; i < mesh->mNumVertices; ++i) {
            Vertex v;
            v.pos    = { mesh->mVertices[i].x,
                         mesh->mVertices[i].y,
                         mesh->mVertices[i].z };
            v.normal = { mesh->mNormals[i].x,
                         mesh->mNormals[i].y,
                         mesh->mNormals[i].z };
            v.uv     = mesh->HasTextureCoords(0) ?
                glm::vec2(mesh->mTextureCoords[0][i].x,
                          mesh->mTextureCoords[0][i].y) :
                glm::vec2(0.0f);
            vertices.emplace_back(v);
        }


        std::cout << "num vertices: " << mesh->mNumVertices << std::endl;

        for (unsigned f = 0; f < mesh->mNumFaces; ++f) {
            const aiFace& face = mesh->mFaces[f];
            indices.insert(indices.end(),
                           face.mIndices,
                           face.mIndices + face.mNumIndices);
        }

        auto vb = std::make_shared<OpenGLVertexBuffer>(
                      vertices.data(),
                      sizeof(Vertex) * vertices.size());
        auto ib = std::make_shared<OpenGLIndexBuffer>(
                      indices.data(),
                      sizeof(uint32_t) * indices.size());

        vb->initializeGL();
        ib->initializeGL();

        _meshes.emplace_back(SubMesh{
            std::static_pointer_cast<IVertexBuffer>(vb),
            ib,
            static_cast<unsigned int>(indices.size())
        });

        std::cout << "vertex size: " << vertices.size() << std::endl;
        std::cout << "index size: " << indices.size() << std::endl;
    }
}
