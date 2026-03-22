#include "Mesh.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

bool Mesh::loadFromFile(const std::string& path) {
    Assimp::Importer importer;

    const aiScene* scene = importer.ReadFile(path,
        aiProcess_Triangulate |
        aiProcess_GenNormals |
        aiProcess_CalcTangentSpace |
        aiProcess_JoinIdenticalVertices);

    if (!scene || !scene->HasMeshes()) return false;


    for (unsigned int m = 0; m < scene->mNumMeshes; m++) {
        aiMesh* mesh = scene->mMeshes[m];

        unsigned int vertexOffset = 0;

        // Add vertices
        for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
            Vertex v{};
            v.position = glm::vec3(mesh->mVertices[i].x,
                mesh->mVertices[i].y,
                mesh->mVertices[i].z);
            if (mesh->HasNormals()) v.normal = glm::vec3(mesh->mNormals[i].x,
                mesh->mNormals[i].y,
                mesh->mNormals[i].z);
            if (mesh->mTextureCoords[0]) v.uv = glm::vec2(mesh->mTextureCoords[0][i].x,
                mesh->mTextureCoords[0][i].y);
            if (mesh->HasTangentsAndBitangents()) {
                v.tangent = glm::vec3(mesh->mTangents[i].x,
                    mesh->mTangents[i].y,
                    mesh->mTangents[i].z);
                v.bitangent = glm::vec3(mesh->mBitangents[i].x,
                    mesh->mBitangents[i].y,
                    mesh->mBitangents[i].z);
            }
            vertices.push_back(v);
        }

        // Add indices with offset
        for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
            aiFace face = mesh->mFaces[i];
            for (unsigned int j = 0; j < face.mNumIndices; j++) {
                indices.push_back(face.mIndices[j] + vertexOffset);
            }
        }
    }

    return true;
}

std::vector<Triangle> Mesh::buildTriangles() {
    std::vector<Triangle> tris;
    tris.reserve(indices.size() / 3);

    for (size_t i = 0; i < indices.size(); i += 3) {
        Triangle t;

        const auto& v0 = vertices[indices[i + 0]];
        const auto& v1 = vertices[indices[i + 1]];
        const auto& v2 = vertices[indices[i + 2]];

        t.v0 = v0.position;
        t.v1 = v1.position;
        t.v2 = v2.position;

        t.centroid = (t.v0 + t.v1 + t.v2) / 3.0f;

        t.bounds.expand(t.v0);
        t.bounds.expand(t.v1);
        t.bounds.expand(t.v2);

        tris.push_back(t);
    }

    return tris;
}