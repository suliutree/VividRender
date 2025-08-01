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
