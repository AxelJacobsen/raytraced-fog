#ifndef STRUCTS_H
#define STRUCTS_H

#include <glm/glm.hpp>
#include <vector>

struct AABB {
    glm::vec4 min{ FLT_MAX };
    glm::vec4 max{ -FLT_MAX };

    void expand(const glm::vec4& p) {
        min = glm::min(min, p);
        max = glm::max(max, p);
    }

    void expand(const AABB& b) {
        expand(b.min);
        expand(b.max);
    }
};

struct Triangle {
    glm::vec3 v0, v1, v2;
    glm::vec3 centroid;
    AABB bounds;
};

struct BVHNode {
    int firstTri = 0;
    int triCount = 0;
};

struct MeshGPU {
    int vertexOffset;
    int indexOffset;
    int indexCount;
    int bvhOffset;
    int bvhNodeCount;
    int vertexCount;
};

#endif