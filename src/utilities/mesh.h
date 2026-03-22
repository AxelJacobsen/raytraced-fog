#pragma once
#include <vector>
#include <string>
#include <glm/glm.hpp>
#include "structs.h"

struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 uv;
    glm::vec3 tangent;
    glm::vec3 bitangent;
};

class Mesh {
public:
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;

    bool loadFromFile(const std::string& path);
    std::vector<Triangle> Mesh::buildTriangles();
};