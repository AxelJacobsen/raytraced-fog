#pragma once
#include <vector>
#include <string>
#include <glm/glm.hpp>
#include "structs.h"
#include <assimp/scene.h>

struct Vertex {
    glm::vec4 position;
    glm::vec4 normal;
    glm::vec4 uv;
    glm::vec4 tangent;
    glm::vec4 bitangent;
};


class Mesh {
public:
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;
    std::vector<Texture> textures;
    int texId;

    bool loadFromFile(const std::string& path);
    std::vector<Triangle> Mesh::buildTriangles();
};